#pragma once

#include "roadnet_mod.h"

#include "block.h"


// 子类注册函数
typedef void (*RegisterModRoadnetsFunc)(RoadnetFactory* factory);

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

// 路网实体
class Roadnet {
public:
	Roadnet() = delete;
	Roadnet(RoadnetFactory* factory, std::string roadnet);
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
	Block* LocateBlock(std::string road, int id) const;

	// 自动寻路
	//const std::vector<Connection> AutoNavigate(
	//	const std::vector<std::pair<Connection, float>>& startRoads, const std::vector<std::pair<Connection, float>>& endRoads) const;
	//const std::vector<Connection> AutoNavigate(
	//	const Block* startBlock, const Block* endBlock) const;
	//const std::vector<Connection> AutoNavigate(
	//	const std::vector<std::pair<Connection, float>>& startRoads, const Block* endBlock) const;
	//const std::vector<Connection> AutoNavigate(
	//	const Block* startBlock, const std::vector<std::pair<Connection, float>>& endRoads) const;

private:
	RoadnetMod* mod;
	RoadnetFactory* factory;

	std::string type;
	std::string name;

	VARIABLE_HOLDER std::vector<Node*> nodes;
	VARIABLE_HOLDER std::vector<Connection*> connections;
	VARIABLE_HOLDER std::vector<Block*> blocks;
	//VARIABLE_HOLDER std::vector<Route*> routes;
	std::unordered_map<std::string, std::vector<Block*>> addresses;
};

