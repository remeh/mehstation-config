#include <QDebug>
#include <QSqlDatabase>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QString>
#include <QVariant>
#include <iostream>

#include "db.h"
#include "models/executable.h"
#include "models/executable_resource.h"
#include "models/platform.h"

Database::Database() : opened(false) {
}

Database::~Database() {
}

bool Database::open(const QString& filename) {
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(filename);
	if (!db.open()) {
		QMessageBox msgBox(QMessageBox::Critical, "Error", "Can't open the database.");
		msgBox.exec();
	} else {
		opened = true;
	}
	std::cout << "Database: '" << filename.toStdString() << "' opened." << std::endl;

	// Tries a query to ensure the validity of the database
	QSqlQuery q;
	q.exec("select * from platform");
	q.next();
	const QSqlError& error = q.lastError();
	if (error.isValid()) {
		// The database looks to be in an inconsistent state.
		// We should try to create it.
		// TODO create the database.db file.
		
		opened = false;
	}

	return opened;
}

// getPlatforms returns all the platforms available in the
// database.
// Memory should be freed by the caller.
// Returns nullptr if an error occurred.
QList<Platform>* Database::getPlatforms() {
	QSqlQuery q;
	q.exec("select id, name, command, icon, background FROM platform ORDER BY id");
	QList<Platform>* result = new QList<Platform>;
	while (q.next()) {
		if (!q.isValid()) {
			std::cerr << "Unable to retrieve the platforms from the database." << std::endl;
			delete result;
			return nullptr;
		}

		QSqlRecord record = q.record();
		QSqlField id = record.field("id");
		QSqlField name = record.field("name");
		QSqlField command = record.field("command");
		QSqlField icon = record.field("icon");
		QSqlField background = record.field("background");

		result->append(Platform(
						id.value().toInt(),
						name.value().toString(),
						command.value().toString(),
						icon.value().toString(),
						background.value().toString()
						));
	}
	q.clear();
	return result;
}

// getExecutable returns all the executables for the given platform.
// Memory should be free by the caller.
// Returns nullptr if an error occurred.
QList<Executable>* Database::getExecutables(int platformId) {
	QSqlQuery q;
	q.prepare("select id, platform_id, display_name, filepath, description, genres, players, publisher, developer, release_date, rating FROM executable where platform_id = :platform_id ORDER BY display_name");
	q.bindValue(":platform_id", platformId);
	q.exec();
	QList<Executable>* result = new QList<Executable>;
	while (q.next()) {
		if (!q.isValid()) {
			qWarning() << "Unable to retrieve the executables of the platform id" << platformId;
			delete result;
			return nullptr;
		}

		QSqlRecord record = q.record();
		QSqlField id = record.field("id");
		QSqlField platformId = record.field("platform_id");
		QSqlField display_name = record.field("display_name");
		QSqlField filepath = record.field("filepath");
		QSqlField description = record.field("description");
		QSqlField genres = record.field("genres");
		QSqlField players = record.field("players");
		QSqlField publisher = record.field("publisher");
		QSqlField developer = record.field("developer");
		QSqlField release_date = record.field("release_date");
		QSqlField rating = record.field("rating");

		Executable e = Executable(
						id.value().toInt(),
						platformId.value().toInt(),
						display_name.value().toString(),
						filepath.value().toString(),
						description.value().toString(),
						genres.value().toString(),
						players.value().toString(),
						publisher.value().toString(),
						developer.value().toString(),
						release_date.value().toString(),
						rating.value().toString()
						);

		e.resources = this->getExecutableResources(e.id);
		result->append(e);
	}
	q.clear();
	return result;
}

QList<ExecutableResource> Database::getExecutableResources(int executableId) {
	QSqlQuery q;
	q.prepare("select id, executable_id, filepath, type from executable_resource where executable_id = :executable_id");
	q.bindValue(":executable_id", executableId);
	q.exec();

	QList<ExecutableResource> result = QList<ExecutableResource>();

	QSqlError error = q.lastError();
	if (error.isValid()) {
		QMessageBox msgBox(QMessageBox::Critical, "Error", QString("Can't load the executable resources: ").append(error.text()));
		msgBox.exec();
		qCritical() << "Error while loading executable resources:" << error.text();
		return result;
	}

	while (q.next()) {
		QSqlRecord record = q.record();
		QSqlField id = record.field("id");
		QSqlField executable_id = record.field("executable_id");
		QSqlField filepath = record.field("filepath");
		QSqlField type = record.field("type");

		ExecutableResource er;
		er.id = id.value().toInt();
		er.executableId = executable_id.value().toInt();
		er.filepath = filepath.value().toString();
		er.type = type.value().toString();
		result.append(er);
	}
	q.clear();
	return result;
}

