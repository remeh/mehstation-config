#pragma once

#include <QString>
#include "model.h"

class ExecutableResource : public Model {
	public:
		ExecutableResource();
		ExecutableResource(int id, int executableId, QString filepath, QString type);
		~ExecutableResource();

		int id;
		int executableId;
		QString filepath;
		QString type;
};
