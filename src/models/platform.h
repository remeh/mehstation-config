#pragma once

#include <QList>
#include <QString>
#include "model.h"

class Platform : public Model {
	public:
		Platform();
		Platform(int id, QString name, QString command, QString icon, QString background, QString view);

		int id;
		QString name;
		QString command;
		QString icon;
		QString background;
		QString view;
};
