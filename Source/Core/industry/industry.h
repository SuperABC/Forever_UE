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

	// 设置资源路径
	void SetResourcePath(const std::string& path);

	// 读取Mods
	void InitProducts(std::unordered_map<std::string, HMODULE>& modHandles);
	void InitStorages(std::unordered_map<std::string, HMODULE>& modHandles);
	void InitManufactures(std::unordered_map<std::string, HMODULE>& modHandles);

	// 初始化全部工业
	void Init(Map* map);

	// 读取配置文件
	void ReadConfigs(const std::string& path) const;

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
	// 资源路径
	std::string resourcePath;

	// Mod管理
	static ProductFactory* productFactory;
	static StorageFactory* storageFactory;
	static ManufactureFactory* manufactureFactory;

	std::vector<Storage*> storages;
	std::vector<Manufacture*> manufactures;
};

