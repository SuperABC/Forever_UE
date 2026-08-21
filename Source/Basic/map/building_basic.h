#pragma once

#include "map/building_mod.h"


// 住宅建筑
class ResidentialBuilding : public BuildingMod {
public:
	/*
	* 构造住宅建筑
	*/
	ResidentialBuilding();

	/*
	* 析构住宅建筑
	*/
	virtual ~ResidentialBuilding();

	/*
	* Override
	* 建筑静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 建筑动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 建筑实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 建筑在各功能分区的生成权重
	*/
	static std::vector<float> GetPowers();

	// 按地块计算应生成的建筑数量
	static std::function<int(const Lot*, int, int)> BuildingAssigner;

	/*
	* Override
	* 采样建筑面积
	*/
	virtual float RandomAcreage() override;

	/*
	* Override
	* 放置楼栋
	*/
	virtual void PlaceConstruction(const Quad* quad) override;

	/*
	* Override
	* 布局建筑
	* @quad: 建筑所占据的矩形区域
	*/
	virtual void LayoutBuilding(const Quad* quad) override;

	/*
	* Override
	* 放置寻址锚点
	* @building: 建筑所占据的矩形区域
	*/
	virtual void PlacePivots(Quad* building) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

// 低密度住宅楼
class ResidentialLowBuilding : public ResidentialBuilding {
public:
	/*
	* Override
	* 建筑静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 建筑动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 建筑在各功能分区的生成权重
	*/
	static std::vector<float> GetPowers();

	/*
	* Override
	* 采样建筑面积
	*/
	virtual float RandomAcreage() override;
};

// 中密度住宅楼
class ResidentialMiddleBuilding : public ResidentialBuilding {
public:
	/*
	* Override
	* 建筑静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 建筑动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 建筑在各功能分区的生成权重
	*/
	static std::vector<float> GetPowers();

	/*
	* Override
	* 采样建筑面积
	*/
	virtual float RandomAcreage() override;
};

// 高密度住宅楼
class ResidentialHighBuilding : public ResidentialBuilding {
public:
	/*
	* Override
	* 建筑静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 建筑动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 建筑在各功能分区的生成权重
	*/
	static std::vector<float> GetPowers();

	/*
	* Override
	* 采样建筑面积
	*/
	virtual float RandomAcreage() override;
};

// 商店建筑
class ShopBuilding : public BuildingMod {
public:
	/*
	* 构造商店建筑
	*/
	ShopBuilding();

	/*
	* 析构商店建筑
	*/
	virtual ~ShopBuilding();

	/*
	* Override
	* 建筑静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 建筑动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 建筑实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 建筑在各功能分区的生成权重
	*/
	static std::vector<float> GetPowers();

	// 按地块计算应生成的建筑数量
	static std::function<int(const Lot*, int, int)> BuildingAssigner;

	/*
	* Override
	* 采样建筑面积
	*/
	virtual float RandomAcreage() override;

	/*
	* Override
	* 放置楼栋
	*/
	virtual void PlaceConstruction(const Quad* quad) override;

	/*
	* Override
	* 布局建筑
	* @quad: 建筑所占据的矩形区域
	*/
	virtual void LayoutBuilding(const Quad* quad) override;

	/*
	* Override
	* 放置寻址锚点
	* @building: 建筑所占据的矩形区域
	*/
	virtual void PlacePivots(Quad* building) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;

	// 建筑朝向
	int direction;
};

// 工厂建筑
class FactoryBuilding : public BuildingMod {
public:
	/*
	* 构造工厂建筑
	*/
	FactoryBuilding();

	/*
	* 析构工厂建筑
	*/
	virtual ~FactoryBuilding();

	/*
	* Override
	* 建筑静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 建筑动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 建筑实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 建筑在各功能分区的生成权重
	*/
	static std::vector<float> GetPowers();

	// 按地块计算应生成的建筑数量
	static std::function<int(const Lot*, int, int)> BuildingAssigner;

