#pragma once

#include "terrain.h"
#include "roadnet.h"
#include "zone.h"
#include "building.h"
#include "component.h"
#include "room.h"

#define CHUNK_SIZE 256


// 10m*10m地图元素，用于地图可视化
class Element {
public:
	// 构造元素
	Element();

	// 析构元素
	~Element();

	// 获取元素地形名称
	std::string GetTerrain() const;

	// 设置元素地形名称
	bool SetTerrain(const std::string& terrain);

	// 获取元素高度
	float GetHeight() const;

	// 设置元素高度
	bool SetHeight(float height);

	// 获取园区
	std::string GetZone() const;

	// 设置园区
	bool SetZone(const std::string& zone);

	// 获取建筑
	std::string GetBuilding() const;

	// 设置建筑
	bool SetBuilding(const std::string& building);

private:
	// 所在地形类型
	std::string terrain;

	// 所在地形高度
	float height;

	// 所在园区名称
	std::string zone;

	// 所在建筑名称
	std::string building;
};

// CHUNK_SIZE*CHUNK_SIZE地图区块
class Chunk {
public:
	// 禁止空构造
	Chunk() = delete;

	// 按照排列位置构造区块
	Chunk(int x, int y);

	// 析构区块
	~Chunk();

	// 获取某一元素地形类型
	std::string GetTerrain(int x, int y) const;

	// 设置某一元素地形类型
	bool SetTerrain(int x, int y, const std::string& terrain);

	// 获取某一元素地形高度
	float GetHeight(int x, int y) const;

	// 设置某一元素地形高度
	bool SetHeight(int x, int y, float height);

	// 获取某一元素所在园区名称
	std::string GetZone(int x, int y) const;

	// 设置某一元素所在园区名称
	bool SetZone(int x, int y, const std::string& zone);

	// 获取某一元素所在建筑名称
	std::string GetBuilding(int x, int y) const;

	// 设置某一元素所在建筑名称
	bool SetBuilding(int x, int y, const std::string& zone);

private:
	// 检查全局坐标是否在地块内
	bool CheckXY(int x, int y) const;

	// 地块排列位置
	int offsetX, offsetY;

	// 地块内元素
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
	
	// 读取 Mods
	void InitTerrains(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);
	void InitRoadnets(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);
	void InitZones(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);
	void InitBuildings(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);
	void InitComponents(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);
	void InitRooms(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 初始化地图
	int Init(int chunkX, int chunkY);

	// 释放空间
	void Destroy();

	// 获取地图尺寸
	std::pair<int, int> GetSize() const;

	// 获取某一元素地形类型
	std::string GetTerrain(int x, int y) const;

	// 设置某一元素地形类型
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
	const Zone* GetZone(const std::string& name) const;

	// 获取所有建筑
	const std::unordered_map<std::string, Building*>& GetBuildings() const;

	// 根据名称查找建筑
	const Building* GetBuilding(const std::string& name) const;

	// 获取所有组合
	std::vector<Component*> GetComponents() const;

	// 获取所有房间
	std::vector<Room*> GetRooms() const;

	// 寻址地块
	const Block* LocateBlock(const std::string& address) const;

	// 寻址园区
	const Zone* LocateZone(const std::string& address) const;

	// 寻址建筑
	const Building* LocateBuilding(const std::string& address) const;

	// 寻址房间
	const Room* LocateRoom(const std::string& address) const;

private:
	// 检查全局坐标是否在地图内
	bool CheckXY(int x, int y) const;

	// 分配地块内布局
	void ArrangeBlocks();

	// 清理空园区建筑
	void ClearZero();

	// 统一工厂
	static TerrainFactory* terrainFactory;
	static RoadnetFactory* roadnetFactory;
	static ZoneFactory* zoneFactory;
	static BuildingFactory* buildingFactory;
	static ComponentFactory* componentFactory;
	static RoomFactory* roomFactory;

	// 地图信息
	int width;
	int height;
	OBJECT_HOLDER std::vector<std::vector<Chunk*>> chunks;

	// 玩家坐标
	std::pair<float, float> playerPos;

	// 路网
	OBJECT_HOLDER Roadnet* roadnet;

	// 园区
	std::unordered_map<std::string, Zone*> zones;

	// 建筑
	std::unordered_map<std::string, Building*> buildings;

	// 预设布局
	OBJECT_HOLDER Layout* layout;
};
