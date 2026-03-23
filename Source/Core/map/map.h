#pragma once

#include "terrain.h"
#include "roadnet.h"
#include "zone.h"
#include "building.h"
#include "component.h"
#include "room.h"

#define CHUNK_SIZE 256


class Element {
public:
	Element();
	~Element();

	// 获取/设置地形名称
	std::string GetTerrain() const;
	bool SetTerrain(const std::string& terrain, float height);

	// 获取/设置高度
	float GetHeight() const;
	bool SetHeight(float height);

	// 获取/设置园区/建筑标识
	std::string GetZone() const;
	bool SetZone(const std::string& zone);
	std::string GetBuilding() const;
	bool SetBuilding(const std::string& building);

private:
	std::string terrain;
	float height;
	std::string zone;
	std::string building;
};

class Chunk {
public:
	Chunk(int x, int y);
	~Chunk();

	// 获取/设置地形
	std::string GetTerrain(int x, int y) const;
	bool SetTerrain(int x, int y, const std::string& terrain, float height);
	float GetHeight(int x, int y) const;

	// 检查全局坐标是否在地块内
	bool CheckXY(int x, int y) const;

	// 获取地块内某全局坐标的信息
	std::shared_ptr<Element> GetElement(int x, int y) const;

private:
	int offsetX, offsetY;
	std::vector<std::vector<std::shared_ptr<Element>>> elements;
};

class Map {
public:
	Map();
	~Map();

	// 读取配置文件
	void LoadConfigs() const;
	
	// 读取 Mods
	void InitTerrains(std::unordered_map<std::string, HMODULE>& modHandles,
		std::vector<std::string>& dlls);
	void InitRoadnets(std::unordered_map<std::string, HMODULE>& modHandles,
		std::vector<std::string>& dlls);
	void InitZones(std::unordered_map<std::string, HMODULE>& modHandles,
		std::vector<std::string>& dlls);
	void InitBuildings(std::unordered_map<std::string, HMODULE>& modHandles,
		std::vector<std::string>& dlls);
	void InitComponents(std::unordered_map<std::string, HMODULE>& modHandles,
		std::vector<std::string>& dlls);
	void InitRooms(std::unordered_map<std::string, HMODULE>& modHandles,
		std::vector<std::string>& dlls);

	// 初始化地图
	int Init(int chunkX, int chunkY);

	// 释放空间
	void Destroy();

	// 获取地图尺寸
	std::pair<int, int> GetSize() const;

	// 获取玩家坐标
	std::pair<float, float> GetPlayerPos() const;

	// 检查全局坐标是否在地图内
	bool CheckXY(int x, int y) const;

	// 获取地块
	std::shared_ptr<Chunk> GetChunk(int x, int y) const;

	// 获取元素
	std::shared_ptr<Element> GetElement(int x, int y) const;

	// 获取/设置地形
	std::string GetTerrain(int x, int y) const;
	bool SetTerrain(int x, int y, const std::string& terrain, float height);
	float GetHeight(int x, int y) const;

	// 获取路网
	Roadnet* GetRoadnet() const;

	// 获取园区/建筑
	std::unordered_map<std::string, Zone*>& GetZones();
	std::unordered_map<std::string, Building*>& GetBuildings();

	// 获取组合/房间
	std::vector<Component*> GetComponents() const;
	std::vector<Room*> GetRooms() const;

	// 获取/设置元素所属园区/建筑
	Zone* GetZone(const std::string& name) const;
	Building* GetBuilding(const std::string& name) const;
	void SetZone(Zone* zone, const std::string& name);
	void SetBuilding(Building* building, const std::string& name);
	void SetBuilding(Building* building, const std::string& name, std::pair<float, float> offset);

	// 寻址
	Block* LocateBlock(const std::string& address) const;
	Zone* LocateZone(const std::string& address) const;
	Building* LocateBuilding(const std::string& address) const;
	Room* LocateRoom(const std::string& address) const;

private:
	void ArrangeBlocks();

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
	std::vector<std::vector<std::shared_ptr<Chunk>>> chunks;

	// 玩家信息
	std::pair<float, float> playerPos;

	// 路网
	Roadnet* roadnet;

	// 园区
	std::unordered_map<std::string, Zone*> zones;

	// 建筑
	std::unordered_map<std::string, Building*> buildings;

	// 预设布局
	Layout* layout;
};
