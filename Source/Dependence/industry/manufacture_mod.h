#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


// 工坊Mod基类
class ManufactureMod {
public:
	/*
	* 构造工坊
	*/
	ManufactureMod();

	/*
	* 析构工坊
	*/
	virtual ~ManufactureMod();

	/*
	* Override
	* 工坊静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 工坊动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 工坊实例唯一名称
	* @return: 实例唯一名称
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
	* @id: 工坊类型
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterManufacture(const std::string& id,
		std::function<ManufactureMod* ()> creator, std::function<void(ManufactureMod*)> deleter);

	/*
	* 清空所有注册
	*/
	void RemoveAll();

	/*
	* 创建工坊
	* @id: 工坊类型
	*/
	ManufactureMod* CreateManufacture(const std::string& id) const;

	/*
	* 检查工坊是否已注册
	* @id: 工坊类型
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置工坊启用配置
	* @name: 工坊类型
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 析构工坊
	* @manufactureMod: 待析构的工坊
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
