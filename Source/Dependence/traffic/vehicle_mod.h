#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


// 载具Mod基类
class VehicleMod {
public:
	/*
	* 构造载具
	*/
	VehicleMod();

	/*
	* 析构载具
	*/
	virtual ~VehicleMod();

	/*
	* Override
	* 载具静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 载具动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 载具实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 自定义载具标签
	* @return: 载具标签
	*/
	COSTOM_INIT static std::string GetLabel();
	
	/*
	* Override
	* 自定义载具属性
	*/
	COSTOM_INIT virtual void SetupVehicle() = 0;

	// 载具资产路径
	std::string path;

	// 载具宽度
	float width;

	// 载具长度
	float length;

	// 关联剧情与脚本
	std::pair<std::string, std::vector<std::string>> script;
};

class VehicleFactory {
public:
	/*
	* 构造载具工厂,预留暂存区容量
	*/
	VehicleFactory();

	/*
	* 注册载具
	* @id: 载具类型
	* @label: 载具标签
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterVehicle(const std::string& id, std::string label,
		std::function<VehicleMod* ()> creator, std::function<void(VehicleMod*)> deleter);

	/*
	* 将暂存数据合并进正式注册表
	*/
	void MergeTemp();

	/*
	* 清空暂存数据
	*/
	void CleanTemp();

	/*
	* 清空所有注册
	*/
	void RemoveAll();

	/*
	* 创建载具
	* @label: 载具标签
	*/
	VehicleMod* CreateVehicle(const std::string& label) const;

	/*
	* 检查载具是否已注册
	* @id: 载具类型
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置载具启用配置
	* @name: 载具类型
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 析构载具
	* @vehicleMod: 待析构的载具
	*/
	void DestroyVehicle(VehicleMod* vehicleMod) const;

private:
	// 暂存本轮注册写入的数据,避免跨模块直接写正式成员
	struct Temp {
		// 注册表
		std::unordered_map<
			std::string,
			std::pair<std::function<VehicleMod* ()>, std::function<void(VehicleMod*)>>
		> registries;

		// 标签
		std::unordered_map<std::string, std::vector<std::string>> labels;
	};

	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<VehicleMod* ()>, std::function<void(VehicleMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;

	// 标签
	std::unordered_map<std::string, std::vector<std::string>> labels;

	// 暂存数据
	Temp temp;
};

