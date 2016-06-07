#include <QWidget>
#include <QWizard>

#include "ui_wizard_platform.h"
#include "ui_wizard_platform_1.h"
#include "ui_wizard_platform_2.h"
#include "app.h"

class PlatformWizard : public QWizard {
		Q_OBJECT
	public:
		PlatformWizard(App* app, QWidget* parent);
		~PlatformWizard();

	private:
		App* app;
		Ui::PlatformWizard ui;

	public slots:
};


class PlatformWizard1 : public QWizardPage {
		Q_OBJECT
	public:
		PlatformWizard1() {
			this->ui.setupUi(this);
		};

		~PlatformWizard1() {};

	private:
		Ui::PlatformWizard1 ui;

	public slots:
};

class PlatformWizard2 : public QWizardPage {
		Q_OBJECT
	public:
		PlatformWizard2() {
			this->ui.setupUi(this);
		};

		QString platformName;

		~PlatformWizard2() {};

	private:
		Ui::PlatformWizard2 ui;

	public slots:
};
