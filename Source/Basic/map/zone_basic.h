#pragma once

#include "map/zone_mod.h"


// 住宅园区
class ResidentialZone : public ZoneMod {
public:
	/*
	* 构造住宅园区
	*/
	ResidentialZone();

	/*
	* 析构住宅园区
	*/
	virtual ~ResidentialZone();

	/*
	* Override
	* 园区静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 园区动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 园区实例唯一名称
	*/
	virtual const char* GetName() override;

	// 按地块计算应生成的园区数量
	static std::function<int(const Lot*)> ZoneAssigner;

	/*
	* Override
	* 设计园区
	* @lot: 所在地块
	*/
	virtual void LayoutZone(const Lot* lot) override;

	/*
	* Override
	* 放置寻址锚点
	* @zone: 园区所占据的矩形区域
	*/
	virtual void PlacePivots(Quad* zone) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};
