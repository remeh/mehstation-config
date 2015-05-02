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
	}

	return opened;
}

// getPlatforms returns all the platforms available in the
// database.
// Memory should be freed by the caller.
// Returns nullptr if an error occurred.
QList<Platform>* Database::getPlatforms() {
	QSqlQuery q;
	q.exec("select id, name, command, icon, background FROM platform");
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
	q.prepare("select id, display_name, filepath, description, genres, players, publisher, developer, release_date, rating FROM executable where platform_id = :platform_id");
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
		QSqlField display_name = record.field("display_name");
		QSqlField filepath = record.field("filepath");
		QSqlField description = record.field("description");
		QSqlField genres = record.field("genres");
		QSqlField players = record.field("players");
		QSqlField publisher = record.field("publisher");
		QSqlField developer = record.field("developer");
		QSqlField release_date = record.field("release_date");
		QSqlField rating = record.field("rating");

		result->append(Executable(
						id.value().toInt(),
						display_name.value().toString(),
						filepath.value().toString(),
						description.value().toString(),
						genres.value().toString(),
						players.value().toString(),
						publisher.value().toString(),
						developer.value().toString(),
						release_date.value().toString(),
						rating.value().toString()
						));
	}
	q.clear();
	return result;
}
