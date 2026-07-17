#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"


class Industry {
public:
	/*
	* 构造工业
	*/
	Industry();

	/*
	* 析构工业
	*/
	~Industry();

	/*
	* 读取配置文件
	*/
	void LoadConfigs() const;

	/*
	* 读取产品模组
	* @modHandles: 已加载的模组句柄表
	* @dlls: 需加载的DLL路径列表
	*/
	void InitProducts(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	/*
	* 读取仓库模组
	* @modHandles: 已加载的模组句柄表
	* @dlls: 需加载的DLL路径列表
	*/
	void InitStorages(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	/*
	* 读取工坊模组
	* @modHandles: 已加载的模组句柄表
	* @dlls: 需加载的DLL路径列表
	*/
	void InitManufactures(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	/*
	* 初始化工业，创建仓库与工坊并建立上下游连接
	* @map: 地图对象
	*/
	void Init(Map* map);

	/*
	* 释放全部工业对象
	*/
	void Destroy();

	/*
	* 每日时钟周期，驱动生产结算与物流
	* @player: 玩家对象
	*/
	void Tick(Player* player);

	/*
	* 应用剧情变化
	* @change: 变化对象
	* @getValues: 值获取回调列表
	*/
	std::vector<Event*> ApplyChange(Change* change,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues);

	// 产品工厂
	static ProductFactory* productFactory;

	// 仓库工厂
	static StorageFactory* storageFactory;

	// 工坊工厂
	static ManufactureFactory* manufactureFactory;

private:
	// 全部仓库
	std::vector<Storage*> storages;

	// 全部工坊
	std::vector<Manufacture*> manufactures;

};
