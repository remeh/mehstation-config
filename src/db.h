#pragma once

#include <QList>
#include <QSqlDatabase>
#include <QString>
#include "models/platform.h"

class Database {
	public:
		Database();
		~Database();

		bool open(const QString& filename);

		QList<Platform>* getPlatforms();

	private:
		QSqlDatabase db;
		bool opened;
};
