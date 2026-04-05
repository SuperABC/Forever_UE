#pragma once

#include "../common/utility.h"
#include "../common/error.h"

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
};

class StationFactory {
public:
	// 注册站点
	void RegisterStation(const std::string& id,
		std::function<StationMod* ()> creator, std::function<void(StationMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建站点
	StationMod* CreateStation(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 析构站点
	void DestroyStation(StationMod* terrain) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<StationMod* ()>, std::function<void(StationMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

