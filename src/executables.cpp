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
	this->ui.setupUi(this);

	QListWidget* listExecutables = this->getListExecutables();
	connect(listExecutables, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onExecutableSelected(QListWidgetItem*)));
	QListWidget* listResources = this->getListResources();
	connect(listResources, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onResourceSelected(QListWidgetItem*)));

	QToolButton* resourceFilepathOpen = this->ui.resourceFilepathOpen;
	connect(resourceFilepathOpen, SIGNAL(clicked()), this, SLOT(onResourceFilepathClicked()));

	connect(this->ui.cover, SIGNAL(clicked()), this, SLOT(onTypeChanged()));
	connect(this->ui.screenshot, SIGNAL(clicked()), this, SLOT(onTypeChanged()));
	connect(this->ui.fanart, SIGNAL(clicked()), this, SLOT(onTypeChanged()));
	connect(this->ui.logo, SIGNAL(clicked()), this, SLOT(onTypeChanged()));
	connect(this->ui.video, SIGNAL(clicked()), this, SLOT(onTypeChanged()));

	// connect each edit values to a "modifying" flag
	connect(this->ui.editName, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdition()));
	connect(this->ui.editFilepath, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdition()));
	connect(this->ui.editGenres, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdition()));
	connect(this->ui.editPublisher, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdition()));
	connect(this->ui.editDeveloper, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdition()));
	connect(this->ui.editReleaseDate, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdition()));
	connect(this->ui.editPlayers, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdition()));
	connect(this->ui.editRating, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdition()));
	connect(this->ui.textDescription, SIGNAL(textChanged()), this, SLOT(onTextEdition()));

	connect(this->ui.filepathTool, SIGNAL(clicked(bool)), this, SLOT(onFilepathTool()));
	connect(this->ui.newExecutable, SIGNAL(clicked(bool)), this, SLOT(onNewExecutable()));
	connect(this->ui.newResource, SIGNAL(clicked(bool)), this, SLOT(onNewResource()));
	connect(this->ui.deleteResource, SIGNAL(clicked(bool)), this, SLOT(onDeleteResource()));
	connect(this->ui.deleteExecutable, SIGNAL(clicked(bool)), this, SLOT(onDeleteExecutable()));
	connect(this->ui.save, SIGNAL(clicked(bool)), this, SLOT(onSave()));
	connect(this->ui.saveResource, SIGNAL(clicked(bool)), this, SLOT(onSaveResource()));
}

Executables::~Executables() {
	if (this->executables != nullptr) {
		delete this->executables;
	}
}

void Executables::onFilepathTool() {
	QFileDialog fileDialog(this);
	fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
	connect(&fileDialog, SIGNAL(fileSelected(const QString&)), this, SLOT(onFilepathSelected(const QString&)));
	fileDialog.exec();	
}

void Executables::onFilepathSelected(const QString& filepath) {
	QLineEdit* editFilepath = this->ui.editFilepath;
	editFilepath->setText(filepath);
	this->onTextEdition();
}

void Executables::onNewExecutable() {
	if (this->app->getSelectedPlatform().id == -1 || this->executables == NULL) {
		return;
	}

	Executable executable = this->app->getDb()->createNewExecutable(this->app->getSelectedPlatform().id);

	this->executables->append(executable);

	// add the item in the list of executables
	ItemWithId* item = new ItemWithId(executable.displayName, executable.id);
	QListWidget* listResources = this->getListExecutables();
	listResources->addItem(item);
	listResources->setCurrentItem(item); // select the item

	this->onExecutableSelected(item);
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
	// update the executable in the list of executable
	for (int i = 0; i < this->executables->count(); i++) {
		if (this->selectedExecutable.id == this->executables->at(i).id) {
			this->executables->replace(i, this->selectedExecutable);
		}
	}

	// add the item in the list of resources
	QString text;
	text.append(QString::number(res.id)).append(" - ");
	ItemWithId* item = new ItemWithId(text, res.id);
	QListWidget* listResources = this->getListResources();
	listResources->addItem(item);
	listResources->setCurrentItem(item); // select the item
	this->onResourceSelected(item);

	this->clearResource();

	// activate the filepath selection
	QToolButton* resourceOpen = this->ui.resourceFilepathOpen;
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
	QPushButton* saveResource = this->ui.saveResource;
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
	QRadioButton* cover = this->ui.cover;
	QRadioButton* screenshot = this->ui.screenshot;
	QRadioButton* fanart = this->ui.fanart;
	QRadioButton* logo = this->ui.logo;
	QRadioButton* video = this->ui.video;

	cover->setAutoExclusive(false);
	screenshot->setAutoExclusive(false);
	fanart->setAutoExclusive(false);
	logo->setAutoExclusive(false);
	video->setAutoExclusive(false);

	cover->setChecked(false);
	screenshot->setChecked(false);
	fanart->setChecked(false);
	logo->setChecked(false);
	video->setChecked(false);

	cover->setAutoExclusive(true);
	screenshot->setAutoExclusive(true);
	fanart->setAutoExclusive(true);
	logo->setAutoExclusive(true);
	video->setAutoExclusive(true);

	QLineEdit* resourceFilepath = this->ui.resourceFilepath;
	resourceFilepath->setText("");

	QLabel* labelImage = this->ui.image;
	labelImage->clear();

	QPushButton* saveResource = this->ui.saveResource;
	saveResource->setEnabled(false);
}

