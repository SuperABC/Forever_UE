#pragma once

#include "player/asset_mod.h"


// 园区资产
class ZoneAsset : public AssetMod {
public:
	/*
	* 构造园区资产
	*/
	ZoneAsset();

	/*
	* 析构园区资产
	*/
	virtual ~ZoneAsset();

	/*
	* 园区资产静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 园区资产动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 园区资产实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* 定义资产属性
	*/
	virtual void DefineAsset();

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 名称缓存
	std::string name;
};

// 建筑资产
class BuildingAsset : public AssetMod {
public:
	/*
	* 构造建筑资产
	*/
	BuildingAsset();

	/*
	* 析构建筑资产
	*/
	virtual ~BuildingAsset();

	/*
	* 建筑资产静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 建筑资产动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 建筑资产实例唯一名称
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

// 房间资产
class RoomAsset : public AssetMod {
public:
	/*
	* 构造房间资产
	*/
	RoomAsset();

	/*
	* 析构房间资产
	*/
	virtual ~RoomAsset();

	/*
	* 房间资产静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 房间资产动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 房间资产实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* 定义资产属性
	*/
	virtual void DefineAsset();

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 名称缓存
	std::string name;
};
