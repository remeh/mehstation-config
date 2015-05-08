#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QToolButton>
#include <QRadioButton>
#include <QStackedLayout>
#include <QTextEdit>
#include <QUiLoader>
#include <QWidget>
#include "app.h"
#include "executables.h"
#include "models/executable.h"
#include "models/executable_resource.h"

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

	QListWidget* listExecutables = this->getListExecutables();
	connect(listExecutables, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onExecutableSelected(QListWidgetItem*)));
	QListWidget* listResources = this->getListResources();
	connect(listResources, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onResourceSelected(QListWidgetItem*)));

	QToolButton* resourceFilepathOpen = this->mainWidget->findChild<QToolButton*>("resourceFilepathOpen");
	connect(resourceFilepathOpen, SIGNAL(clicked()), this, SLOT(onResourceFilepathClicked()));

	QRadioButton* cover = this->mainWidget->findChild<QRadioButton*>("cover");
	connect(cover, SIGNAL(clicked()), this, SLOT(onTypeChanged()));
	QRadioButton* screenshot = this->mainWidget->findChild<QRadioButton*>("screenshot");
	connect(screenshot, SIGNAL(clicked()), this, SLOT(onTypeChanged()));
	QRadioButton* fanart = this->mainWidget->findChild<QRadioButton*>("fanart");
	connect(fanart, SIGNAL(clicked()), this, SLOT(onTypeChanged()));

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

	QPushButton* newResource = this->mainWidget->findChild<QPushButton*>("newResource");
	connect(newResource, SIGNAL(clicked(bool)), this, SLOT(onNewResource()));

	QPushButton* deleteResource = this->mainWidget->findChild<QPushButton*>("deleteResource");
	connect(deleteResource, SIGNAL(clicked(bool)), this, SLOT(onDeleteResource()));

	QPushButton* deleteExecutable = this->mainWidget->findChild<QPushButton*>("deleteExecutable");
	connect(deleteExecutable, SIGNAL(clicked(bool)), this, SLOT(onDeleteExecutable()));

	QPushButton* save = this->mainWidget->findChild<QPushButton*>("save");
	connect(save, SIGNAL(clicked(bool)), this, SLOT(onSave()));

	QPushButton* saveResource = this->mainWidget->findChild<QPushButton*>("saveResource");
	connect(saveResource, SIGNAL(clicked(bool)), this, SLOT(onSaveResource()));
}

Executables::~Executables() {
	if (this->executables != nullptr) {
		delete this->executables;
	}
}

void Executables::onNewExecutable() {
}

void Executables::onDeleteExecutable() {
	if (this->selectedExecutable.id == -1) {
		return;
	}

	this->app->getDb()->deleteExecutable(this->selectedExecutable);

	// delete the executable from the current list of executable
	for (int i = 0; i < this->executables->count(); i++) {
		if (this->executables->at(i).id == this->selectedExecutable.id) {
			this->executables->removeAt(i);
			break;
		}
	}

	// delete the entry from the list
	QListWidget* listExecutables = this->getListExecutables();
	delete listExecutables->currentItem();

	// load the currently selected item (which has changed due to the deletion)
	if (listExecutables->currentItem() != NULL) {
		this->onExecutableSelected(listExecutables->currentItem());
	}
}

void Executables::onNewResource() {
	// only create a resource if we're really in an executable
	if (this->selectedExecutable.id == -1) {
		return;
	}
	ExecutableResource res = this->app->getDb()->createNewResource(this->selectedExecutable.id);
	this->selectedExecutable.resources.append(res);

	// add the item in the list of resources
	QString text;
	text.append(QString::number(res.id)).append(" - ");
	ItemWithId* item = new ItemWithId(text, res.id);
	QListWidget* listResources = this->getListResources();
	listResources->addItem(item);
	listResources->setCurrentItem(item); // select the item

	this->clearResource();

	// activate the filepath selection
	QToolButton* resourceOpen = this->mainWidget->findChild<QToolButton*>("resourceFilepathOpen");
	resourceOpen->setEnabled(true);
}

void Executables::onDeleteResource() {
	QListWidget* listResources = this->getListResources();
	this->app->getDb()->deleteResource(this->selectedResource.id);
	this->removeResourceFromExecutable(this->selectedResource);
	delete listResources->currentItem();
	this->clearResource();

	if (listResources->currentItem() != NULL) {
		onResourceSelected(listResources->currentItem());
	}
}

void Executables::onTypeChanged() {
	QPushButton* saveResource = this->mainWidget->findChild<QPushButton*>("saveResource");
	saveResource->setEnabled(true);
}

