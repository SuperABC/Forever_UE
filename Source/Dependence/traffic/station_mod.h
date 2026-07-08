#pragma once

#include "../common/utility.h"
#include "../common/error.h"
#include "../map/geometry.h"

#include <string>
#include <functional>


class StationMod {
public:
	// 无构造
	StationMod();

	// 无析构
	virtual ~StationMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 划分区块
	COSTOM_INIT static std::function<std::vector<int>&(const std::vector<Lot*>& blocks)> StationAssigner;

	// 布局站点
	COSTOM_INIT virtual void LayoutStation(const Lot* block) = 0;

	// 布局站点
	COSTOM_INIT virtual void PlaceInterface(const Quad* building) = 0;

	// 建筑类型
	std::string buildingType;

	// 建筑面积
	int buildingAcreage;

	// 接口向量
	std::vector<std::pair<std::string, std::vector<float>>> interfaces;

};

class StationFactory {
public:
	// 注册站点
	void RegisterStation(const std::string& id,
		std::function<std::vector<int>& (const std::vector<Lot*>& blocks)> assigner,
		std::function<StationMod* ()> creator, std::function<void(StationMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建站点
	StationMod* CreateStation(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 获取所有启用站点
	std::vector<std::string> GetTypes() const;

	// 分配一类站点
	std::vector<int> AssignStations(const std::string& type, const std::vector<Lot*>& blocks) const;

	// 析构站点
	void DestroyStation(StationMod* stationMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<StationMod* ()>, std::function<void(StationMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;

	// 站点生成器
	std::unordered_map<std::string,
		std::function<std::vector<int>& (const std::vector<Lot*>& blocks)>> assigners;
};

