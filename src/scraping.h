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

	private:
		App* app;
		Ui::Scraping ui;

		QProcess platformsProcess;
		QWidget *widget;

	public slots:
		void onPlatformsOutput();
		void onPlatformSelection();
};

