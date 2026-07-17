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

// 资产Mod基类
class AssetMod {
public:
	/*
	* 构造资产
	*/
	AssetMod();

	/*
	* 析构资产
	*/
	virtual ~AssetMod();

	/*
	* Override
	* 资产静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 资产动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 资产实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 定义资产属性
	*/
	COSTOM_INIT virtual void DefineAsset() = 0;

	/*
	* Tool
	* 定义为不动产
	*/
	void DefineEstate();

	/*
	* Tool
	* 定义为载具
	*/
	void DefineVehicle();

	/*
	* Tool
	* 定义为普通物体
	* @weight, size: 重量与体积
	* @backpack: 是否可背
	*/
	void DefineObject(float weight, float size, bool backpack = false);

	/*
	* Tool
	* 定义为容器物体
	* @weight, size: 自身重量与体积
	* @volume: 容积
	* @backpack: 是否可背
	*/
	void DefineContainer(float weight, float size, float volume, bool backpack = false);

	/*
	* Tool
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

	// 是否可使用
	bool usable;

	// 资产图标路径
	std::string icon;

	// 资产模型路径
	std::string mesh;
};

class AssetFactory {
public:
	/*
	* 注册资产
	* @id: 资产类型
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterAsset(const std::string& id,
		std::function<AssetMod* ()> creator, std::function<void(AssetMod*)> deleter);

	/*
	* 清空所有注册
	*/
	void RemoveAll();

	/*
	* 创建资产
	* @id: 资产类型
	*/
	AssetMod* CreateAsset(const std::string& id) const;

	/*
	* 检查资产是否已注册
	* @id: 资产类型
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置资产启用配置
	* @name: 资产类型
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 析构资产
	* @assetMod: 待析构的资产
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
