#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#define CHUNK_SIZE 256


// 10m*10m地图元素，用于地图可视化
class Element {
public:
	// 构造地图元素
	Element();

	// 析构地图元素
	~Element();

	// 获取地图元素地形名称
	std::string GetTerrain() const;

	// 设置地图元素地形名称
	bool SetTerrain(const std::string& terrain);

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

private:
	// 地图元素所在地形类型
	std::string terrain;

	// 地图元素所在地形高度
	float height;

	// 地图元素所在园区名称
	std::string zone;

	// 地图元素所在建筑名称
	std::string building;
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

	// 设置地图区块内某一元素地形名称
	bool SetTerrain(int x, int y, const std::string& terrain);

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

	// 初始化地图
	int Init(int chunkX, int chunkY);

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

	// 获取某一元素地形名称
	std::string GetTerrain(int x, int y) const;

	// 设置某一元素地形名称
	bool SetTerrain(int x, int y, const std::string& terrain);

	// 获取某一元素地形高度
	float GetHeight(int x, int y) const;

	// 设置某一元素地形高度
	bool SetHeight(int x, int y, float height);

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

	// 获取所有建筑
	const std::unordered_map<std::string, Building*>& GetBuildings() const;

	// 根据名称查找建筑
	Building* GetBuilding(const std::string& name) const;

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

	// 地图尺寸
	int width, height;

	// 地图区块矩阵
	OBJECT_HOLDER std::vector<std::vector<Chunk*>> chunks;

	// 玩家坐标
	std::pair<float, float> playerPos;

	// 路网
	OBJECT_HOLDER Roadnet* roadnet;

	// 所有园区（名称->园区）
	std::unordered_map<std::string, Zone*> zones;

	// 建筑（名称->建筑）
	std::unordered_map<std::string, Building*> buildings;

	// 预设布局
	OBJECT_HOLDER Layout* layout;
};