void Executables::onResourceFilepathSelected(const QString& filepath) {
	this->selectedResource.filepath = filepath;
	QPushButton* saveResource = this->ui.saveResource;
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
	QPushButton* deleteResource = this->ui.deleteResource;
	deleteResource->setEnabled(true);
}

void Executables::reloadResourceInfo() {
	ExecutableResource res = this->selectedResource;
	// set the fields.
	QLineEdit* filepath = this->ui.resourceFilepath;
	filepath->setText(res.filepath);
	
	QToolButton* resourceOpen = this->ui.resourceFilepathOpen;
	resourceOpen->setEnabled(true);

	QRadioButton* cover = this->ui.cover;
	QRadioButton* screenshot = this->ui.screenshot;
	QRadioButton* fanart = this->ui.fanart;
	QRadioButton* logo = this->ui.logo;
	QRadioButton* video = this->ui.video;

	cover->setChecked(false);
	screenshot->setChecked(false);
	fanart->setChecked(false);
	logo->setChecked(false);
	video->setChecked(false);

	if (res.type == "cover") {
		cover->setChecked(true);
	} else if (res.type == "screenshot") {
		screenshot->setChecked(true);
	} else if (res.type == "fanart") {
		fanart->setChecked(true);
	} else if (res.type == "logo") {
		logo->setChecked(true);
	} else if (res.type == "video") {
		video->setChecked(true);
	}

	this->displayImage(res.filepath);

}

void Executables::displayImage(QString filepath) {
	// display the image
	QLabel* labelImage = this->ui.image;
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

	// start by cleaning the view.
	this->clearResource();

	Executable e = this->findExecutable(id);
	
	// set the fields.
	QLineEdit* name = this->ui.editName;
	name->setText(e.displayName);
	QLineEdit* filepath = this->ui.editFilepath;
	filepath->setText(e.filepath);
	QLineEdit* genres = this->ui.editGenres;
	genres->setText(e.genres);;
	QLineEdit* publisher = this->ui.editPublisher;
	publisher->setText(e.publisher);;
	QLineEdit* developer = this->ui.editDeveloper;
	developer->setText(e.publisher);;
	QLineEdit* releaseDate = this->ui.editReleaseDate;
	releaseDate->setText(e.releaseDate);;
	QLineEdit* players = this->ui.editPlayers;
	players->setText(e.players);;
	QLineEdit* rating = this->ui.editRating;
	rating->setText(e.players);;
	QTextEdit* description = this->ui.textDescription;
	description->setText(e.description);;
	QPushButton* deleteExecutable = this->ui.deleteExecutable;

	// set the resources
	QListWidget* listResources = this->ui.listResources;
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
	QPushButton* save = this->ui.save;
	save->setEnabled(false);
	if (this->modifying) {
		save->setEnabled(true);
	}
}

void Executables::onSaveResource() {
	// update the executable values
	QLineEdit* filepath = this->ui.resourceFilepath;

	this->selectedResource.filepath = filepath->text();

	QRadioButton* cover = this->ui.cover;
	QRadioButton* screenshot = this->ui.screenshot;
	QRadioButton* fanart = this->ui.fanart;
	QRadioButton* logo = this->ui.logo;
	QRadioButton* video = this->ui.video;

	if (cover->isChecked()) {
		this->selectedResource.type = "cover";
	} else if (screenshot->isChecked()) {
		this->selectedResource.type = "screenshot";
	} else if (fanart->isChecked()) {
		this->selectedResource.type = "fanart";
	} else if (logo->isChecked()) {
		this->selectedResource.type = "logo";
	} else if (video->isChecked()) {
		this->selectedResource.type = "video";
	}

	Database* db = this->app->getDb();
	db->update(this->selectedResource);

	this->updateInternalResource(this->selectedResource);
	// update the executable in the list of executable
	for (int i = 0; i < this->executables->count(); i++) {
		if (this->selectedExecutable.id == this->executables->at(i).id) {
			this->executables->replace(i, this->selectedExecutable);
		}
	}
	
	QPushButton* saveResource = this->ui.saveResource;
	saveResource->setEnabled(false);
}

void Executables::onSave() {
	// update the executable values
	QLineEdit* name = this->ui.editName;
	QLineEdit* filepath = this->ui.editFilepath;
	QLineEdit* genres = this->ui.editGenres;
	QLineEdit* publisher = this->ui.editPublisher;
	QLineEdit* developer = this->ui.editDeveloper;
	QLineEdit* releaseDate = this->ui.editReleaseDate;
	QLineEdit* players = this->ui.editPlayers;
	QLineEdit* rating = this->ui.editRating;
	QTextEdit* description = this->ui.textDescription;

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
