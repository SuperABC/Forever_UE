#include "../common/error.h"
#include "../common/utility.h"

#include "organization.h"


using namespace std;

// 默认组织

string DefaultOrganization::GetId() {
    return "default";
}

string DefaultOrganization::GetType() const {
    return "default";
}

string DefaultOrganization::GetName() const {
    return "默认组织";
}

float DefaultOrganization::GetPower() {
    return 1.0f;
}

vector<pair<string, pair<int, int>>> DefaultOrganization::ComponentRequirements() const {
    vector<pair<string, pair<int, int>>> requirements;
    requirements.emplace_back("default_working", make_pair(1, 1));
    return requirements;
}

vector<pair<string, vector<vector<string>>>> DefaultOrganization::ArrageVacancies(
    const vector<pair<string, int>>& components) const {
    vector<pair<string, vector<vector<string>>>> jobs;
    for (const auto& [name, count] : components) {
        jobs.emplace_back(name, vector<vector<string>>(count));
        for (int i = 0; i < count; ++i) {
            jobs.back().second[i] = vector<string>(10, "default");
        }
    }
    return jobs;
}

void DefaultOrganization::SetCalendar(CalendarFactory* factory) {
    if (factory == nullptr) {
        THROW_EXCEPTION(NullPointerException, "Calendar factory is null.\n");
    }
    for (auto& [component, vacancies] : GetJobs()) {
        for (auto& [job, occupantId] : vacancies) {
            Calendar* calendar = factory->CreateCalendar("full");
            if (calendar) {
                job->SetCalendar(calendar);
            }
        }
    }
}

void DefaultOrganization::ArrangeRooms() {
    for (auto& [component, vacancies] : GetJobs()) {
        if (component == nullptr) {
            debugf("Warning: Component is null.\n");
            continue;
        }
        const auto& rooms = component->GetRooms();
        if (rooms.empty()) {
            debugf("Warning: Component has no rooms.\n");
            continue;
        }
        for (auto& [job, occupantId] : vacancies) {
            job->SetPosition(rooms[0]);
        }
    }
}

