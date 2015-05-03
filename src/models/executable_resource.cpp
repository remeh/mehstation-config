#include "executable_resource.h"

ExecutableResource::ExecutableResource() :
	id(-1),
	executableId(-1) {
}
ExecutableResource::ExecutableResource(int id, int executableId, QString filepath, QString type) :
	id(id),
	executableId(executableId),
	filepath(filepath),
	type(type) {
}

ExecutableResource::~ExecutableResource() {
}
