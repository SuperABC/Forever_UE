#include "scheduler.h"


using namespace std;

// 纯工作调度

WorkOnlyScheduler::WorkOnlyScheduler() {

}

WorkOnlyScheduler::~WorkOnlyScheduler() {

}

string WorkOnlyScheduler::GetId() {
	return "work_only";
}

string WorkOnlyScheduler::GetType() const {
	return "work_only";
}

string WorkOnlyScheduler::GetName() const {
	return "纯工作调度";
}

