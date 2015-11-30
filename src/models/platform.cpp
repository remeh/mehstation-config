#include <QDebug>
#include <QString>
#include "platform.h"

Platform::Platform() :
	id(-1) {
}

Platform::Platform(int id, QString name, QString command, QString icon, QString background, QString view) :
	id(id),
	name(name),
	command(command),
	icon(icon),
	background(background),
	view(view) {
	qDebug() << "Platform" << name << "loaded from DB.";
}
