#pragma once

#include "society/organization_mod.h"


// 商店组织
class ShopOrganization : public OrganizationMod {
public:
	/*
	* 构造商店组织
	*/
	ShopOrganization();

	/*
	* 析构商店组织
	*/
	virtual ~ShopOrganization();

	/*
	* Override
	* 组织静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 组织动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 组织实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 组织分配权重
	*/
	static float GetPower();

	/*
	* Override
	* 设置所需组合及数量范围
	*/
	virtual void ComponentRequirements() override;

	/*
	* Override
	* 根据实际组合数量安排工作岗位与日程
	* @components: 各组合类型的实际数量
	*/
	virtual void ArrageVacancies(const std::unordered_map<std::string, int>& components) override;

	/*
	* Override
	* 为工作岗位分配工位
	* @arrangements: 各岗位类型与分配到的工位序号
	* @rooms: 可用工位类型列表
	*/
	virtual void ArrageRoom(std::vector<std::pair<std::string, int>>& arrangements,
		const std::vector<std::string>& rooms) override;

	/*
	* Override
	* 初始化组织
	*/
	virtual void InitOrganization() override;

	/*
	* Override
	* 每日规划
	* @time: 当前游戏时间
	* @post: 向Core发起查询的句柄
	*/
	virtual void DailyPlan(const Time& time, PostHandle* post) override;

	/*
	* Override
	* 规划节点调用
	* @name: 节点名称
	* @storyScript, organizationScript: 主线剧情脚本、组织自身剧情脚本
	* @post: 向Core发起查询的句柄
	*/
	virtual void ExecNode(const std::string& name,
		Container* storyScript, Container* organizationScript, PostHandle* post) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

// 医院组织
class HospitalOrganization : public OrganizationMod {
public:
	/*
	* 构造医院组织
	*/
	HospitalOrganization();

	/*
	* 析构医院组织
	*/
	virtual ~HospitalOrganization();

	/*
	* Override
	* 组织静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 组织动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 组织实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 组织分配权重
	*/
	static float GetPower();

	/*
	* Override
	* 设置所需组合及数量范围
	*/
	virtual void ComponentRequirements() override;

	/*
	* Override
	* 根据实际组合数量安排工作岗位与日程
	* @components: 各组合类型的实际数量
	*/
	virtual void ArrageVacancies(const std::unordered_map<std::string, int>& components) override;

	/*
	* Override
	* 为工作岗位分配工位
	* @arrangements: 各岗位类型与分配到的工位序号
	* @rooms: 可用工位类型列表
	*/
	virtual void ArrageRoom(std::vector<std::pair<std::string, int>>& arrangements,
		const std::vector<std::string>& rooms) override;

	/*
	* Override
	* 初始化组织
	*/
	virtual void InitOrganization() override;

	/*
	* Override
	* 每日规划
	* @time: 当前游戏时间
	* @post: 向Core发起查询的句柄
	*/
	virtual void DailyPlan(const Time& time, PostHandle* post) override;

	/*
	* Override
	* 规划节点调用
	* @name: 节点名称
	* @storyScript, organizationScript: 主线剧情脚本、组织自身剧情脚本
	* @post: 向Core发起查询的句柄
	*/
	virtual void ExecNode(const std::string& name,
		Container* storyScript, Container* organizationScript, PostHandle* post) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

// 酒店组织
class HotelOrganization : public OrganizationMod {
public:
	/*
	* 构造酒店组织
	*/
	HotelOrganization();

	/*
	* 析构酒店组织
	*/
	virtual ~HotelOrganization();

	/*
	* Override
	* 组织静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 组织动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 组织实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 组织分配权重
	*/
	static float GetPower();

	/*
	* Override
	* 设置所需组合及数量范围
	*/
	virtual void ComponentRequirements() override;

	/*
	* Override
	* 根据实际组合数量安排工作岗位与日程
	* @components: 各组合类型的实际数量
	*/
	virtual void ArrageVacancies(const std::unordered_map<std::string, int>& components) override;

	/*
	* Override
	* 为工作岗位分配工位
	* @arrangements: 各岗位类型与分配到的工位序号
	* @rooms: 可用工位类型列表
	*/
	virtual void ArrageRoom(std::vector<std::pair<std::string, int>>& arrangements,
		const std::vector<std::string>& rooms) override;

	/*
	* Override
	* 初始化组织
	*/
	virtual void InitOrganization() override;

	/*
	* Override
	* 每日规划
	* @time: 当前游戏时间
	* @post: 向Core发起查询的句柄
	*/
	virtual void DailyPlan(const Time& time, PostHandle* post) override;

	/*
	* Override
	* 规划节点调用
	* @name: 节点名称
	* @storyScript, organizationScript: 主线剧情脚本、组织自身剧情脚本
	* @post: 向Core发起查询的句柄
	*/
	virtual void ExecNode(const std::string& name,
		Container* storyScript, Container* organizationScript, PostHandle* post) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};
