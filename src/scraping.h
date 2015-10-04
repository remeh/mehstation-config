#pragma once

#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QWidget>
#include <QProcess>

#include "ui_scraping.h"

class Scraping : public QWidget {
		Q_OBJECT
	public:
		Scraping(class App* app);
		~Scraping();

		void getPlatforms();
		void setPlatformId(int p) {
			this->platform = p;
		}

	private:
		App* app;
		Ui::Scraping ui;

		QProcess platformsProcess;
		QProcess scrapingProcess;
		QWidget *widget;

		int platform;

		void startScraping();
		void addExt(QString&, QString);

	public slots:
		void onDirectoryTool();
		void onOutputDirTool();
		void onCancel();
		void onNext();
		void onPrevious();
		void onStart();
		void onStopScraping();
		void onPlatformsOutput();
		void onScrapingOutput();
		void onScrapingFinished();
		void onPlatformSelection();
		void onDirectorySelected(const QString&);
		void onOutputDirSelected(const QString&);
		void onChangeSettings();
};

