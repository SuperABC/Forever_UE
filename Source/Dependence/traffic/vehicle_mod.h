#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class VehicleMod {
public:
	// 无构造
	VehicleMod();

	// 无析构
	virtual ~VehicleMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;
};

class VehicleFactory {
public:
	// 注册载具
	void RegisterVehicle(const std::string& id,
		std::function<VehicleMod* ()> creator, std::function<void(VehicleMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建载具
	VehicleMod* CreateVehicle(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 析构载具
	void DestroyVehicle(VehicleMod* terrain) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<VehicleMod* ()>, std::function<void(VehicleMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

