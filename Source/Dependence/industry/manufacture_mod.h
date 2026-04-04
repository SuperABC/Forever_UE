#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class ManufactureMod {
public:
	// 无构造
	ManufactureMod();

	// 无析构
	virtual ~ManufactureMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 设置生产目标
	COSTOM_INIT virtual void SetTargets() = 0;

	// 生产目标
	std::unordered_map<std::string, float> targets;
};

class ManufactureFactory {
public:
	// 注册工坊
	void RegisterManufacture(const std::string& id,
		std::function<ManufactureMod* ()> creator, std::function<void(ManufactureMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建工坊
	ManufactureMod* CreateManufacture(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 析构工坊
	void DestroyManufacture(ManufactureMod* terrain) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<ManufactureMod* ()>, std::function<void(ManufactureMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

