#include "organization_base.h"
#include "../common/error.h"
#include "../common/utility.h"

using namespace std;

vector<Job*> Organization::EnrollEmployee(vector<int>& ids) {
    vector<Job*> positions;

    int i = 0;
    for (auto& component : jobs) {
        for (auto& vacancy : component.second) {
            if (vacancy.second < 0) {
                positions.push_back(vacancy.first);
                vacancy.second = ids[i++];
                if (i >= ids.size()) return positions;
            }
        }
    }

    return positions;
}

vector<pair<Component*, vector<pair<Job*, int>>>> Organization::GetJobs() const {
    return jobs;
}

void Organization::AddVacancy(Component* component, vector<Job*> vacancies) {
    vector<pair<Job*, int>> positions;
    for (auto vacancy : vacancies) {
        positions.push_back(make_pair(vacancy, -1));
    }
    jobs.push_back(make_pair(component, positions));
}

void OrganizationFactory::RegisterOrganization(const string& id, float power,
    function<Organization*()> creator, function<void(Organization*)> deleter) {
    registries[id] = {creator, deleter};
    powers[id] = power;
}

Organization* OrganizationFactory::CreateOrganization(const string& id) {
    if (configs.find(id) == configs.end() || !configs.find(id)->second)
        return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first();
    }
    return nullptr;
}

bool OrganizationFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void OrganizationFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}

const unordered_map<string, float>& OrganizationFactory::GetPowers() const {
    return powers;
}

void OrganizationFactory::DestroyOrganization(Organization* organization) const {
    if (!organization) return;
    auto it = registries.find(organization->GetType());
    if (it != registries.end()) {
        it->second.second(organization);
    } else {
        debugf(("Deleter not found for " + organization->GetType() + ".\n").data());
    }
}