#include "scheduler_basic.h"


using namespace std;

int WorkonlyScheduler::count = 0;

WorkonlyScheduler::WorkonlyScheduler() : id(count++) {

}

WorkonlyScheduler::~WorkonlyScheduler() {

}

const char* WorkonlyScheduler::GetId() {
	return "workonly";
}

const char* WorkonlyScheduler::GetType() const {
	return "workonly";
}

const char* WorkonlyScheduler::GetName() {
	name = "纯工作调度" + to_string(id);
	return name.data();
}

float WorkonlyScheduler::GetPower() {
	return 0.f;
}

void WorkonlyScheduler::InitScheduler() {
	script = { "empty", { "basic_scheduler" } };
}
