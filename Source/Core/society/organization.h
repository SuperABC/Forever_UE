#pragma once

#include "organization_mod.h"

#include "job.h"

#include "map/component.h"


// 子类注册函数
typedef void (*RegisterModOrganizationsFunc)(OrganizationFactory* factory);

// 组织实体
class Organization {
public:
	// 禁止默认构造
	Organization() = delete;

	// 通过类型从工厂构造
	Organization(OrganizationFactory* factory, const std::string& component);

	// 析构组织
	~Organization();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 返回每个需要的组合类型与组合数量上下限
	std::unordered_map<std::string, std::pair<int, int>> ComponentRequirements() const;

	// 根据实际组合数量安排工作岗位
	std::unordered_map<std::string, std::vector<std::vector<std::pair<std::string, std::string>>>> ArrageVacancies(
		const std::unordered_map<std::string, int>& components);

	// 添加职缺
	void AddVacancy(Component* component, const std::vector<Job*>& vacancies);

	// 员工入职
	virtual std::vector<Job*> EnrollEmployee(const std::vector<Person*>& candidates);

	// 分配工位
	void ArrangeRooms();

private:
	// 模组对象
	OBJECT_HOLDER OrganizationMod* mod;

	// 工厂
	OrganizationFactory* factory;

	// 组织类型
	std::string type;

	// 组织名称
	std::string name;

	// 员工
	std::vector<std::pair<Component*, std::vector<std::pair<Job*, Person*>>>> jobs;
};

// 空组织
class EmptyOrganization : public OrganizationMod {
public:
	EmptyOrganization();
	virtual ~EmptyOrganization();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static float GetPower();

	virtual void ComponentRequirements() override;

	virtual void ArrageVacancies(const std::unordered_map<std::string, int>& components) override;

	virtual void ArrageRoom(std::vector<std::pair<std::string, int>>& arrages,
		const std::vector<std::string>& rooms);

private:
	static int count;

	int id;
	std::string name;
};

