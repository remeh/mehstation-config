#pragma once

#include <QList>
#include <QSqlDatabase>
#include <QString>

#include "models/executable.h"
#include "models/platform.h"

class Database {
	public:
		Database();
		~Database();

		bool open(const QString& filename);

		QList<Platform>* getPlatforms();
		QList<Executable>* getExecutables(int platformId);

		void update(Executable executablem);

	private:
		QSqlDatabase db;
		bool opened;
};
