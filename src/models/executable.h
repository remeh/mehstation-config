#pragma once

#include <QString>
#include "model.h"

class Executable : public Model {
	public:
		Executable();
		Executable(int id, QString displayName, QString filepath, QString description, QString genres, 
				QString players, QString publisher, QString developer, QString releaseDate, QString rating);

		int id;
		QString displayName;
		QString filepath;
		QString description;
		QString genres;
		QString players;
		QString publisher;
		QString developer;
		QString releaseDate;
		QString rating;

		// TODO resources
};

