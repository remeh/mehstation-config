#include <QtUiTools>
#include <QFileDialog>
#include <QLayout>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QWidget>
#include <QAction>
#include <iostream>
#include "app.h"
#include "db.h"
#include "models/platform.h"

App::App(int & argc, char** argv) :
	QApplication(argc,argv),
	executablesWidget(nullptr),
	platforms(nullptr) {
	setApplicationName("mehstation-config");
	connect(this, SIGNAL(aboutToQuit()), SLOT(onQuit()));
}

App::~App() {
	if (platforms != nullptr) {
		delete platforms;
	}
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

	/*
	 * Menu
	 */

	// Quit action
	QAction* actionQuit = this->mainWidget->findChild<QAction*>("actionQuit");
	connect(actionQuit, SIGNAL(triggered()), this, SLOT(onClickQuit()));
	// Open action
	QAction* actionOpen = this->mainWidget->findChild<QAction*>("actionOpen");
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(onClickOpen()));
	connect(&fileDialog, SIGNAL(fileSelected(const QString&)), this, SLOT(onFileSelected(const QString&)));

	/*
	 * Platforms list
	 */
	// Select an entry
	QListWidget* listPlatforms = this->mainWidget->findChild<QListWidget*>("listPlatforms");
	connect(listPlatforms, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onPlatformSelected(QListWidgetItem*)));

	// NOTE We forces the use of the non-native dialog because with the native
	// NOTE the slot onFileSelected is called two times. - remy
	fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);

	file.close();
	return true;
}

void App::showWindow() {
	Q_ASSERT(this->mainWidget != NULL);
	this->mainWidget->show();
	this->mainWidget->raise();
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

inline QListWidget* App::getPlatformListWidget() {
	return this->mainWidget->findChild<QListWidget*>("listPlatforms");
}

void App::onPlatformSelected(QListWidgetItem* item) {
	// don't reload when it's the same one.
	if (item == this->selectedPlatform) {
		return;
	}

	// delete the previous data if any
	if (this->selectedPlatform != nullptr && this->executables != nullptr) {
		delete this->executables;
	}

	// store the selected item.
	this->selectedPlatform = item;
	
	// load all the executables of this platform
	this->executables = this->db.getExecutables(item->data(MEH_ROLE_PLATFORM_ITEM).toInt());

	// create the executable widget.
	if (this->executablesWidget != nullptr) {
		delete this->executablesWidget;
	}

	QUiLoader loader;

	QFile uiFile("res/executable.ui");
	uiFile.open(QFile::ReadOnly);

	this->executablesWidget = loader.load(&uiFile, NULL);

	QWidget* executables = this->mainWidget->findChild<QWidget*>("executables");
	executables->layout()->addWidget(executablesWidget);
}

void App::onFileSelected(const QString& filename) {
	qDebug()  << filename;

	QFile file(filename);
	if (!file.exists()) {
		QMessageBox msgBox(QMessageBox::Critical, "Error", "This file doesn't exist. Please select an existing database.");
		msgBox.exec();
		return;
	}

	// open the database
	db.open(filename);

	QList<Platform>* platforms = db.getPlatforms();

	QListWidget* listPlatforms = this->getPlatformListWidget();

	if (platforms != nullptr) {
		// store the list pointer
		this->platforms = platforms;

		// add all platforms in the view.
		Platform p;
		foreach (p, *platforms) {
			PlatformItem* item = new PlatformItem(p.name, p.id);
			listPlatforms->addItem(reinterpret_cast<QListWidgetItem*>(item));
		}
	}
}
