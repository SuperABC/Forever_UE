#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include "geometry.h"

#include <string>
#include <functional>


class ZoneMod {
public:
	// 无构造
	ZoneMod();

	// 无析构
	virtual ~ZoneMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 指定园区数量
	COMSTOM_INIT static std::function<int(const Lot*)> ZoneAssigner;

	// 设计园区
	COMSTOM_INIT virtual void LayoutZone(const Lot* lot) = 0;

	// 园区面积
	float acreage;

	// 内部建筑
	std::vector<std::pair<std::string, float>> buildings;

	// 关联剧情与脚本
	std::pair<std::string, std::vector<std::string>> script;
};

class ZoneFactory {
public:
	// 注册园区
	void RegisterZone(const std::string& id, std::function<int(const Lot*)> assigner,
		std::function<ZoneMod* ()> creator, std::function<void(ZoneMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建园区
	ZoneMod* CreateZone(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 获取所有启用园区
	std::vector<std::string> GetTypes() const;

	// 在地块内生成一类园区
	std::vector<ZoneMod*> CreateZones(const std::string& type, const Lot* lot) const;

	// 析构园区
	void DestroyZone(ZoneMod* zone) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<ZoneMod* ()>, std::function<void(ZoneMod*)>>
	> registries;
	
	// 启用配置
	std::unordered_map<std::string, bool> configs;
	
	// 园区生成器
	std::unordered_map<std::string, std::function<int(const Lot*)>> assigners;
};

