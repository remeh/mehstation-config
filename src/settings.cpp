#include <QDebug>
#include <QFile>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QStackedLayout>
#include <QWidget>

#include "app.h"
#include "settings.h"
#include "ui_settings.h"

Settings::Settings(App* app, QWidget* parent = NULL) :
	QWidget(parent),
	app(app) {
	this->ui.setupUi(this);

	this->initValues();

	QLineEdit* platform = this->ui.platform;
	QLineEdit* command = this->ui.command;
	QLineEdit* icon = this->ui.icon;
	QLineEdit* background = this->ui.background;
	connect(platform, SIGNAL(textEdited(const QString&)), this, SLOT(onChange()));
	connect(command, SIGNAL(textEdited(const QString&)), this, SLOT(onChange()));
	connect(icon, SIGNAL(textEdited(const QString&)), this, SLOT(onChange()));
	connect(background, SIGNAL(textEdited(const QString&)), this, SLOT(onChange()));

	QPushButton* save = this->ui.save;
	connect(save, SIGNAL(clicked()), this, SLOT(onSave()));
	save->setEnabled(false);

	QToolButton* iconButton = this->ui.iconButton;
	connect(iconButton, SIGNAL(clicked()), this, SLOT(onIconButton()));

	connect(this->ui.backgroundButton, SIGNAL(clicked()), this, SLOT(onBackgroundButton()));
	connect(this->ui.commandButton, SIGNAL(clicked(bool)), this, SLOT(onCommandButton()));
	connect(this->ui.completeView, SIGNAL(clicked()), this, SLOT(onViewSelected()));
	connect(this->ui.coverView, SIGNAL(clicked()), this, SLOT(onViewSelected()));
}

Settings::~Settings() {
}

void Settings::initValues() {
	if (this->app == NULL) {
		return;
	}

	this->ui.platform->setText(this->app->getSelectedPlatform().name);
	this->ui.command->setText(this->app->getSelectedPlatform().command);
	this->ui.icon->setText(this->app->getSelectedPlatform().icon);
	this->ui.background->setText(this->app->getSelectedPlatform().background);

	QLabel* iconImage = this->ui.iconImage;
	QLabel* backgroundImage = this->ui.backgroundImage;

	QImage image(this->ui.icon->text());
	QPixmap pixmap;
	pixmap.convertFromImage(image);
	if (!image.isNull()) {
		iconImage->setPixmap(pixmap.scaled(iconImage->width(),iconImage->height(),Qt::KeepAspectRatio));
		iconImage->setScaledContents(true);
	}

	image = QImage(this->ui.background->text());
	pixmap.convertFromImage(image);
	if (!image.isNull()) {
		backgroundImage->setPixmap(pixmap);
		backgroundImage->setScaledContents(true);
	}

	qDebug() << this->app->getSelectedPlatform().view ;
	if (this->app->getSelectedPlatform().view == "complete") {
		this->ui.completeView->setChecked(true);
		this->ui.coverView->setChecked(false);
	} else {
		this->ui.coverView->setChecked(true);
		this->ui.completeView->setChecked(false);
	}
}

void Settings::onViewSelected() {
	this->enableSave();
}

void Settings::onCommandButton() {
	QFileDialog fileDialog(this);
	fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
	connect(&fileDialog, SIGNAL(fileSelected(const QString&)), this, SLOT(onCommandSelected(const QString&)));
	fileDialog.exec();	
}

void Settings::onCommandSelected(const QString& command) {
	this->ui.command->setText(command);
	this->enableSave();
}


void Settings::onIconButton() {
	QFileDialog fileDialog(this);
	fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
	connect(&fileDialog, SIGNAL(fileSelected(const QString&)), this, SLOT(onIconSelected(const QString&)));
	fileDialog.exec();	
}

void Settings::onIconSelected(const QString& filepath) {
	QLineEdit* icon = this->ui.icon;
	icon->setText(filepath);
	this->enableSave();

	// refresh the image
	QLabel* iconImage = this->ui.iconImage;
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
	QLineEdit* background = this->ui.background;
	background->setText(filepath);
	this->enableSave();

	// refresh the image
	QLabel* backgroundImage = this->ui.backgroundImage;
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
	QPushButton* save = this->ui.save;
	save->setEnabled(true);
}

void Settings::onSave() {
	QLineEdit* platform = this->ui.platform;
	QLineEdit* command = this->ui.command;
	QLineEdit* icon = this->ui.icon;
	QLineEdit* background = this->ui.background;
	Platform& selectedPlatform= this->app->getSelectedPlatform();
	selectedPlatform.name = platform->text();
	selectedPlatform.command = command->text();
	selectedPlatform.icon = icon->text();
	selectedPlatform.background = background->text();

	if (this->ui.completeView->isChecked()) {
		selectedPlatform.view = "complete";
	} else {
		selectedPlatform.view = "cover";
	}

	// replace the platform in the list of platform of the app
	this->app->updateInternalPlatform(selectedPlatform);

	// update the db
	this->app->getDb()->update(selectedPlatform);

	this->app->updatePlatformList();

	QPushButton* save = this->ui.save;
	save->setEnabled(false);
}
