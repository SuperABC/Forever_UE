#pragma once

#include "map/roadnet_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModRoadnetsFunc)(RoadnetFactory* factory);

// 子类注册收尾函数
typedef void (*FinishModRoadnetsFunc)(RoadnetFactory* factory);

// 路网实体
class Roadnet {
public:
	/*
	* 禁止默认构造
	*/
	Roadnet() = delete;

	/*
	* 通过路网静态类型标识从工厂构造路网
	* @factory: 路网工厂
	* @roadnet: 路网静态类型标识
	*/
	Roadnet(RoadnetFactory* factory, const std::string& roadnet);

	/*
	* 析构路网
	*/
	~Roadnet();

	/*
	* 获取路网类型
	*/
	std::string GetType() const;

	/*
	* 获取路网名称
	*/
	std::string GetName() const;

	/*
	* 构建路网
	* @width, height: 地图分辨率
	* @getTerrain: 获取地图element处地形类型
	* @getHeight: 获取地图element处地形高度
	* @getWater: 获取地图element处水域信息（是否水域、水面高度）
	* @addHatch: 将开洞信息保存进地图element
	*/
	void DistributeRoadnet(int width, int height,
		const std::function<std::string(int, int)>& getTerrain,
		const std::function<float(int, int)>& getHeight,
		const std::function<std::pair<bool, float>(int, int)>& getWater,
		const std::function<void(Quad, float)>& addHatch, int nodeStaticCount);

	/*
	* 获取所有外界连接点
	*/
	const std::vector<Node*>& GetExterns() const;

	/*
	* 获取所有节点
	*/
	const std::vector<Intersection*>& GetIntersections() const;

	/*
	* 获取所有连接
	*/
	const std::vector<Road*>& GetRoads() const;

	/*
	* 获取所有地块
	*/
	const std::vector<Block*>& GetBlocks() const;

	/*
	* 为所有地块按所在道路分配地址索引
	*/
	void AllocateAddress();

	/*
	* 按道路名称及地址索引查找地块
	* @road: 道路名称
	* @id: 地址索引
	*/
	Block* LocateBlock(const std::string& road, int id) const;

private:
	// 模组对象
	OBJECT_HOLDER RoadnetMod* mod;

	// 工厂
	RoadnetFactory* factory;

	// 路网类型
	std::string type;

	// 路网名称
	std::string name;

	// 外界连接点
	OBJECT_HOLDER std::vector<Node*> externs;

	// 节点
	OBJECT_HOLDER std::vector<Intersection*> intersections;

	// 道路
	OBJECT_HOLDER std::vector<Road*> roads;

	// 街区
	OBJECT_HOLDER std::vector<Block*> blocks;

	// 地址簿
	std::unordered_map<std::string, std::vector<Block*>> addresses;
};

// 空路网
class EmptyRoadnet : public RoadnetMod {
public:
	/*
	* 构造空路网
	*/
	EmptyRoadnet();

	/*
	* 析构空路网
	*/
	virtual ~EmptyRoadnet();

	/*
	* Override
	* 路网静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 路网动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 路网实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 构建路网
	* @width, height: 地图分辨率
	* @getTerrain: 获取地图element处地形类型
	* @getHeight: 获取地图element处地形高度
	* @getWater: 获取地图element处水域信息（是否水域、水面高度）
	*/
	virtual void DistributeRoadnet(int width, int height,
		const std::function<std::string(int, int)>& getTerrain,
		const std::function<float(int, int)>& getHeight,
		const std::function<std::pair<bool, float>(int, int)>& getWater, int nodeStaticCount) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

