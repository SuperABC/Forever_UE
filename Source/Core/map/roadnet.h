#pragma once

#include "map/roadnet_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModRoadnetsFunc)(RoadnetFactory* factory);

// 路网实体
class Roadnet {
public:
	// 禁止默认构造
	Roadnet() = delete;

	// 通过类型从工厂构造
	Roadnet(RoadnetFactory* factory, const std::string& roadnet);
	
	// 析构地形
	~Roadnet();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 构建路网
	void DistributeRoadnet(int width, int height,
		std::function<std::string(int, int)> get);

	// 获取所有节点
	const std::vector<Node*>& GetNodes() const;

	// 获取所有连接
	const std::vector<Connection*>& GetConnections() const;

	// 获取所有地块
	const std::vector<Block*>& GetBlocks() const;

	// 分配地块地址
	void AllocateAddress();

	// 按地址查找
	Block* LocateBlock(const std::string& road, int id) const;

	// 自动寻路
	std::vector<std::pair<Connection*, std::pair<float, float>>> AutoNavigate(
		const std::vector<std::pair<Connection*, float>>& startRoads, const std::vector<std::pair<Connection*, float>>& endRoads) const;

	// 自动寻路
	std::vector<std::pair<Connection*, std::pair<float, float>>> AutoNavigate(
		Block* startBlock, Block* endBlock) const;

	// 自动寻路
	std::vector<std::pair<Connection*, std::pair<float, float>>> AutoNavigate(
		const std::vector<std::pair<Connection*, float>>& startRoads, Block* endBlock) const;

	// 自动寻路
	std::vector<std::pair<Connection*, std::pair<float, float>>> AutoNavigate(
		Block* startBlock, const std::vector<std::pair<Connection*, float>>& endRoads) const;

private:
	// 模组对象
	OBJECT_HOLDER RoadnetMod* mod;

	// 工厂
	RoadnetFactory* factory;

	// 路网类型
	std::string type;

	// 路网名称
	std::string name;

	// 节点
	OBJECT_HOLDER std::vector<Node*> nodes;

	// 道路
	OBJECT_HOLDER std::vector<Connection*> connections;

	// 街区
	OBJECT_HOLDER std::vector<Block*> blocks;

	// 地址簿
	std::unordered_map<std::string, std::vector<Block*>> addresses;
};

// 空路网
class EmptyRoadnet : public RoadnetMod {
public:
	EmptyRoadnet();
	virtual ~EmptyRoadnet();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void DistributeRoadnet(int width, int height,
		std::function<std::string(int, int)> get) override;

private:
	static int count;

	int id;
	std::string name;
};

