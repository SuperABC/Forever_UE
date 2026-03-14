#pragma once

#include "../map/component_base.h"
#include "../society/job_base.h"
#include "../society/calendar_base.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#undef AddJob


class Organization {
public:
    Organization();
    virtual ~Organization();

    // 子类实现方法

    // 统一类型定义
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 组织权重
    static float GetPower();

    // 所需组合及数量范围
    // 返回每个需要的组合类型与组合数量上下限
    virtual std::vector<std::pair<std::string, std::pair<int, int>>> ComponentRequirements() const = 0;

    // 根据实际组合安排工作岗位
    // 输入全部的组合类型与组合数量，返回所有组合类型与每个类型中所有组合中的根据数量重复的职业类型
    virtual std::vector<std::pair<std::string, std::vector<std::vector<std::string>>>> ArrageVacancies(
        const std::vector<std::pair<std::string, int>>& components) const = 0;

    // 设定工作日历
    virtual void SetCalendar(CalendarFactory* factory) = 0;

    // 分配工位
    virtual void ArrangeRooms() = 0;

    // 父类实现方法

    // 获取全部组织职位
    std::vector<std::pair<Component*, std::vector<std::pair<Job*, int>>>>& GetJobs();

    // 员工入职
    virtual std::vector<Job*> EnrollEmployee(const std::vector<int>& ids);

    // 获取全部职位
    const std::vector<std::pair<Component*, std::vector<std::pair<Job*, int>>>>& GetJobs() const;

    // 添加职缺
    void AddVacancy(Component* component, const std::vector<Job*>& vacancies);

private:
    std::vector<std::pair<Component*, std::vector<std::pair<Job*, int>>>> jobs;
};

class OrganizationFactory {
public:
    // 注册组织
    void RegisterOrganization(const std::string& id, float power,
        std::function<Organization* ()> creator, std::function<void(Organization*)> deleter);

    // 创建组织（包含new操作）
    Organization* CreateOrganization(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 获取全部组织权重
    const std::unordered_map<std::string, float>& GetPowers() const;

    // 析构组织（包含delete操作）
    void DestroyOrganization(Organization* organization) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Organization* ()>, std::function<void(Organization*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
    std::unordered_map<std::string, float> powers;
};

