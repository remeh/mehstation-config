#include <QDebug>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QProcess>
#include <QWidget>

#include "app.h"
#include "scraping.h"
#include "ui_scraping.h"

Scraping::Scraping(App* app) :
	app(app),
	platform(-1) {
	this->ui.setupUi(this);
	connect(&platformsProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(onPlatformsOutput()));
	connect(&scrapingProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(onScrapingOutput()));
	connect(&scrapingProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onScrapingFinished()));
	connect(this->ui.listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onPlatformSelection()));
	connect(this->ui.toolExecutablesDir, SIGNAL(clicked()), this, SLOT(onDirectoryTool()));
	connect(this->ui.toolOutputDirectory, SIGNAL(clicked()), this, SLOT(onOutputDirTool()));
	connect(this->ui.cancel, SIGNAL(clicked()), this, SLOT(onCancel()));
	connect(this->ui.next, SIGNAL(clicked()), this, SLOT(onNext()));
	connect(this->ui.previous, SIGNAL(clicked()), this, SLOT(onPrevious()));
	connect(this->ui.start, SIGNAL(clicked()), this, SLOT(onStart()));
	connect(this->ui.stopScraping, SIGNAL(clicked()), this, SLOT(onStopScraping()));
	connect(this->ui.zipCheckbox, SIGNAL(stateChanged(int)), this, SLOT(onChangeSettings()));
	connect(this->ui.rarCheckbox, SIGNAL(stateChanged(int)), this, SLOT(onChangeSettings()));
	connect(this->ui.isoCheckbox, SIGNAL(stateChanged(int)), this, SLOT(onChangeSettings()));
	connect(this->ui.executablesDir, SIGNAL(textChanged(const QString&)), this, SLOT(onChangeSettings()));
	connect(this->ui.outputDirectory, SIGNAL(textChanged(const QString&)), this, SLOT(onChangeSettings()));
	connect(this->ui.lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(onChangeSettings()));

	this->ui.secondStep->hide();
	this->ui.thirdStep->hide();
}

Scraping::~Scraping() {

}

void Scraping::getPlatforms() {
	if (platformsProcess.state() == QProcess::Running) {
		return;
	}

	this->ui.firstStep->show();
	this->ui.secondStep->hide();
	this->ui.thirdStep->hide();

	this->ui.listWidget->clear();
	
	platformsProcess.setProgram("./mehtadata");
	platformsProcess.setArguments(QStringList("-platforms"));
	platformsProcess.start();
}

void Scraping::onStopScraping() {
	if (scrapingProcess.state() == QProcess::NotRunning) {
		return;
	}

	// stop the process
	scrapingProcess.kill();
}

void Scraping::onPlatformsOutput() {
	if (platformsProcess.state() == QProcess::NotRunning) {
		return;
	}

	while (platformsProcess.canReadLine()) {
		QString platform = platformsProcess.readLine();
		this->ui.listWidget->addItem(platform.replace("\n",""));
	}
}

void Scraping::onScrapingOutput() {
	while (scrapingProcess.canReadLine()) {
		QString line = scrapingProcess.readLine();
		this->ui.terminalOutput->addItem(line.replace("\n", ""));
	}
}

void Scraping::onScrapingFinished() {
	// reconnect to the database
	this->ui.terminalOutput->addItem("Scraping done, refreshing the database.");
	this->app->getDb()->open(this->app->getDb()->filename);
	this->ui.terminalOutput->addItem("OK. The scraping is over!");
	this->ui.stopScraping->setEnabled(false);
	// reselect the entry in the platforms list
	// NOTE(remy): if the user has selected another platform during the scraping process, this won't work
	this->app->getSelectedPlatform().id = -1;
	this->app->onPlatformSelected(this->app->getCurrentItem(), NULL);
}

void Scraping::onPlatformSelection() {
	QList<QListWidgetItem *> items = this->ui.listWidget->selectedItems();
	if (items.length() == 0) {
		this->ui.next->setEnabled(false);
	} else {
		this->ui.next->setEnabled(true);
	}
}

