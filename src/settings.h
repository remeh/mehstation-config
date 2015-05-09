#include <QList>
#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>

#include "app.h"

class Settings : public QWidget {
	public:
		Settings(App* app, QWidget* parent);
		~Settings();

	private:
		App* app;
		QWidget* mainWidget;
};