void Executables::onResourceFilepathClicked() {
	QFileDialog fileDialog(this);
	fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
	connect(&fileDialog, SIGNAL(fileSelected(const QString&)), this, SLOT(onResourceFilepathSelected(const QString&)));
	fileDialog.exec();	
}

void Executables::clearResource() {
	// empty the filepath and unselect the type
	QRadioButton* cover = this->mainWidget->findChild<QRadioButton*>("cover");
	QRadioButton* screenshot = this->mainWidget->findChild<QRadioButton*>("screenshot");
	QRadioButton* fanart = this->mainWidget->findChild<QRadioButton*>("fanart");

	cover->setAutoExclusive(false);
	screenshot->setAutoExclusive(false);
	fanart->setAutoExclusive(false);
	cover->setChecked(false);
	screenshot->setChecked(false);
	fanart->setChecked(false);
	cover->setAutoExclusive(true);
	screenshot->setAutoExclusive(true);
	fanart->setAutoExclusive(true);

	QLineEdit* resourceFilepath = this->mainWidget->findChild<QLineEdit*>("resourceFilepath");
	resourceFilepath->setText("");

	QLabel* labelImage = this->mainWidget->findChild<QLabel*>("image");
	labelImage->clear();
}

void Executables::onResourceFilepathSelected(const QString& filepath) {
	this->selectedResource.filepath = filepath;
	QPushButton* saveResource = this->mainWidget->findChild<QPushButton*>("saveResource");
	saveResource->setEnabled(true);
	this->reloadResourceInfo();
}

void Executables::setExecutables(QList<Executable>* executables) {
	if (executables == nullptr) {
		return;
	}

	this->executables = executables;

	// fill the list
	QListWidget* listExecutables = this->getListExecutables();
	if (listExecutables == NULL) {
		qCritical() << "Can't retrieve the listExecutables.";
		return;
	}

	// start by clearing the list
	listExecutables->clear();
	
	// add all executables in the view.
	Executable e;
	foreach (e, *executables) {
		ItemWithId* item = new ItemWithId(e.displayName, e.id);
		listExecutables->addItem(item);
	}
}

void Executables::onResourceSelected(QListWidgetItem* item) {
	int id = item->data(MEH_ROLE_EXEC_ID).toInt();
	ExecutableResource res = this->findResource(id);
	this->selectedResource = res;
	this->reloadResourceInfo();
	QPushButton* deleteResource = this->mainWidget->findChild<QPushButton*>("deleteResource");
	deleteResource->setEnabled(true);
}

void Executables::reloadResourceInfo() {
	ExecutableResource res = this->selectedResource;
	// set the fields.
	QLineEdit* filepath = this->mainWidget->findChild<QLineEdit*>("resourceFilepath");
	filepath->setText(res.filepath);
	
	QToolButton* resourceOpen = this->mainWidget->findChild<QToolButton*>("resourceFilepathOpen");
	resourceOpen->setEnabled(true);

	QRadioButton* cover = this->mainWidget->findChild<QRadioButton*>("cover");
	QRadioButton* screenshot = this->mainWidget->findChild<QRadioButton*>("screenshot");
	QRadioButton* fanart = this->mainWidget->findChild<QRadioButton*>("fanart");
	cover->setChecked(false);
	screenshot->setChecked(false);
	fanart->setChecked(false);
	if (res.type == "cover") {
		cover->setChecked(true);
	}
	if (res.type == "screenshot") {
		screenshot->setChecked(true);
	}
	if (res.type == "fanart") {
		fanart->setChecked(true);
	}

	this->displayImage(res.filepath);

}