void Scraping::startScraping() {
	// generate the ext parameter

	QString extensions;
	if (this->ui.isoCheckbox->isChecked()) {
		addExt(extensions, ".iso");
	}
	if (this->ui.zipCheckbox->isChecked()) {
		addExt(extensions, ".zip");
	}
	if (this->ui.rarCheckbox->isChecked()) {
		addExt(extensions, ".rar");
	}
	if (this->ui.lineEdit->text().length() > 0) {
		addExt(extensions, this->ui.lineEdit->text());
	}

	// Checks whether or not it's a valid number
	int maxWidth = this->ui.width->text().toInt();
	if (maxWidth == 0) {
		maxWidth = 768;
	}

	// close the db to not corrupt it.

	this->app->getDb()->close();

	// generate the platforms parameter

	QString platforms;
	QList<QListWidgetItem*> items = this->ui.listWidget->selectedItems();
	for (int i = 0; i < items.size(); i++) {
		QListWidgetItem* item = items.at(i);
		if (i > 0) {
			platforms += ",";
		}
		platforms += item->text().replace("\n","");
	}

	scrapingProcess.setProgram("./mehtadata");
	QStringList arguments;
	arguments	<< "-dest" << "mehstation"
				<<	"-ext" << extensions
				<< "-in-dir" << this->ui.executablesDir->text()
				<< "-out-dir" << this->ui.outputDirectory->text()
				<< "-meh-db" << this->app->getDb()->filename
				<< "-meh-platform" << QString::number(this->platform)
				<< "-w" << QString::number(maxWidth)
				<< "-p" << platforms;

	scrapingProcess.setArguments(arguments);
	scrapingProcess.start();

	this->ui.terminalOutput->addItem("Launching the scraping with:");
	this->ui.terminalOutput->addItem("./mehtadata " + arguments.join(" "));
}

void Scraping::addExt(QString& exts, QString ext) {
	if (exts.length() > 0) {
		exts += ",";
	}
	exts += ext;
}

void Scraping::onCancel() {
	this->platform = -1;
	this->ui.firstStep->show();
	this->ui.secondStep->hide();
	this->ui.thirdStep->hide();
	this->hide();
}

void Scraping::onStart() {
	this->ui.secondStep->hide();
	this->ui.thirdStep->show();

	// TODO(remy): check that the directory are readable

	this->resize(500, this->height());
	this->startScraping();
}

void Scraping::onNext() {
	this->ui.firstStep->hide();
	this->ui.secondStep->show();
	this->ui.thirdStep->hide();
}

void Scraping::onPrevious() {
	this->ui.firstStep->show();
	this->ui.secondStep->hide();
	this->ui.thirdStep->hide();
}

void Scraping::onDirectoryTool() {
	QFileDialog fileDialog(this);
	fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
	fileDialog.setFileMode(QFileDialog::Directory);
	connect(&fileDialog, SIGNAL(fileSelected(const QString&)), this, SLOT(onDirectorySelected(const QString&)));
	fileDialog.exec();
}

void Scraping::onOutputDirTool() {
	QFileDialog fileDialog(this);
	fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
	fileDialog.setFileMode(QFileDialog::Directory);
	connect(&fileDialog, SIGNAL(fileSelected(const QString&)), this, SLOT(onOutputDirSelected(const QString&)));
	fileDialog.exec();
}

void Scraping::onOutputDirSelected(const QString& directory) {
	this->ui.outputDirectory->setText(directory);
}

void Scraping::onDirectorySelected(const QString& directory) {
	this->ui.executablesDir->setText(directory);
}

void Scraping::onChangeSettings() {
	bool ok = false;
	if (this->ui.isoCheckbox->isChecked() || this->ui.rarCheckbox->isChecked() ||
			this->ui.zipCheckbox->isChecked()) {
		ok = true;
	}

	if (this->ui.lineEdit->text().length() > 0) {
		ok = true;
	}

	if (ok && this->ui.executablesDir->text().length() > 0) {
		this->ui.start->setEnabled(true);
	} else {
		this->ui.start->setEnabled(false);
	}
}
