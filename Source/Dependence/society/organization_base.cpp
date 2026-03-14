#include "../common/error.h"
#include "../common/utility.h"

#include "organization_base.h"


using namespace std;

// 全成员默认构造
Organization::Organization() :
    jobs() {

}

// 无析构
Organization::~Organization() {

}

vector<pair<Component*, vector<pair<Job*, int>>>>& Organization::GetJobs() {
    return jobs;
}

// 员工入职
vector<Job*> Organization::EnrollEmployee(const vector<int>& ids) {
    vector<Job*> positions;

    int i = 0;
    for (auto& [component, vacancies] : jobs) {
        for (auto& [job, employee] : vacancies) {
            if (employee < 0) {
                positions.push_back(job);
                if (i >= ids.size()) {
                    return positions;
                }
                employee = ids[i];
                ++i;
            }
        }
    }

    return positions;
}

// 获取全部职位
const vector<pair<Component*, vector<pair<Job*, int>>>>& Organization::GetJobs() const {
    // 一个组织连接多个Component，每个Component中对应若干组职业与雇员
    return jobs;
}

// 添加职缺
void Organization::AddVacancy(Component* component, const vector<Job*>& vacancies) {
    if (component == nullptr) {
        THROW_EXCEPTION(NullPointerException, "Vacancy component is null.\n");
    }
    vector<pair<Job*, int>> positions;
    for (auto vacancy : vacancies) {
        positions.emplace_back(vacancy, -1);
    }
    jobs.emplace_back(component, positions);
}

// 注册组织
void OrganizationFactory::RegisterOrganization(const string& id, float power,
    function<Organization* ()> creator, function<void(Organization*)> deleter) {
    registries[id] = { creator, deleter };
    powers[id] = power;
}

// 创建组织
Organization* OrganizationFactory::CreateOrganization(const string& id) const {
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Organization %s not enabled when creating.\n", id.data());
        return nullptr;
    }

    auto it = registries.find(id);
    if (it == registries.end()) {
        debugf("Warning: Organization %s not registered when creating.\n", id.data());
        return nullptr;
    }

    if (it->second.first) {
        return it->second.first();
    } else {
        THROW_EXCEPTION(NullPointerException, "Organization " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool OrganizationFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void OrganizationFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 获取全部组织权重
const unordered_map<string, float>& OrganizationFactory::GetPowers() const {
    return powers;
}

// 析构组织
void OrganizationFactory::DestroyOrganization(Organization* organization) const {
    if (!organization) {
        debugf("Warning: Organization is null when deleting.\n");
        return;
    }

    auto it = registries.find(organization->GetType());
    if (it == registries.end()) {
        debugf("Warning: Organization %s not registered when deleting.\n", organization->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(organization);
    } else {
        THROW_EXCEPTION(NullPointerException, "Organization " + organization->GetType() + " deleter is null.\n");
    }
}

