#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QWidget>

#include "app.h"

class Settings : public QWidget {
		Q_OBJECT
	public:
		Settings(App* app, QWidget* parent);
		~Settings();

	private:
		App* app;
		QWidget* mainWidget;

		void initValues();

	public slots:
		void onChange();
};
