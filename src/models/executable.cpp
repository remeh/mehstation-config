#include <QString>
#include "executable.h"

Executable::Executable() {
}

Executable::Executable(int id, QString displayName, QString filepath, QString description, QString genres,
		QString players, QString publisher, QString developer, QString releaseDate, QString rating) :
	id(id),
	displayName(displayName),
	filepath(filepath),
	description(description),
	genres(genres),
	players(players),
	publisher(publisher),
	developer(developer),
	releaseDate(releaseDate),
	rating(rating) {
}

