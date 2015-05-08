#pragma once
	
#include <QApplication>
#include <QFileDialog>
#include <QListWidget>
#include <QString>
#include <QVariant>

#include "db.h"

#define MEH_ROLE_PLATFORM_ITEM 1001

class App : public QApplication {
		Q_OBJECT
	public:
		App(int& argc, char** argv);
		~App();

		bool loadWindow();
		void showWindow();

		Database* getDb() { return &(this->db); }

	private:
		// Main window read from window.ui
		QWidget* mainWidget;

		// Executables widget containing all the executables for a platform
		QWidget* executablesWidget;

		// The file dialog to open a database.
		QFileDialog fileDialog;

		// Database connection
		Database db;

		// Loaded platforms.
		QList<Platform>* platforms;

		// Loaded executables.
		QList<Executable>* executables;

		// Currently selected platform.
		Platform selectedPlatform;

		// Returns the platform list widget.
		inline QListWidget* getPlatformListWidget();

	public slots:
		void onClickQuit();
		void onClickOpen();
		void onPlatformSelected(QListWidgetItem* item);
		void onFileSelected(const QString& filename);
		void onQuit();
};

// Specific items for the platform list, containing the platform
// ID is in the data QVariant.
class PlatformItem : public QListWidgetItem {
	public:
		PlatformItem(const QString& text, int id)
			: QListWidgetItem(text) {
			this->id = id;			
		}
		void setData(int role, const QVariant& value) {
			if (role == MEH_ROLE_PLATFORM_ITEM) {
				this->id = value.toInt();
				return;
			}
			QListWidgetItem::setData(role, value);
		}
		QVariant data(int role) const {
			if (role == MEH_ROLE_PLATFORM_ITEM) {
				return this->id;
			}
			return QListWidgetItem::data(role);
		}
		int id;
};
