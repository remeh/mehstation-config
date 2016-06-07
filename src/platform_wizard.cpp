#include <QWidget>

#include "platform_wizard.h"
#include "app.h"

PlatformWizard::PlatformWizard(App* app, QWidget* parent = NULL) :
	QWizard(parent),
	app(app) {
	this->ui.setupUi(this);
	this->app = app;

}

PlatformWizard::~PlatformWizard() {
}
