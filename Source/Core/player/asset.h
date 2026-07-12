#pragma once

#include "player/asset_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModAssetsFunc)(AssetFactory* factory);

// 资产实体
class Asset {
public:
	/*
	* 禁止默认构造
	*/
	Asset() = delete;

	/*
	* 构造资产实体
	* @factory: 资产工厂
	* @asset: 资产类型
	*/
	Asset(AssetFactory* factory, const std::string& asset);

	/*
	* 析构资产
	*/
	~Asset();

	/*
	* 应用动态类型标识
	* @return: 动态类型标识
	*/
	std::string GetType() const;

	/*
	* 应用实例唯一名称
	* @return: 实例唯一名称
	*/
	std::string GetName() const;

	/*
	* 定义资产属性
	*/
	void DefineAsset();

	/*
	* 获取资产描述文本
	* @return: 描述文本
	*/
	std::string GetAsset();

	/*
	* 设置资产描述文本
	* @asset: 描述文本
	*/
	void SetAsset(const std::string& asset);

	/*
	* 获取资产移动性
	* @return: 资产移动性
	*/
	ASSET_MOBILITY GetMobility() const;

	/*
	* 获取资产重量
	* @return: 资产重量
	*/
	float GetWeight() const;

	/*
	* 获取资产体积
	* @return: 资产体积
	*/
	float GetSize() const;

	/*
	* 获取资产容积
	* @return: 资产容积
	*/
	float GetVolume() const;

	/*
	* 向容器中添加内容
	* @name: 内容名称
	* @content: 内容资产
	* @return: 是否超容量，超容量的话拒绝添加
	*/
	bool AddContent(const std::string& name, Asset* content);

	/*
	* 按名称删除内容
	* @name: 内容名称
	*/
	void RemoveContent(const std::string& name);

	/*
	* 析构容器内的全部直接子资产并清空容器
	*/
	void RemoveContents();

	/*
	* 获取容器剩余空间
	* @return: 容器剩余空间
	*/
	float GetSpace() const;

	/*
	* 获取容器当前所有内容
	* @return: 当前所有内容
	*/
	const std::unordered_map<std::string, Asset*>& GetContents() const;

	/*
	* 是否可背在身上
	* @return: 是否可背在身上
	*/
	bool GetBackpack() const;

	/*
	* 获取资产图标路径
	* @return: 资产图标路径
	*/
	std::string GetIcon() const;

	/*
	* 获取资产模型路径
	* @return: 资产模型路径
	*/
	std::string GetMesh() const;

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

	// 资产移动性
	ASSET_MOBILITY mobility;

	// 资产重量
	float weight;

	// 资产体积
	float size;

	// 资产容积
	float volume;

	// 资产空间
	float space;

	// 容器内容
	OBJECT_HOLDER std::unordered_map<std::string, Asset*> contents;

	// 是否可背
	bool backpack;

	// 资产图标路径
	std::string icon;

	// 资产模型路径
	std::string mesh;
};

// 空资产
class EmptyAsset : public AssetMod {
public:
	/*
	* 构造空资产
	*/
	EmptyAsset();

	/*
	* 析构空资产
	*/
	virtual ~EmptyAsset();

	/*
	* 空资产静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 空资产动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 空资产实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* 定义资产属性
	*/
	virtual void DefineAsset() override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 名称缓存
	std::string name;
};
