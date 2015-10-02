#include <QWidget>
#include <QDebug>
#include <QListWidgetItem>
#include <QProcess>

#include "app.h"
#include "scraping.h"
#include "ui_scraping.h"

Scraping::Scraping(App* app) :
	app(app) {
	this->ui.setupUi(this);
	connect(&platformsProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(onPlatformsOutput()));
	connect(this->ui.listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onPlatformSelection()));

	this->ui.secondStep->hide();
}

Scraping::~Scraping() {

}

void Scraping::getPlatforms() {
	if (platformsProcess.state() == QProcess::Running) {
		return;
	}

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
