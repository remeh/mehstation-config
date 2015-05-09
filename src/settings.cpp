#include <QDebug>
#include <QFile>
#include <QStackedLayout>
#include <QUiLoader>
#include <QWidget>
#include "settings.h"

Settings::Settings(App* app, QWidget* parent) :
	QWidget(parent),
	mainWidget(nullptr),
	app(app) {
	QUiLoader loader;

	QFile uiFile("res/settings.ui");
	uiFile.open(QFile::ReadOnly);

	this->mainWidget = loader.load(&uiFile, NULL);
	this->setLayout(new QStackedLayout());
	this->layout()->addWidget(this->mainWidget);
}

Settings::~Settings() {

}
