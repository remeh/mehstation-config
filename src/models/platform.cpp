#include <QDebug>
#include <QString>
#include "platform.h"

Platform::Platform() :
	id(-1) {
}

Platform::Platform(int id, QString name, QString command, QString icon, QString background) :
	id(id),
	name(name),
	command(command),
	icon(icon),
	background(background) {
	qDebug() << "Platform" << name << "loaded from DB.";
}
