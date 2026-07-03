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

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};
