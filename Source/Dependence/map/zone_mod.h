#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include "geometry.h"

#include <string>
#include <functional>


class ZoneMod {
public:
	/*
	* 无构造
	*/
	ZoneMod();

	/*
	* 无析构
	*/
	virtual ~ZoneMod();

	/*
	* Override
	* 园区静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 园区动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 园区实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 按地块计算应生成的园区数量
	* @block: 所在地块
	*/
	COSTOM_INIT static std::function<int(const Lot* block)> ZoneAssigner;

	/*
	* Override
	* 设计园区
	* @block: 所在地块
	*/
	COSTOM_INIT virtual void LayoutZone(const Lot* block) = 0;

	/*
	* Override
	* 放置寻址锚点
	* @zone: 园区所占据的矩形区域
	*/
	COSTOM_INIT virtual void PlacePivots(Quad* zone) = 0;

	// 园区面积
	float acreage;

	// 锚点
	std::vector<std::vector<float>> pivots;

	// 内部建筑
	std::vector<std::pair<std::string, float>> buildings;

	// 关联剧情与脚本
	std::pair<std::string, std::vector<std::string>> script;
};

class ZoneFactory {
public:
	/*
	* 注册园区及其数量生成器、创建与销毁方式
	* @id: 园区静态类型标识
	* @assigner: 按地块计算应生成的园区数量的方法
	* @creator, deleter: 创建/销毁园区实例的方法
	*/
	void RegisterZone(const std::string& id, std::function<int(const Lot*)> assigner,
		std::function<ZoneMod* ()> creator, std::function<void(ZoneMod*)> deleter);

	/*
	* 清空所有启用的园区配置
	*/
	void RemoveAll();

	/*
	* 按静态类型标识创建园区实例
	* @id: 园区静态类型标识
	*/
	ZoneMod* CreateZone(const std::string& id) const;

	/*
	* 检查园区是否已注册
	* @id: 园区静态类型标识
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置园区的启用配置
	* @name: 园区静态类型标识
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 获取所有已启用的园区静态类型标识
	*/
	std::vector<std::string> GetTypes() const;

	/*
	* 在地块内按数量生成器生成一类园区的多个实例
	* @type: 园区静态类型标识
	* @lot: 待生成园区的地块
	*/
	std::vector<ZoneMod*> CreateZones(const std::string& type, const Lot* lot) const;

	/*
	* 销毁园区实例
	* @zoneMod: 待销毁的园区实例
	*/
	void DestroyZone(ZoneMod* zoneMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<ZoneMod* ()>, std::function<void(ZoneMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;

	// 园区生成器
	std::unordered_map<std::string, std::function<int(const Lot*)>> assigners;
};

