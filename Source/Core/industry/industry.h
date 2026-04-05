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

	// 读取 Mods
	void InitProducts(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);
	void InitStorages(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);
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

	// 统一工厂
	static ProductFactory* productFactory;
	static StorageFactory* storageFactory;
	static ManufactureFactory* manufactureFactory;

private:
	// 全部仓库
	std::vector<Storage*> storages;

	// 全部工坊
	std::vector<Manufacture*> manufactures;

};

