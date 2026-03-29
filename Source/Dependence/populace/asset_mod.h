#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class AssetMod {
public:
	// 无构造
	AssetMod();

	// 无析构
	virtual ~AssetMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 资产描述
	std::string asset;
};

class AssetFactory {
public:
	// 注册资产
	void RegisterAsset(const std::string& id,
		std::function<AssetMod* ()> creator, std::function<void(AssetMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建资产
	AssetMod* CreateAsset(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 析构资产
	void DestroyAsset(AssetMod* terrain) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<AssetMod* ()>, std::function<void(AssetMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

