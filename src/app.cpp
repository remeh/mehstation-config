#include <QAction>
#include <QtUiTools>
#include <QFileDialog>
#include <QLayout>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QInputDialog>
#include <QWidget>
#include <iostream>

#include "app.h"
#include "db.h"
#include "executables.h"
#include "settings.h"
#include "models/platform.h"
#include "ui_app.h"

App::App(int & argc, char** argv) :
	QApplication(argc,argv),
	mainWidget(nullptr),
	executablesWidget(nullptr),
	settingsWidget(nullptr),
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
	this->ui.setupUi(&this->mainWidget);

	/*
	 * Menu
	 */

	// Quit action
	QAction* actionQuit = this->ui.actionQuit;
	connect(actionQuit, SIGNAL(triggered()), this, SLOT(onClickQuit()));
	// Open action
	QAction* actionOpen = this->ui.actionOpen;
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(onClickOpen()));
	connect(&fileDialog, SIGNAL(fileSelected(const QString&)), this, SLOT(onFileSelected(const QString&)));

	QAction* actionAbout = this->ui.actionAbout;
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));

	/*
	 * Platforms list
	 */
	// Select an entry
	QListWidget* listPlatforms = this->ui.listPlatforms;
	connect(listPlatforms, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onPlatformSelected(QListWidgetItem*)));

	QPushButton *deletePlatform = this->ui.deletePlatform;
	connect(deletePlatform, SIGNAL(clicked()), this, SLOT(onDeletePlatform()));

	QPushButton *addPlatform = this->ui.addPlatform;
	connect(addPlatform, SIGNAL(clicked()), this, SLOT(onNewPlatform()));
	addPlatform->setEnabled(true);

	// NOTE We forces the use of the non-native dialog because with the native
	// NOTE the slot onFileSelected is called two times. - remy
	fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);

	return true;
}

void App::showWindow() {
	this->mainWidget.show();
	this->mainWidget.raise();
}

void App::onQuit() {
	std::cout << "Exiting mehstation-config." << std::endl;
}

void App::onClickQuit() {
	this->exit();
}

void App::onAbout() {
	QMessageBox::about(NULL, "mehstation config", "<b>mehstation configuration</b><br>Interface to setup your mehstation<br>Bad code written by Rémy 'remeh' Mathieu");
}

void App::onClickOpen() {
	this->fileDialog.show();
}

void App::onNewPlatform() {
	QString name = QInputDialog::getText(NULL, "Platform name ?", "Enter a name for the new platform");
	if (name.length() == 0) {
		return;
	}
	Platform p = this->getDb()->createNewPlatform(name);
	this->platforms->append(p);
	PlatformItem* item = new PlatformItem(p.name, p.id);
	QListWidget* listPlatforms = this->getPlatformListWidget();
	listPlatforms->addItem(item);
	listPlatforms->setCurrentItem(item);
	this->onPlatformSelected(item);
}

void App::onDeletePlatform() {
	QMessageBox::StandardButton result = QMessageBox::question(NULL, "Deletion confirmation", "Are you sure to delete this platform along with all its executables and executable resources ? This can't be undone!");
	if (result == QMessageBox::StandardButton::Yes) {
		this->getDb()->deletePlatform(this->selectedPlatform);
	}
	QListWidget* listPlatforms = this->getPlatformListWidget();
	delete listPlatforms->currentItem();
	if (listPlatforms->currentItem() != NULL) {
		this->onPlatformSelected(listPlatforms->currentItem());
	}
}

void App::updatePlatformList() {
	QListWidget* listPlatforms = this->getPlatformListWidget();
	QListWidgetItem* item = listPlatforms->currentItem();
	if (item != NULL) {
		item->setText(this->selectedPlatform.name);
	}
}

inline QListWidget* App::getPlatformListWidget() {
	return this->ui.listPlatforms;
}

void App::onPlatformSelected(QListWidgetItem* item) {
	if (item == NULL || platforms == NULL) {
		return;
	}

	// look for the platform in the list of loaded platforms.
	Platform p;
	Platform clickedPlatform;
	foreach (p, *platforms) {
		if (p.id == item->data(MEH_ROLE_PLATFORM_ITEM).toInt()) {
			clickedPlatform = p;
			break;
		}
	}

	// same platform, stop here.
	if (clickedPlatform.id == this->selectedPlatform.id)   {
		return;
	}

	this->selectedPlatform = clickedPlatform;

	// create the executable widget.
	if (this->executablesWidget != nullptr) {
		delete this->executablesWidget;
	}
	if (this->settingsWidget != nullptr) {
		delete this->settingsWidget;
	}

	QWidget* executablesTab = this->ui.executables;
	Executables* executables = new Executables(this, NULL);
	executablesTab->layout()->addWidget(executables);
	// load all the executables of this platform and assign it to the widget.
	executables->setExecutables(this->db.getExecutables(this->selectedPlatform.id));
	this->executablesWidget = executables;

	QWidget* settingsTab = this->ui.settings;
	Settings* settings = new Settings(this, NULL);
	settingsTab->layout()->addWidget(settings);
	this->settingsWidget = settings;

	// enable the tab
	QWidget* tabWidget = this->ui.tabWidget;
	tabWidget->setEnabled(true);

	QPushButton *deletePlatform = this->ui.deletePlatform;
	deletePlatform->setEnabled(true);
}

// onFileSelected called when a database file has been selected.
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
			listPlatforms->addItem(item);
		}
	}
}
