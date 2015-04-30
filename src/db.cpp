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
// Returns nullptr if an error occured.
QList<Platform>* Database::getPlatforms() {
	QSqlQuery q;
	q.exec("select id, name, command, icon, background FROM platform");
	QList<Platform>* result = new(QList<Platform>);
	while (q.next()) {
		if (!q.isValid()) {
			std::cerr << "Unable to retrieve the platforms from the database." << std::endl;
			delete(result);
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
