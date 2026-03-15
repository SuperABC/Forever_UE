#include "job.h"


using namespace std;

// 默认职业

DefaultJob::DefaultJob() : Job() {

}

DefaultJob::~DefaultJob() {

}

string DefaultJob::GetId() {
    return "default";
}

string DefaultJob::GetType() const {
    return "default";
}

string DefaultJob::GetName() const {
    return "默认工作";
}

vector<string> DefaultJob::GetScripts() const {
    return { "scripts/jobs/default.json" };
}