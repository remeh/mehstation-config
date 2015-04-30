#pragma once

#include <QApplication>
#include <QFileDialog>
#include <QString>

#include "db.h"

class App : public QApplication {
		Q_OBJECT
	public:
		App(int& argc, char** argv);
		~App();

		bool loadWindow();
		void showWindow();

	private:
		// Main window
		QWidget* mainWidget;

		// The file dialog to open a database.
		QFileDialog fileDialog;

		// Database connection
		Database db;

	public slots:
		void onClickQuit();
		void onClickOpen();
		void onFileSelected(const QString& filename);
		void onQuit();
};
