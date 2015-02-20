#pragma once

#include <QApplication>

class App : public QApplication {
		Q_OBJECT
	public:
		App(int& argc, char** argv);
		~App();

		void loadWindow();
		void showWindow();

	private:
		QWidget* mainWidget;

	public slots:
		void onQuit();
};
