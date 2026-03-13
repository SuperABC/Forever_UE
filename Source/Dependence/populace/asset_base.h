#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>


class Asset {
public:
	Asset();
	virtual ~Asset();

	// 子类实现方法

	// 统一类型定义
	static std::string GetId();
	virtual std::string GetType() const = 0;
	virtual std::string GetName() const = 0;

	// 父类实现方法

	// 获取资产名称
	const std::string& GetAsset() const;

	// 设置资产名称
	void SetAsset(const std::string& asset);

protected:
	std::string asset;
};

class AssetFactory {
public:
	// 注册资产
	void RegisterAsset(const std::string& id,
		std::function<Asset* ()> creator, std::function<void(Asset*)> deleter);

	// 创建资产（包含new操作）
	Asset* CreateAsset(const std::string& id);

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 析构资产（包含delete操作）
	void DestroyAsset(Asset* asset) const;

private:
	std::unordered_map<
		std::string,
		std::pair<std::function<Asset* ()>, std::function<void(Asset*)>>
	> registries;
	std::unordered_map<std::string, bool> configs;
};