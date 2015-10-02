#include <QDebug>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QProcess>
#include <QWidget>

#include "app.h"
#include "scraping.h"
#include "ui_scraping.h"

Scraping::Scraping(App* app) :
	app(app) {
	this->ui.setupUi(this);
	connect(&platformsProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(onPlatformsOutput()));
	connect(this->ui.listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onPlatformSelection()));
	connect(this->ui.toolExecutablesDir, SIGNAL(clicked()), this, SLOT(onDirectoryTool()));
	connect(this->ui.next, SIGNAL(clicked()), this, SLOT(onNext()));
	connect(this->ui.previous, SIGNAL(clicked()), this, SLOT(onPrevious()));

	this->ui.secondStep->hide();
}

Scraping::~Scraping() {

}

void Scraping::getPlatforms() {
	if (platformsProcess.state() == QProcess::Running) {
		return;
	}

	this->ui.firstStep->show();
	this->ui.secondStep->hide();

	this->ui.listWidget->clear();
	
	platformsProcess.setProgram("./mehtadata");
	platformsProcess.setArguments(QStringList("-platforms"));
	platformsProcess.start();
}

void Scraping::onPlatformsOutput() {
	if (platformsProcess.state() == QProcess::NotRunning) {
		return;
	}

	while (platformsProcess.canReadLine()) {
		QString platform = platformsProcess.readLine();
		this->ui.listWidget->addItem(platform);
	}
}

void Scraping::onPlatformSelection() {
	QList<QListWidgetItem *> items = this->ui.listWidget->selectedItems();
	if (items.length() == 0) {
		this->ui.next->setEnabled(false);
	} else {
		this->ui.next->setEnabled(true);
	}
}

void Scraping::onNext() {
	this->ui.firstStep->hide();
	this->ui.secondStep->show();
}

void Scraping::onPrevious() {
	this->ui.firstStep->show();
	this->ui.secondStep->hide();
}

void Scraping::onDirectoryTool() {
	QFileDialog fileDialog(this);
	fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
	fileDialog.setFileMode(QFileDialog::Directory);
	connect(&fileDialog, SIGNAL(fileSelected(const QString&)), this, SLOT(onDirectorySelected(const QString&)));
	fileDialog.exec();
}

void Scraping::onDirectorySelected(const QString& directory) {
	this->ui.executablesDir->setText(directory);
}
