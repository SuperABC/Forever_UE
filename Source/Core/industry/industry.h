#pragma once

#include "../map/map.h"
#include "../story/story.h"

#include "product.h"
#include "storage.h"
#include "manufacture.h"

#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>


class Industry {
public:
	Industry();
	~Industry();

	// 读取Mods
	void InitProducts(std::unordered_map<std::string, HMODULE>& modHandles,
		std::vector<std::string>& dlls);
	void InitStorages(std::unordered_map<std::string, HMODULE>& modHandles,
		std::vector<std::string>& dlls);
	void InitManufactures(std::unordered_map<std::string, HMODULE>& modHandles,
		std::vector<std::string>& dlls);

	// 读取配置文件
	void LoadConfigs() const;

	// 初始化全部工业
	void Init(Map* map);

	// 释放空间
	void Destroy();

	// 时钟前进
	void Tick(int day, int hour, int min, int sec);

	// 输出当前工业
	void Print() const;

	// 保存/加载工业
	void Load(const std::string& path);
	void Save(const std::string& path) const;

	// 应用变更
	void ApplyChange(Change* change, Story* story,
		std::vector<std::function<std::pair<bool, ValueType>(
			const std::string&)>>&getValues);

private:
	// Mod管理
	static ProductFactory* productFactory;
	static StorageFactory* storageFactory;
	static ManufactureFactory* manufactureFactory;

	std::vector<Storage*> storages;
	std::vector<Manufacture*> manufactures;
};

