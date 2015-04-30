#pragma once

#include <QString>
#include "model.h"

class Platform : public Model {
	public:
		Platform();
		Platform(int id, QString name, QString command, QString icon, QString background);

		int id;
		QString name;
		QString command;
		QString icon;
		QString background;
};
