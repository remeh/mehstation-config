#include <QDebug>
#include <QFile>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QStackedLayout>
#include <QUiLoader>
#include <QWidget>
#include "app.h"
#include "executables.h"
#include "models/executable.h"

Executables::Executables(App* app, QWidget* parent = NULL) :
	QWidget(parent),
	app(app),
	mainWidget(nullptr),
	executables(nullptr) {
	QUiLoader loader;

	QFile uiFile("res/executable.ui");
	uiFile.open(QFile::ReadOnly);

	this->mainWidget = loader.load(&uiFile, NULL);
	this->setLayout(new QStackedLayout());
	this->layout()->addWidget(this->mainWidget);

	QListWidget* listExecutables = this->mainWidget->findChild<QListWidget*>("listExecutables");
	connect(listExecutables, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onExecutableSelected(QListWidgetItem*)));
}

Executables::~Executables() {
	if (this->executables != nullptr) {
		delete this->executables;
	}
}

void Executables::setExecutables(QList<Executable>* executables) {
	if (executables == nullptr) {
		return;
	}

	this->executables = executables;

	// fill the list
	QListWidget* listExecutables = this->mainWidget->findChild<QListWidget*>("listExecutables");
	if (listExecutables == NULL) {
		qCritical() << "Can't retrieve the listExecutables.";
		return;
	}

	// start by clearing the list
	listExecutables->clear();

	// TODO clear the entries
	
	// add all executables in the view.
	Executable e;
	foreach (e, *executables) {
		ExecutableItem* item = new ExecutableItem(e.displayName, e.id);
		listExecutables->addItem(item);
	}
}

void Executables::onExecutableSelected(QListWidgetItem* item) {
	int id = item->data(MEH_ROLE_EXEC_ID).toInt();

	Executable e = this->findExecutable(id);
	
	// set the fields.
	QLineEdit* editName = this->mainWidget->findChild<QLineEdit*>("editName");
	editName->setText(e.displayName);
}

Executable Executables::findExecutable(int id) {
	if (this->executables == nullptr) {
		return Executable();
	}

	Executable e;
	foreach (e, *(this->executables)) {
		if (e.id == id) {
			return e;
		}
	}

	return Executable();
}
