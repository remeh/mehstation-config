#include <QtUiTools>
#include <QFileDialog>
#include <QWidget>
#include <QAction>
#include <iostream>
#include "app.h"

App::App(int & argc, char** argv) : QApplication(argc,argv) {
	setApplicationName("mehstation-config");
	connect(this, SIGNAL(aboutToQuit()), SLOT(onQuit()));
}

App::~App() {
}

bool App::loadWindow() {
	QUiLoader loader;

	QFile file("res/window.ui");
	file.open(QFile::ReadOnly);

	this->mainWidget = loader.load(&file, NULL);

	if (this->mainWidget == NULL) {
		std::cerr << "[err] Can't find the UI file." << std::endl;
		return false;
	}

	// Quit action
	QAction* actionQuit = this->mainWidget->findChild<QAction*>("actionQuit");
	connect(actionQuit, SIGNAL(triggered()), this, SLOT(onClickQuit()));
	// Open action
	QAction* actionOpen = this->mainWidget->findChild<QAction*>("actionOpen");
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(onClickOpen()));
	connect(&fileDialog, SIGNAL(fileSelected(const QString&)), this, SLOT(onFileSelected(const QString&)));

	// NOTE We forces the use of the non-native dialog because the native
	// NOTE sometimes calls the slot onFileSelected is called two times. - remy
	fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);

	file.close();
	return true;
}

void App::showWindow() {
	Q_ASSERT(this->mainWidget != NULL);
	this->mainWidget->show();
}

void App::onQuit() {
	std::cout << "Exiting mehstation-config." << std::endl;
}

void App::onClickQuit() {
	this->exit();
}

void App::onClickOpen() {
	this->fileDialog.show();
}

void App::onFileSelected(const QString& filename) {
	qDebug()  << filename;

	// TODO Open the given database.
}
