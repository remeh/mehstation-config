#include <QList>
#include <QWidget>
#include <QListWidgetItem>

#include "app.h"

#define MEH_ROLE_EXEC_ID 1002

class Executables : public QWidget {
		Q_OBJECT
	public:
		Executables(App* app, QWidget* parent);
		~Executables();
		App* app;

		QWidget* mainWidget;

		void setExecutables(QList<Executable>* executables);

		Executable findExecutable(int id);

	private:
		void saveChangeState();

		QList<Executable>* executables;

		// true when something has been modified.
		bool modifying;

	public slots:
		void onExecutableSelected(QListWidgetItem* item);
		void onTextEdition();
		void onSave();
};

// Specific items for the executable list, containing the name and the
// ID is in the data QVariant.
class ExecutableItem : public QListWidgetItem {
	public:
		ExecutableItem(const QString& text, int id)
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
