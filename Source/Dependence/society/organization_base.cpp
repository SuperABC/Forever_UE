#include "../common/error.h"
#include "../common/utility.h"

#include "organization_base.h"


using namespace std;

Organization::Organization() :
    jobs() {
    // 全成员默认构造

}

Organization::~Organization() {
    // 无析构
}

vector<Job*> Organization::EnrollEmployee(const vector<int>& ids) {
    // 员工入职
    vector<Job*> positions;

    int i = 0;
    for (auto& [component, vacancies] : jobs) {
        for (auto& [job, occupantId] : vacancies) {
            if (occupantId < 0) {
                positions.push_back(job);
                if (i >= ids.size()) {
                    return positions;
                }
                occupantId = ids[i];
                ++i;
            }
        }
    }

    return positions;
}

const vector<pair<Component*, vector<pair<Job*, int>>>>& Organization::GetJobs() const {
    // 获取全部职位
    return jobs;
}

void Organization::AddVacancy(Component* component, const vector<Job*>& vacancies) {
    // 添加职缺
    if (component == nullptr) {
        THROW_EXCEPTION(NullPointerException, "Vacancy component is null.\n");
    }
    vector<pair<Job*, int>> positions;
    for (auto vacancy : vacancies) {
        positions.emplace_back(vacancy, -1);
    }
    jobs.emplace_back(component, positions);
}

void OrganizationFactory::RegisterOrganization(const string& id, float power,
    function<Organization* ()> creator, function<void(Organization*)> deleter) {
    // 注册构造器和析构器
    registries[id] = { creator, deleter };
    powers[id] = power;
}

Organization* OrganizationFactory::CreateOrganization(const string& id) {
    // 根据配置构造组织
    auto configIt = configs.find(id);
    if (configIt == configs.end() || !configIt->second) {
        return nullptr;
    }

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first();
    }
    return nullptr;
}

bool OrganizationFactory::CheckRegistered(const string& id) {
    // 检查是否注册
    return registries.find(id) != registries.end();
}

void OrganizationFactory::SetConfig(const string& name, bool config) {
    // 设置启用配置
    configs[name] = config;
}

const unordered_map<string, float>& OrganizationFactory::GetPowers() const {
    // 获取全部组织权重
    return powers;
}

void OrganizationFactory::DestroyOrganization(Organization* organization) const {
    // 析构组织
    if (!organization) {
        return;
    }
    auto it = registries.find(organization->GetType());
    if (it != registries.end()) {
        it->second.second(organization);
    }
    else {
        debugf("Deleter not found for %s.\n", organization->GetType().data());
    }
}

