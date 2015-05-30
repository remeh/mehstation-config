#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QWidget>

#include "ui_settings.h"
#include "app.h"

class Settings : public QWidget {
		Q_OBJECT
	public:
		Settings(App* app, QWidget* parent);
		~Settings();

	private:
		App* app;
		QWidget* mainWidget;
		Ui::Settings ui;

		void initValues();
		void enableSave();

	public slots:
		void onChange();
		void onSave();
		void onIconButton();
		void onIconSelected(const QString&);
		void onBackgroundButton();
		void onBackgroundSelected(const QString&);
		void onCommandButton();
		void onCommandSelected(const QString& command);
};
