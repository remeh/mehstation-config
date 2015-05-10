#include <QDebug>
#include <QFile>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QStackedLayout>
#include <QUiLoader>
#include <QWidget>

#include "app.h"
#include "settings.h"

Settings::Settings(App* app, QWidget* parent = NULL) :
	QWidget(parent),
	app(app),
	mainWidget(nullptr) {
	QUiLoader loader;

	QFile uiFile("res/settings.ui");
	uiFile.open(QFile::ReadOnly);

	this->mainWidget = loader.load(&uiFile, NULL);
	this->setLayout(new QStackedLayout());
	this->layout()->addWidget(this->mainWidget);

	this->initValues();

	QLineEdit* platform = this->mainWidget->findChild<QLineEdit*>("platform");
	QLineEdit* command = this->mainWidget->findChild<QLineEdit*>("command");
	QLineEdit* icon = this->mainWidget->findChild<QLineEdit*>("icon");
	QLineEdit* background = this->mainWidget->findChild<QLineEdit*>("background");
	connect(platform, SIGNAL(textEdited(const QString&)), this, SLOT(onChange()));
	connect(command, SIGNAL(textEdited(const QString&)), this, SLOT(onChange()));
	connect(icon, SIGNAL(textEdited(const QString&)), this, SLOT(onChange()));
	connect(background, SIGNAL(textEdited(const QString&)), this, SLOT(onChange()));

	QPushButton* save = this->mainWidget->findChild<QPushButton*>("save");
	connect(save, SIGNAL(clicked()), this, SLOT(onSave()));
	save->setEnabled(false);

	QToolButton* iconButton = this->mainWidget->findChild<QToolButton*>("iconButton");
	connect(iconButton, SIGNAL(clicked()), this, SLOT(onIconButton()));

	QToolButton* backgroundButton = this->mainWidget->findChild<QToolButton*>("backgroundButton");
	connect(backgroundButton, SIGNAL(clicked()), this, SLOT(onBackgroundButton()));
}

Settings::~Settings() {
}

void Settings::initValues() {
	if (this->app == NULL) {
		return;
	}

	QLineEdit* platform = this->mainWidget->findChild<QLineEdit*>("platform");
	platform->setText(this->app->getSelectedPlatform().name);
	QLineEdit* command = this->mainWidget->findChild<QLineEdit*>("command");
	command->setText(this->app->getSelectedPlatform().command);
	QLineEdit* icon = this->mainWidget->findChild<QLineEdit*>("icon");
	icon->setText(this->app->getSelectedPlatform().icon);
	QLineEdit* background = this->mainWidget->findChild<QLineEdit*>("background");
	background->setText(this->app->getSelectedPlatform().background);

	QLabel* iconImage = this->mainWidget->findChild<QLabel*>("iconImage");
	QLabel* backgroundImage = this->mainWidget->findChild<QLabel*>("backgroundImage");

	QImage image(icon->text());
	QPixmap pixmap;
	pixmap.convertFromImage(image);
	if (!image.isNull()) {
		iconImage->setPixmap(pixmap);
		iconImage->setScaledContents(true);
	}

	image = QImage(background->text());
	pixmap.convertFromImage(image);
	if (!image.isNull()) {
		backgroundImage->setPixmap(pixmap);
		backgroundImage->setScaledContents(true);
	}
}

void Settings::onIconButton() {
	QFileDialog fileDialog(this);
	fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
	connect(&fileDialog, SIGNAL(fileSelected(const QString&)), this, SLOT(onIconSelected(const QString&)));
	fileDialog.exec();	
}

void Settings::onIconSelected(const QString& filepath) {
	QLineEdit* icon = this->mainWidget->findChild<QLineEdit*>("icon");
	icon->setText(filepath);
	this->enableSave();

	// refresh the image
	QLabel* iconImage = this->mainWidget->findChild<QLabel*>("iconImage");
	QImage image(icon->text());
	QPixmap pixmap;
	pixmap.convertFromImage(image);
	if (!image.isNull()) {
		iconImage->setPixmap(pixmap);
	}
}

void Settings::onBackgroundButton() {
	QFileDialog fileDialog(this);
	fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
	connect(&fileDialog, SIGNAL(fileSelected(const QString&)), this, SLOT(onBackgroundSelected(const QString&)));
	fileDialog.exec();	
}

void Settings::onBackgroundSelected(const QString& filepath) {
	QLineEdit* background = this->mainWidget->findChild<QLineEdit*>("background");
	background->setText(filepath);
	this->enableSave();

	// refresh the image
	QLabel* backgroundImage = this->mainWidget->findChild<QLabel*>("backgroundImage");
	QImage image(background->text());
	QPixmap pixmap;
	pixmap.convertFromImage(image);
	if (!image.isNull()) {
		backgroundImage->setPixmap(pixmap);
	}
}

void Settings::onChange() {
	this->enableSave();
}

void Settings::enableSave() {
	QPushButton* save = this->mainWidget->findChild<QPushButton*>("save");
	save->setEnabled(true);
}

void Settings::onSave() {
	QLineEdit* platform = this->mainWidget->findChild<QLineEdit*>("platform");
	QLineEdit* command = this->mainWidget->findChild<QLineEdit*>("command");
	QLineEdit* icon = this->mainWidget->findChild<QLineEdit*>("icon");
	QLineEdit* background = this->mainWidget->findChild<QLineEdit*>("background");
	Platform& selectedPlatform= this->app->getSelectedPlatform();
	selectedPlatform.name = platform->text();
	selectedPlatform.command = command->text();
	selectedPlatform.icon = icon->text();
	selectedPlatform.background = background->text();

	// replace the platform in the list of platform of the app
	this->app->updateInternalPlatform(selectedPlatform);

	// update the db
	this->app->getDb()->update(selectedPlatform);

	this->app->updatePlatformList();

	QPushButton* save = this->mainWidget->findChild<QPushButton*>("save");
	save->setEnabled(false);
}
