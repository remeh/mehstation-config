#include <QtUiTools>
#include <QWidget>
#include <iostream>
#include "app.h"

App::App(int & argc, char** argv) : QApplication(argc,argv) {
	setApplicationName("mehstation-config");
	connect(this, SIGNAL(aboutToQuit()), SLOT(onQuit()));
}

App::~App() {
}

void App::loadWindow() {
	QUiLoader loader;

	QFile file("res/window.ui");
	file.open(QFile::ReadOnly);

	this->mainWidget = loader.load(&file, NULL);

	file.close();
}

void App::showWindow() {
	Q_ASSERT(this->mainWidget != NULL);

	this->mainWidget->show();
}

void App::onQuit() {
	std::cout << "Exiting mehstation-config." << std::endl;
}
