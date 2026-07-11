#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


enum ASSET_MOBILITY {
	ASSET_ESTATE,
	ASSET_VEHICLE,
	ASSET_OBJECT,
	ASSET_CONTAINEE
};

class AssetMod {
public:
	/*
	* 构造资产模组
	*/
	AssetMod();

	/*
	* 析构资产模组
	*/
	virtual ~AssetMod();

	/*
	* 统一类型标识符
	*/
	static const char* GetId();

	/*
	* 获取资产类型
	*/
	virtual const char* GetType() const = 0;

	/*
	* 获取资产名称
	*/
	virtual const char* GetName() = 0;

	/*
	* 填写资产属性
	*/
	COSTOM_INIT virtual void DefineAsset() = 0;

	/*
	* 定义为不动产
	*/
	void DefineEstate();

	/*
	* 定义为载具
	*/
	void DefineVehicle();

	/*
	* 定义为普通物体（非容器）
	* @weight, size: 重量与体积
	* @backpack: 是否可背
	*/
	void DefineObject(float weight, float size, bool backpack = false);

	/*
	* 定义为容器物体
	* @weight, size: 自身重量与体积
	* @volume: 容积
	* @backpack: 是否可背
	*/
	void DefineContainer(float weight, float size, float volume, bool backpack = false);

	/*
	* 定义为容器内容物
	* @weight, size: 重量与占用容积
	*/
	void DefineContainee(float weight, float size);

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

	// 是否可背
	bool backpack;
};

class AssetFactory {
public:
	/*
	* 注册资产类型
	* @id: 类型标识符
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterAsset(const std::string& id,
		std::function<AssetMod* ()> creator, std::function<void(AssetMod*)> deleter);

	/*
	* 清空所有类型的启用配置
	*/
	void RemoveAll();

	/*
	* 创建指定类型的资产模组
	* @id: 类型标识符
	*/
	AssetMod* CreateAsset(const std::string& id) const;

	/*
	* 检查指定类型是否已注册
	* @id: 类型标识符
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置类型的启用配置
	* @name: 类型标识符
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 析构资产模组对象
	* @assetMod: 待析构的模组对象
	*/
	void DestroyAsset(AssetMod* assetMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<AssetMod* ()>, std::function<void(AssetMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};