void Database::deleteExecutable(Executable executable) {
	if (executable.id == -1) {
		return;
	}
	
	// first we want to delete each resource.
	// for cleaner code, we'll reuse deleteResource
	for (int i = 0; i < executable.resources.count(); i++) {
		ExecutableResource res = executable.resources.at(i);
		this->deleteResource(res.id);
	}

	// finally delete the executable
	QSqlQuery q;
	q.prepare("delete from executable where id = :exec_id");
	q.bindValue(":exec_id", executable.id);
	q.exec();
	QSqlError error = q.lastError();
	if (error.isValid()) {
		QMessageBox msgBox(QMessageBox::Critical, "Error", QString("Error while deleting an executable : ").append(error.text()));
		msgBox.exec();
		qCritical() << "Error while deleting an executable:" << error.text();
	}
	q.clear();
}

void Database::deleteResource(int resourceId) {
	QSqlQuery q;
	q.prepare("delete from executable_resource where id = :resource_id");
	q.bindValue(":resource_id", resourceId);
	q.exec();
	QSqlError error = q.lastError();
	if (error.isValid()) {
		QMessageBox msgBox(QMessageBox::Critical, "Error", QString("Can't delete an executable resource: ").append(error.text()));
		msgBox.exec();
		qCritical() << "Error while deleting an executable resource:" << error.text();
	}
	q.clear();
}

Executable Database::createNewExecutable(int platformId) {
	Executable executable;

	QSqlQuery q;
	q.prepare("insert into executable (platform_id, display_name) VALUES (:platform_id, \"New executable\")");
	q.bindValue(":platform_id", platformId);
	q.exec();
	QSqlError error = q.lastError();
	if (error.isValid()) {
		QMessageBox msgBox(QMessageBox::Critical, "Error", QString("Can't create a new executable: ").append(error.text()));
		msgBox.exec();
		qCritical() << "Error while creating an executable:" << error.text();
		return executable;
	}

	QVariant lastId = q.lastInsertId();
	executable.id = lastId.toInt();
	executable.platformId = platformId;
	executable.displayName = "New executable";
	qDebug() << "New executable id:" << executable.id;

	q.clear();
	return executable;
}

ExecutableResource Database::createNewResource(int executableId) {
	ExecutableResource res;

	QSqlQuery q;
	q.prepare("insert into executable_resource (executable_id) VALUES (:executable_id)");
	q.bindValue(":executable_id", executableId);
	q.exec();
	QSqlError error = q.lastError();
	if (error.isValid()) {
		QMessageBox msgBox(QMessageBox::Critical, "Error", QString("Can't create a new executable resource: ").append(error.text()));
		msgBox.exec();
		qCritical() << "Error while creating an executable resource:" << error.text();
		return res;
	}

	QVariant lastId = q.lastInsertId();
	res.id = lastId.toInt();
	qDebug() << "New resource id:" << res.id;
	res.executableId = executableId;

	q.clear();
	return res;
}

void Database::update(ExecutableResource resource) {
	QSqlQuery q;
	q.prepare("update executable_resource set filepath = :filepath, type = :type where id = :id");
	q.bindValue(":filepath", resource.filepath);
	q.bindValue(":type", resource.type);
	q.bindValue(":id", resource.id);
	q.exec();
	QSqlError error = q.lastError();
	if (error.isValid()) {
		QMessageBox msgBox(QMessageBox::Critical, "Error", QString("Can't update the executable resource: ").append(error.text()));
		msgBox.exec();
		qCritical() << "Error while updating an executable resource:" << error.text();
	}
	q.clear();
}

void Database::update(Executable executable) {
	QSqlQuery q;
	q.prepare("update executable set display_name = :display_name, filepath = :filepath, description = :description, genres = :genres, players = :players, publisher = :publisher, developer = :developer, release_date = :release_date, rating = :rating where id = :id");
	q.bindValue(":display_name", executable.displayName);
	q.bindValue(":filepath", executable.filepath);
	q.bindValue(":description", executable.description);
	q.bindValue(":genres", executable.genres);
	q.bindValue(":players", executable.players);
	q.bindValue(":publisher", executable.publisher);
	q.bindValue(":developer", executable.developer);
	q.bindValue(":release_date", executable.releaseDate);
	q.bindValue(":rating", executable.rating);
	q.bindValue(":id", executable.id);
	q.exec();
	QSqlError error = q.lastError();
	if (error.isValid()) {
		QMessageBox msgBox(QMessageBox::Critical, "Error", QString("Can't update the executable: ").append(error.text()));
		msgBox.exec();
		qCritical() << "Error while updating an executable:" << error.text();
	}
	q.clear();
}
