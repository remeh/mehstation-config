#pragma once

#include <QList>
#include <QSqlDatabase>
#include <QString>

#include "models/executable.h"
#include "models/executable_resource.h"
#include "models/platform.h"

class Database {
	public:
		Database();
		~Database();

		bool open(const QString& filename);

		QList<Platform>* getPlatforms();
		QList<Executable>* getExecutables(int platformId);
		QList<ExecutableResource> getExecutableResources(int executableId);
		ExecutableResource createNewResource(int executableId);
		void deleteExecutable(Executable executable);
		void deleteResource(int resourceId);

		void update(Executable executable);
		void update(ExecutableResource resource);

	private:
		QSqlDatabase db;
		bool opened;
};
