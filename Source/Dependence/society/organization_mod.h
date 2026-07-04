#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include "../story/change.h"

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>


class Script;


class OrganizationMod {
public:
	/*
	* 无构造
	*/
	OrganizationMod();

	/*
	* 无析构
	*/
	virtual ~OrganizationMod();

	/*
	* Override
	* 组织静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 组织动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 组织实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 组织分配权重
	*/
	COSTOM_INIT static float GetPower();

	/*
	* Override
	* 设置所需组合及数量范围，结果写入requirements字段
	*/
	COSTOM_INIT virtual void ComponentRequirements() = 0;

	/*
	* Override
	* 根据实际组合数量安排工作岗位与日程，结果写入vacancies字段
	* @components: 各组合类型的实际数量
	*/
	COSTOM_INIT virtual void ArrageVacancies(const std::unordered_map<std::string, int>& components) = 0;

	/*
	* Override
	* 为工作岗位分配工位
	* @arrangements: 各岗位类型与分配到的工位序号
	* @rooms: 可用工位类型列表
	*/
	COSTOM_INIT virtual void ArrageRoom(std::vector<std::pair<std::string, int>>& arrangements,
		const std::vector<std::string>& rooms) = 0;

	/*
	* Override
	* 初始化组织，设置关联剧情信息（写入script字段）
	*/
	COSTOM_INIT virtual void InitOrganization() = 0;

	/*
	* Override
	* 每日规划，填充plans字段
	* @time: 当前游戏时间
	*/
	COSTOM_RUNTIME virtual void DailyPlan(const Time& time) = 0;

	/*
	* Override
	* 规划节点调用，结果写入changes字段
	* @name: 节点名称
	* @storyScript, organizationScript: 主线剧情脚本、组织自身剧情脚本
	*/
	COSTOM_RUNTIME virtual void ExecNode(const std::string& name,
		Container* storyScript, Container* organizationScript) = 0;

	// 关联剧情与脚本
	std::pair<std::string, std::vector<std::string>> script;

	// 每日计划（节点名 -> 触发时刻）
	std::unordered_map<std::string, Time> plans;

	// 节点执行产出的变化列表
	std::vector<ChangeValue> changes;

	// 需要的组合类型与数量上下限
	std::unordered_map<std::string, std::pair<int, int>> requirements;

	//								component	每个component	每个job					job类型		calendar类型
	// 全部组合中的工作岗位与日程
	std::unordered_map<std::string, std::vector<std::vector<std::pair<std::string, std::string>>>> vacancies;
};

class OrganizationFactory {
public:
	/*
	* 注册组织
	* @id: 组织静态类型标识
	* @power: 分配权重
	* @creator, deleter: 构造与析构方法
	*/
	void RegisterOrganization(const std::string& id, float power,
		std::function<OrganizationMod* ()> creator, std::function<void(OrganizationMod*)> deleter);

	/*
	* 清空注册
	*/
	void RemoveAll();

	/*
	* 创建组织
	* @id: 组织静态类型标识
	*/
	OrganizationMod* CreateOrganization(const std::string& id) const;

	/*
	* 检查是否注册
	* @id: 组织静态类型标识
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置启用配置
	* @name: 组织静态类型标识
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 获取全部已启用组织的权重
	*/
	std::unordered_map<std::string, float> GetPowers() const;

	/*
	* 析构组织
	* @organizationMod: 待析构的组织模组对象
	*/
	void DestroyOrganization(OrganizationMod* organizationMod) const;

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

