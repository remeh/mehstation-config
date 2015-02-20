#include "app.h"

int main(int argc, char* argv[]) {
	App app(argc, argv);

	app.loadWindow();
	app.showWindow();

	return app.exec();
}
