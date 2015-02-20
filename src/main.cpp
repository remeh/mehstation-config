#include <iostream>

#include "app.h"

int main(int argc, char* argv[]) {
	App app(argc, argv);

	if (!app.loadWindow()) {
		std::cerr << "[err] Something bad happened during the initialization.\n[err] Exiting" << std::endl;
		return 1;
	}

	app.showWindow();

	return app.exec();
}
