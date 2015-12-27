#pragma once
	
#include <QApplication>
#include <QFileDialog>
#include <QListWidget>
#include <QString>
#include <QVariant>

#include "ui_app.h"
#include "db.h"
#include "scraping.h"
#include "import.h"

#define MEH_ROLE_PLATFORM_ITEM 1001

class App : public QApplication {
		Q_OBJECT
	public:
		App(int& argc, char** argv);
		~App();

		bool loadWindow();
		void showWindow();
		void updatePlatformList();

		Database* getDb() { return &(this->db); }

		QListWidgetItem* getCurrentItem() {
			return this->ui.listPlatforms->currentItem();
		}

		Platform& getSelectedPlatform() {
			return this->selectedPlatform;
		}
		
		void updateInternalPlatform(Platform platform) {
			if (platform.id == -1) {
				return;
			}
			if (this->platforms == NULL) {
				return;
			}
			
			for (int i = 0; i < this->platforms->count(); i++) {
				if (this->platforms->at(i).id == platform.id) {
					this->platforms->replace(i, platform);
				}
			}
		}

		QString mehtadataPath();

	private:
		Ui::App ui;

		// Main window read from window.ui
		QWidget mainWidget;

		// Executables widget containing all the executables for a platform
		QWidget* executablesWidget;
		// Platform settings
		QWidget* settingsWidget;

		// The file dialog to open a database.
		QFileDialog fileDialog;

		// The scraping window.
		Scraping *scraping;

		// The import window.
		Import* import;

		// Database connection
		Database db;

		// Loaded platforms.
		QList<Platform>* platforms;

		// Loaded executables.
		QList<Executable>* executables;

		// Returns the platform list widget.
		inline QListWidget* getPlatformListWidget();

		// Currently selected platform.
		Platform selectedPlatform;

		QString lookForConfPath();

	public slots:
		void onClickQuit();
		void onClickOpen();
		void onOpenScraping();
		void onImport();
		void onPlatformSelected(QListWidgetItem* item, QListWidgetItem*);
		void onFileSelected(const QString& filename);
		void onAbout();
		void onNewPlatform();
		void onDeletePlatform();
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
