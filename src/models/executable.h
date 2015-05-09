#pragma once

#include <QList>
#include <QString>
#include "model.h"
#include "executable_resource.h"

class Executable : public Model {
	public:
		Executable();
		Executable(int id, int platformId, QString displayName, QString filepath, QString description, QString genres,
				QString players, QString publisher, QString developer, QString releaseDate, QString rating);

		int id;
		int platformId;
		QString displayName;
		QString filepath;
		QString description;
		QString genres;
		QString players;
		QString publisher;
		QString developer;
		QString releaseDate;
		QString rating;

		QList<ExecutableResource> resources;
}; 
