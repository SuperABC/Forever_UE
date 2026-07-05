#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class ManufactureMod {
public:
	/*
	* 构造工坊模组基类
	*/
	ManufactureMod();

	/*
	* 析构工坊模组基类
	*/
	virtual ~ManufactureMod();

	/*
	* Override
	* 工坊静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 工坊动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 工坊实例名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 设置生产目标
	*/
	COSTOM_INIT virtual void SetTargets() = 0;

	// 生产目标
	std::unordered_map<std::string, float> targets;

};

class ManufactureFactory {
public:
	/*
	* 注册工坊
	* @id: 工坊静态类型标识
	* @creator: 创建函数
	* @deleter: 析构函数
	*/
	void RegisterManufacture(const std::string& id,
		std::function<ManufactureMod* ()> creator, std::function<void(ManufactureMod*)> deleter);

	/*
	* 清空启用配置
	*/
	void RemoveAll();

	/*
	* 创建工坊实例
	* @id: 工坊静态类型标识
	*/
	ManufactureMod* CreateManufacture(const std::string& id) const;

	/*
	* 检查是否已注册
	* @id: 工坊静态类型标识
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置启用配置
	* @name: 工坊静态类型标识
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 析构工坊实例
	* @manufactureMod: 待析构的工坊模组
	*/
	void DestroyManufacture(ManufactureMod* manufactureMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<ManufactureMod* ()>, std::function<void(ManufactureMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;

};
