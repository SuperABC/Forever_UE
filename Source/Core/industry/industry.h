#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"


class Industry {
public:
	// 构造工业
	Industry();

	// 析构工业
	~Industry();

	// 读取配置文件
	void LoadConfigs() const;

	// 读取产品模组
	void InitProducts(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 读取仓库模组
	void InitStorages(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 读取工坊模组
	void InitManufactures(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 初始化工业
	void Init(Map* map);

	// 释放空间
	void Destroy();

	// 时钟周期
	void Tick(Player* player);

	// 应用剧情变化
	void ApplyChange(Change* change,
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

