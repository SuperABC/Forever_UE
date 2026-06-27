#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#define CHUNK_SIZE 64


// 10m*10m地图元素，用于地图可视化
class Element {
public:
	// 构造地图元素
	Element();

	// 析构地图元素
	~Element();

	// 获取地图元素地形名称
	std::string GetTerrain() const;

	// 获取地图元素水面信息
	std::pair<bool, float> GetWater() const;

	// 设置地图元素地形名称
	bool SetTerrain(const std::string& terrain, const std::pair<bool, float> water);

	// 获取地图元素地形高度
	float GetHeight() const;

	// 设置地图元素地形高度
	bool SetHeight(float height);

	// 获取地图元素所在园区名称
	std::string GetZone() const;

	// 设置地图元素所在园区名称
	bool SetZone(const std::string& zone);

	// 获取地图元素所在建筑名称
	std::string GetBuilding() const;

	// 设置地图元素所在建筑名称
	bool SetBuilding(const std::string& building);

	// 获取地图元素开洞信息
	std::vector<std::pair<Quad, float>> GetHatches() const;

	// 添加地图元素开洞
	void AddHatch(Quad q, float rotation);

private:
	// 地图元素所在地形类型
	std::string terrain;

	// 地图元素所在地形高度
	float height;

	// 地图元素是否添加水面
	std::pair<bool, float> water;

	// 地图元素所在园区名称
	std::string zone;

	// 地图元素所在建筑名称
	std::string building;

	// 开洞
	std::vector<std::pair<Quad, float>> hatches;
};

// CHUNK_SIZE*CHUNK_SIZE地图区块
class Chunk {
public:
	// 禁止空构造
	Chunk() = delete;

	// 按照地图区块排列位置构造区块
	Chunk(int x, int y);

	// 析构地图区块
	~Chunk();

	// 获取地图区块内某一元素地形名称
	std::string GetTerrain(int x, int y) const;

	// 获取地图区块内某一元素水面信息
	std::pair<bool, float> GetWater(int x, int y) const;

	// 设置地图区块内某一元素地形名称
	bool SetTerrain(int x, int y, const std::string& terrain, const std::pair<bool, float> water);

	// 获取地图区块内某一元素地形高度
	float GetHeight(int x, int y) const;

	// 设置地图区块内某一元素地形高度
	bool SetHeight(int x, int y, float height);

	// 获取地图区块内某一元素所在园区名称
	std::string GetZone(int x, int y) const;

	// 设置地图区块内某一元素所在园区名称
	bool SetZone(int x, int y, const std::string& zone);

	// 获取地图区块内某一元素所在建筑名称
	std::string GetBuilding(int x, int y) const;

	// 设置地图区块内某一元素所在建筑名称
	bool SetBuilding(int x, int y, const std::string& zone);

	// 获取地图区块内某一元素开洞信息
	std::vector<std::pair<Quad, float>> GetHatches(int x, int y) const;

	// 添加地图区块内某一元素开洞
	bool AddHatch(int x, int y, Quad q, float rotation);

private:
	// 检查全局坐标是否在地块内
	bool CheckXY(int x, int y) const;

	// 地块排列位置
	int offsetX, offsetY;

	// 地块内元素矩阵
	OBJECT_HOLDER std::vector<std::vector<Element*>> elements;
};

class Map {
public:
	// 构造地图
	Map();

	// 析构地图
	~Map();

	// 读取配置文件
	void LoadConfigs() const;
	
