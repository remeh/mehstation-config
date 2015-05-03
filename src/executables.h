#include <QList>
#include <QWidget>
#include <QListWidgetItem>

#include "app.h"
#include "models/executable.h"

#define MEH_ROLE_EXEC_ID 1002

class Executables : public QWidget {
		Q_OBJECT
	public:
		Executables(App* app, QWidget* parent);
		~Executables();

		void setExecutables(QList<Executable>* executables);

	private:
		Executable findExecutable(int id);
		ExecutableResource findResource(int id);
		void saveChangeState();
		void updateInternalExecutables(Executable executable);
		void updateInternalResource(ExecutableResource resource);
		void displayImage(QString filepath);
		void reloadResourceInfo();

		App* app;
		QWidget* mainWidget;
		QList<Executable>* executables;
		Executable selectedExecutable;
		ExecutableResource selectedResource;

		// true when something has been modified.
		bool modifying;

	public slots:
		void onExecutableSelected(QListWidgetItem* item);
		void onResourceFilepathClicked();
		void onResourceFilepathSelected(const QString& filepath);
		void onResourceSelected(QListWidgetItem* item);
		void onNewResource();
		void onTypeChanged();
		void onTextEdition();
		void onSave();
		void onSaveResource();
};

// Specific items for the executable list, containing the name and the
// ID is in the data QVariant.
class ItemWithId : public QListWidgetItem {
	public:
		ItemWithId(const QString& text, int id)
			: QListWidgetItem(text) {
			this->id = id;			
		}
		void setData(int role, const QVariant& value) {
			if (role == MEH_ROLE_EXEC_ID) {
				this->id = value.toInt();
				return;
			}
			QListWidgetItem::setData(role, value);
		}
		QVariant data(int role) const {
			if (role == MEH_ROLE_EXEC_ID) {
				return this->id;
			}
			return QListWidgetItem::data(role);
		}
		int id;
};
