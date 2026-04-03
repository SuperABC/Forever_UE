#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class OrganizationMod {
public:
	// 无构造
	OrganizationMod();

	// 无析构
	virtual ~OrganizationMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 组织分配权重
	COSTOM_INIT static float GetPower();

	// 设置所需组合及数量范围
	COSTOM_INIT virtual void ComponentRequirements() = 0;

	// 设置工作岗位与日程
	COSTOM_INIT virtual void ArrageVacancies(const std::unordered_map<std::string, int>& components) = 0;

	// 分配工位
	COSTOM_INIT virtual void ArrageRoom(std::vector<std::pair<std::string, int>>& arrages,
		const std::vector<std::string>& rooms) = 0;

	// 需要的组合类型与数量上下限
	std::unordered_map<std::string, std::pair<int, int>> requirements;

	//								component	每个component	每个job					job类型		calendar类型
	// 全部组合中的工作岗位与日程
	std::unordered_map<std::string, std::vector<std::vector<std::pair<std::string, std::string>>>> vacancies;
};

class OrganizationFactory {
public:
	// 注册组织
	void RegisterOrganization(const std::string& id, float power,
		std::function<OrganizationMod* ()> creator, std::function<void(OrganizationMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建组织
	OrganizationMod* CreateOrganization(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 获取全部组织权重
	std::unordered_map<std::string, float> GetPowers() const;

	// 析构组织
	void DestroyOrganization(OrganizationMod* terrain) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<OrganizationMod* ()>, std::function<void(OrganizationMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;

	// 权重
	std::unordered_map<std::string, float> powers;
};

