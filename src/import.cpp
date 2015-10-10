#include <QDebug>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QProcess>
#include <QWidget>

#include "app.h"
#include "import.h"
#include "ui_import.h"

Import::Import(App* app) :
	app(app),
	platform(-1) {
	this->ui.setupUi(this);
	
	connect(this->ui.toolFilepath, SIGNAL(clicked()), this, SLOT(onFilepathTool()));
	connect(this->ui.start, SIGNAL(clicked()), this, SLOT(onStart()));
	connect(this->ui.filepath, SIGNAL(textChanged(const QString&)), this, SLOT(onFilepathChange()));
	connect(&importProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onImportFinished()));
}

Import::~Import() {
	this->ui.filepath->setText("");
	this->ui.start->setEnabled(false);
}

void Import::reset() {
}

void Import::onFilepathTool() {
	QFileDialog fileDialog(this);
	fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
	connect(&fileDialog, SIGNAL(fileSelected(const QString&)), this, SLOT(onSelectedFilepath(const QString&)));
	fileDialog.exec();
}

void Import::onSelectedFilepath(const QString& filepath) {
	this->ui.filepath->setText(filepath);
}

void Import::onFilepathChange() {
	if (this->ui.filepath->text().length() > 0) {
		this->ui.start->setEnabled(true);
	} else {
		this->ui.start->setEnabled(false);
	}
}

void Import::onStart() {
	this->ui.start->setEnabled(false);

	// close the db to not corrupt it.

	this->app->getDb()->close();

	// start the import

	importProcess.setProgram("./mehtadata");
	QStringList arguments;
	arguments	<<"-es" << this->ui.filepath->text()
				<< "-meh-db" << this->app->getDb()->filename
				<< "-meh-platform" << QString::number(this->platform);
	importProcess.setArguments(arguments);
	importProcess.start();
	qDebug() << "OK";
	qDebug() << arguments;
}

void Import::onImportFinished() {
	this->ui.start->setEnabled(true);
	this->app->getDb()->open(this->app->getDb()->filename);
	this->app->getSelectedPlatform().id = -1;
	this->app->onPlatformSelected(this->app->getCurrentItem(), NULL);
	this->hide();
}
