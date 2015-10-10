#pragma once

#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QWidget>
#include <QProcess>

#include "ui_import.h"

class Import : public QWidget {
		Q_OBJECT
	public:
		Import(class App* app);
		~Import();

		void getPlatforms();
		void setPlatformId(int p) {
			this->platform = p;
		}

		void reset();

	private:
		App* app;
		Ui::importES ui;
		int platform;

	public slots:
		void onFilepathTool();
		void onSelectedFilepath(const QString&);
		void onFilepathChange();
};