void Executables::displayImage(QString filepath) {
	// display the image
	QLabel* labelImage = this->mainWidget->findChild<QLabel*>("image");
	QImage image(filepath);
	QPixmap pixmap;
	pixmap.convertFromImage(image);
	if (!image.isNull()) {
		labelImage->setPixmap(pixmap);
		labelImage->setScaledContents(true);
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
	QPushButton* deleteExecutable = this->mainWidget->findChild<QPushButton*>("deleteExecutable");

	// set the resources
	QListWidget* listResources = this->mainWidget->findChild<QListWidget*>("listResources");
	listResources->clear();
	ExecutableResource er;
	foreach (er, e.resources) {
		QString text;
		text.append(QString::number(er.id)).append(" - ").append(er.type);
		ItemWithId* item = new ItemWithId(text, er.id);
		listResources->addItem(item);
	}

	this->selectedExecutable = e;

	// ui states
	
	this->modifying = false;
	this->saveChangeState();
	deleteExecutable->setEnabled(true);
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

void Executables::onSaveResource() {
	// update the executable values
	QLineEdit* filepath = this->mainWidget->findChild<QLineEdit*>("resourceFilepath");

	this->selectedResource.filepath = filepath->text();

	QRadioButton* cover = this->mainWidget->findChild<QRadioButton*>("cover");
	QRadioButton* screenshot = this->mainWidget->findChild<QRadioButton*>("screenshot");
	QRadioButton* fanart = this->mainWidget->findChild<QRadioButton*>("fanart");
	if (cover->isChecked()) {
		this->selectedResource.type = "cover";
	}
	if (screenshot->isChecked()) {
		this->selectedResource.type = "screenshot";
	}
	if (fanart->isChecked()) {
		this->selectedResource.type = "fanart";
	}

	Database* db = this->app->getDb();
	db->update(this->selectedResource);

	this->updateInternalResource(this->selectedResource);
	
	QPushButton* saveResource = this->mainWidget->findChild<QPushButton*>("saveResource");
	saveResource->setEnabled(false);
}

void Executables::onSave() {
	// update the executable values
	QLineEdit* name = this->mainWidget->findChild<QLineEdit*>("editName");
	QLineEdit* filepath = this->mainWidget->findChild<QLineEdit*>("editFilepath");
	QLineEdit* genres = this->mainWidget->findChild<QLineEdit*>("editGenres");
	QLineEdit* publisher = this->mainWidget->findChild<QLineEdit*>("editPublisher");
	QLineEdit* developer = this->mainWidget->findChild<QLineEdit*>("editDeveloper");
	QLineEdit* releaseDate = this->mainWidget->findChild<QLineEdit*>("editReleaseDate");
	QLineEdit* players = this->mainWidget->findChild<QLineEdit*>("editPlayers");
	QLineEdit* rating = this->mainWidget->findChild<QLineEdit*>("editRating");
	QTextEdit* description = this->mainWidget->findChild<QTextEdit*>("textDescription");

	this->selectedExecutable.displayName = name->text();
	this->selectedExecutable.filepath = filepath->text();
	this->selectedExecutable.genres = genres->text();
	this->selectedExecutable.publisher = publisher->text();
	this->selectedExecutable.developer = developer->text();
	this->selectedExecutable.releaseDate = releaseDate->text();
	this->selectedExecutable.players = players->text();
	this->selectedExecutable.rating = rating->text();
	this->selectedExecutable.description = description->toPlainText();

	Database* db = this->app->getDb();
	db->update(this->selectedExecutable);

	this->updateInternalExecutables(this->selectedExecutable);
	
	this->modifying = false;
	this->saveChangeState();
}

void Executables::updateInternalExecutables(Executable executable) {
	for (int i = 0; i < this->executables->count(); i++) {
		if (this->executables->at(i).id == executable.id) {
			this->executables->replace(i, executable);
			break;
		}
	}
	QListWidget* listExecutables = this->getListExecutables();
	QListWidgetItem* item = listExecutables->currentItem();
	if (item != NULL) {
		item->setText(executable.displayName);
	}
}

void Executables::updateInternalResource(ExecutableResource resource) {
	QList<ExecutableResource> resources = this->selectedExecutable.resources;
	for (int i = 0; i < resources.count(); i++) {
		if (resources.at(i).id == resource.id) {
			this->selectedExecutable.resources.replace(i, resource);
			break;
		}
	}

	QListWidget* listResources = this->getListResources();
	QListWidgetItem* item = listResources->currentItem();
	QString text;
	text.append(QString::number(resource.id)).append(" - ").append(resource.type);
	item->setText(text);
}

void Executables::removeResourceFromExecutable(ExecutableResource resource) {
	QList<ExecutableResource> resources = this->selectedExecutable.resources;
	for (int i = 0; i < resources.count(); i++) {
		if (resources.at(i).id == resource.id) {
			QList<ExecutableResource> resources = this->selectedExecutable.resources;
			resources.removeAt(i);
			this->selectedExecutable.resources = resources;

			for (int j = 0; j < executables->count(); j++) {
				if (executables->at(j).id == this->selectedExecutable.id) {
					executables->replace(j, this->selectedExecutable);
				}
			}

			break;
		}
	}
}

ExecutableResource Executables::findResource(int id) {
	if (this->selectedExecutable.id == -1) {
		return ExecutableResource();
	}

	ExecutableResource res;
	foreach (res, this->selectedExecutable.resources) {
		if (res.id == id) {
			return res;
		}
	}

	return ExecutableResource();
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