	/*
	* Override
	* 采样建筑面积
	*/
	virtual float RandomAcreage() override;

	/*
	* Override
	* 放置楼栋
	*/
	virtual void PlaceConstruction(const Quad* quad) override;

	/*
	* Override
	* 布局建筑
	* @quad: 建筑所占据的矩形区域
	*/
	virtual void LayoutBuilding(const Quad* quad) override;

	/*
	* Override
	* 放置寻址锚点
	* @building: 建筑所占据的矩形区域
	*/
	virtual void PlacePivots(Quad* building) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

// 医院建筑
class HospitalBuilding : public BuildingMod {
public:
	/*
	* 构造医院建筑
	*/
	HospitalBuilding();

	/*
	* 析构医院建筑
	*/
	virtual ~HospitalBuilding();

	/*
	* Override
	* 建筑静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 建筑动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 建筑实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 建筑在各功能分区的生成权重
	*/
	static std::vector<float> GetPowers();

	// 按地块计算应生成的建筑数量
	static std::function<int(const Lot*, int, int)> BuildingAssigner;

	/*
	* Override
	* 采样建筑面积
	*/
	virtual float RandomAcreage() override;

	/*
	* Override
	* 放置楼栋
	*/
	virtual void PlaceConstruction(const Quad* quad) override;

	/*
	* Override
	* 布局建筑
	* @quad: 建筑所占据的矩形区域
	*/
	virtual void LayoutBuilding(const Quad* quad) override;

	/*
	* Override
	* 放置寻址锚点
	* @building: 建筑所占据的矩形区域
	*/
	virtual void PlacePivots(Quad* building) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;

	// 建筑朝向
	int direction;
};

// 办公建筑
class OfficeBuilding : public BuildingMod {
public:
	/*
	* 构造办公建筑
	*/
	OfficeBuilding();

	/*
	* 析构办公建筑
	*/
	virtual ~OfficeBuilding();

	/*
	* Override
	* 建筑静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 建筑动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 建筑实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 建筑在各功能分区的生成权重
	*/
	static std::vector<float> GetPowers();

	// 按地块计算应生成的建筑数量
	static std::function<int(const Lot*, int, int)> BuildingAssigner;

	/*
	* Override
	* 采样建筑面积
	*/
	virtual float RandomAcreage() override;

	/*
	* Override
	* 放置楼栋
	*/
	virtual void PlaceConstruction(const Quad* quad) override;

	/*
	* Override
	* 布局建筑
	* @quad: 建筑所占据的矩形区域
	*/
	virtual void LayoutBuilding(const Quad* quad) override;

	/*
	* Override
	* 放置寻址锚点
	* @building: 建筑所占据的矩形区域
	*/
	virtual void PlacePivots(Quad* building) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

// 酒店建筑
class HotelBuilding : public BuildingMod {
public:
	/*
	* 构造酒店建筑
	*/
	HotelBuilding();

	/*
	* 析构酒店建筑
	*/
	virtual ~HotelBuilding();

	/*
	* Override
	* 建筑静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 建筑动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 建筑实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 建筑在各功能分区的生成权重
	*/
	static std::vector<float> GetPowers();

	// 按地块计算应生成的建筑数量
	static std::function<int(const Lot*, int, int)> BuildingAssigner;

	/*
	* Override
	* 采样建筑面积
	*/
	virtual float RandomAcreage() override;

	/*
	* Override
	* 放置楼栋
	*/
	virtual void PlaceConstruction(const Quad* quad) override;

	/*
	* Override
	* 布局建筑
	* @quad: 建筑所占据的矩形区域
	*/
	virtual void LayoutBuilding(const Quad* quad) override;

	/*
	* Override
	* 放置寻址锚点
	* @building: 建筑所占据的矩形区域
	*/
	virtual void PlacePivots(Quad* building) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;

	// 建筑朝向
	int direction;
};
