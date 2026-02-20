#include "job_base.h"


using namespace std;

void Job::SetCalendar(Calendar* calendar) {
    this->calendar = calendar;
}

Calendar* Job::GetCalendar() const {
    return calendar;
}

void Job::SetPosition(Room* room) {
    position = room;
}

Room* Job::GetPosition() const {
    return position;
}

void JobFactory::RegisterJob(const string& id, function<Job* ()> creator) {
    registries[id] = creator;
}

Job* JobFactory::CreateJob(const string& id) {
    if(configs.find(id) == configs.end() || !configs.find(id)->second)return nullptr;
    
    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second();
    }
    return nullptr;
}

bool JobFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void JobFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}

