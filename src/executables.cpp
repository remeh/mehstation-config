#include <QDebug>
#include <QFile>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QStackedLayout>
#include <QTextEdit>
#include <QUiLoader>
#include <QWidget>
#include "app.h"
#include "executables.h"
#include "models/executable.h"

Executables::Executables(App* app, QWidget* parent = NULL) :
	QWidget(parent),
	app(app),
	mainWidget(nullptr),
	executables(nullptr),
	modifying(false) {
	QUiLoader loader;

	QFile uiFile("res/executable.ui");
	uiFile.open(QFile::ReadOnly);

	this->mainWidget = loader.load(&uiFile, NULL);
	this->setLayout(new QStackedLayout());
	this->layout()->addWidget(this->mainWidget);

	QListWidget* listExecutables = this->mainWidget->findChild<QListWidget*>("listExecutables");
	connect(listExecutables, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onExecutableSelected(QListWidgetItem*)));

	// connect each edit values to a "modifying" flag
	QLineEdit* name = this->mainWidget->findChild<QLineEdit*>("editName");
	QLineEdit* filepath = this->mainWidget->findChild<QLineEdit*>("editFilepath");
	QLineEdit* genres = this->mainWidget->findChild<QLineEdit*>("editGenres");
	QLineEdit* publisher = this->mainWidget->findChild<QLineEdit*>("editPublisher");
	QLineEdit* developer = this->mainWidget->findChild<QLineEdit*>("editDeveloper");
	QLineEdit* releaseDate = this->mainWidget->findChild<QLineEdit*>("editReleaseDate");
	QLineEdit* players = this->mainWidget->findChild<QLineEdit*>("editPlayers");
	QLineEdit* rating = this->mainWidget->findChild<QLineEdit*>("editRating");
	QTextEdit* description = this->mainWidget->findChild<QTextEdit*>("textDescription");
	connect(name, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdition()));
	connect(filepath, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdition()));
	connect(genres, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdition()));
	connect(publisher, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdition()));
	connect(developer, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdition()));
	connect(releaseDate, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdition()));
	connect(players, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdition()));
	connect(rating, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdition()));
	connect(description, SIGNAL(textChanged()), this, SLOT(onTextEdition()));

	QPushButton* save = this->mainWidget->findChild<QPushButton*>("save");
	connect(save, SIGNAL(clicked(bool)), this, SLOT(onSave()));
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
	QLineEdit* name = this->mainWidget->findChild<QLineEdit*>("editName");
	name->setText(e.displayName);
	QLineEdit* filepath = this->mainWidget->findChild<QLineEdit*>("editFilepath");
	filepath->setText(e.filepath);
	QLineEdit* genres = this->mainWidget->findChild<QLineEdit*>("editGenres");
	genres->setText(e.genres);;
	QLineEdit* publisher = this->mainWidget->findChild<QLineEdit*>("editPublisher");
	publisher->setText(e.publisher);;
	QLineEdit* developer = this->mainWidget->findChild<QLineEdit*>("editDeveloper");
	developer->setText(e.publisher);;
	QLineEdit* releaseDate = this->mainWidget->findChild<QLineEdit*>("editReleaseDate");
	releaseDate->setText(e.releaseDate);;
	QLineEdit* players = this->mainWidget->findChild<QLineEdit*>("editPlayers");
	players->setText(e.players);;
	QLineEdit* rating = this->mainWidget->findChild<QLineEdit*>("editRating");
	rating->setText(e.players);;
	QTextEdit* description = this->mainWidget->findChild<QTextEdit*>("textDescription");
	description->setText(e.description);;

	this->modifying = false;
	this->saveChangeState();
}

void Executables::onTextEdition() {
	this->modifying = true;
	this->saveChangeState();
}

void Executables::saveChangeState() {
	QPushButton* save = this->mainWidget->findChild<QPushButton*>("save");
	save->setEnabled(false);
	if (this->modifying) {
		save->setEnabled(true);
	}
}

void Executables::onSave() {
	// TODO save the values of the executable
	this->modifying = false;
	this->saveChangeState();
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