	// 读取地形模组
	void InitTerrains(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 读取路网模组
	void InitRoadnets(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 读取园区模组
	void InitZones(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 读取建筑模组
	void InitBuildings(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 读取组合模组
	void InitComponents(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 读取房间模组
	void InitRooms(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 初始化地图区块
	void InitBlocks(int chunkX, int chunkY);

	// 初始化内容
	int InitContents();

	// 市民入驻
	void Checkin(Populace* populace, Player* player) const;

	// 释放空间
	void Destroy();

	// 时钟周期
	void Tick(Player* player);

	// 应用剧情变化
	void ApplyChange(Change* change,
		std::vector < std::function<std::pair<bool, ValueType>(const std::string&)>> getValues);

	// 获取地图尺寸
	std::pair<int, int> GetSize() const;

	// 获取某一元素水面信息
	std::pair<bool, float> GetWater(int x, int y) const;

	// 获取某一元素地形名称
	std::string GetTerrain(int x, int y) const;

	// 设置某一元素地形名称
	bool SetTerrain(int x, int y, const std::string& terrain, const std::pair<bool, float> water = { false, 0.f });

	// 获取某一元素地形高度
	float GetHeight(int x, int y) const;

	// 设置某一元素地形高度
	bool SetHeight(int x, int y, float height);

	// 计算精确位置地形
	const std::unordered_map<std::string, std::pair<int, std::string>>& GetTerrainTextures() const;

	// 获取某一元素所在园区名称
	std::string GetZone(int x, int y) const;

	// 设置某一元素所在园区名称
	bool SetZone(int x, int y, const std::string& zone);

	// 设置园区内所有元素所属园区
	void SetZone(Zone* zone, const std::string& name);

	// 获取某一元素所在建筑名称
	std::string GetBuilding(int x, int y) const;

	// 设置某一元素所在建筑名称
	bool SetBuilding(int x, int y, const std::string& zone);

	// 获取某一元素开洞信息
	std::vector<std::pair<Quad, float>> GetHatches(int x, int y) const;

	// 添加开洞（自动分发到所有重叠element）
	void AddHatch(Quad q, float rotation);

	// 设置建筑内所有元素所属建筑
	void SetBuilding(Building* building, const std::string& name, const std::pair<float, float>& offset = { 0.f, 0.f });

	// 获取玩家坐标
	std::pair<float, float> GetPlayerPos() const;

	// 获取路网
	Roadnet* GetRoadnet() const;

	// 获取所有园区
	const std::unordered_map<std::string, Zone*>& GetZones() const;

	// 根据名称查找园区
	Zone* GetZone(const std::string& name) const;

	// 添加园区
	void AddZone(Zone* zone);

	// 获取所有建筑
	const std::unordered_map<std::string, Building*>& GetBuildings() const;

	// 根据名称查找建筑
	Building* GetBuilding(const std::string& name) const;

	// 添加建筑
	void AddBuilding(Building* building);

	// 获取所有组合
	std::vector<Component*> GetComponents() const;

	// 获取所有房间
	std::vector<Room*> GetRooms() const;

	// 寻址地块
	Block* LocateBlock(const std::string& address) const;

	// 寻址园区
	Zone* LocateZone(const std::string& address) const;

	// 寻址建筑
	Building* LocateBuilding(const std::string& address) const;

	// 寻址房间
	Room* LocateRoom(const std::string& address) const;

	// 自动寻路
	std::vector<Connection*> AutoNavigation(int startIdx, int endIdx);

	// 添加一个与已有节点existingId相连的新节点node，接入寻路图；返回新建的连接
	Connection* AddNode(Node* node, int existingId);

	/*
	* 批量应用一次DivideSpace产生的导航节点/连接增量到导航图
	* @newNodes, newConnections: 新增的导航节点与连接
	* @removedConnections: 因被切分而失效、需要从导航图移除并释放的旧连接
	*/
	void ApplyDivideResult(const std::vector<Node*>& newNodes,
		const std::vector<Connection*>& newConnections, const std::vector<Connection*>& removedConnections);

	// 地形工厂
	static TerrainFactory* terrainFactory;

	// 路网工厂
	static RoadnetFactory* roadnetFactory;

	// 园区工厂
	static ZoneFactory* zoneFactory;

	// 建筑工厂
	static BuildingFactory* buildingFactory;

	// 组合工厂
	static ComponentFactory* componentFactory;

	// 房间工厂
	static RoomFactory* roomFactory;

private:
	// 检查全局坐标是否在地图内
	bool CheckXY(int x, int y) const;

	// 分配所有地块内布局
	void ArrangeBlocks();

	// 清理空园区和空建筑
	void ClearZero();

	// 根据当前路网和地块连接情况初始化寻路图
	void InitNavigationGraph();

	// 地图尺寸
	int width, height;

	// 地图区块矩阵
	OBJECT_HOLDER std::vector<std::vector<Chunk*>> chunks;

	// 玩家坐标
	std::pair<float, float> playerPos;

	// 地形纹理映射
	std::unordered_map<std::string, std::pair<int, std::string>> terrainTextures;

	// 路网
	OBJECT_HOLDER Roadnet* roadnet;

	// 寻路图节点（id->节点，不持有所有权）
	std::unordered_map<int, Node*> navigationNodes;

	// 寻路图邻接表（节点id->(邻居id, 连接)）
	std::unordered_map<int, std::vector<std::pair<int, Connection*>>> navigationGraph;

	// 寻路图中由地图合成的连接（用于将地块接入路网，需要释放）
	OBJECT_HOLDER std::vector<Connection*> navigationConnections;

	// 所有园区（名称->园区）
	std::unordered_map<std::string, Zone*> zones;

	// 建筑（名称->建筑）
	std::unordered_map<std::string, Building*> buildings;

	// 预设布局
	OBJECT_HOLDER Layout* layout;
};
