#include <QDebug>
#include <QFile>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
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

void Settings::onChange() {
	QPushButton* save = this->mainWidget->findChild<QPushButton*>("save");
	save->setEnabled(true);
}
