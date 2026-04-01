#pragma once

#include "asset_mod.h"


// 子类注册函数
typedef void (*RegisterModAssetsFunc)(AssetFactory* factory);

// 资产实体
class Asset {
public:
	// 禁止默认构造
	Asset() = delete;

	// 通过类型从工厂构造
	Asset(AssetFactory* factory, const std::string& component);

	// 析构资产
	~Asset();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 定义资产
	void DefineAsset();

	// 获取资产描述
	std::string GetAsset();

	// 设置资产描述
	void SetAsset(const std::string& asset);

private:
	// 模组对象
	OBJECT_HOLDER AssetMod* mod;

	// 工厂
	AssetFactory* factory;

	// 资产类型
	std::string type;

	// 资产名称
	std::string name;
	
	// 资产描述
	std::string asset;
};

// 空资产
class EmptyAsset : public AssetMod {
public:
	EmptyAsset();
	virtual ~EmptyAsset();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void DefineAsset();

private:
	static int count;

	int id;
	std::string name;
};

