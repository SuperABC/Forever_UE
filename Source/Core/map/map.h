#pragma once

#include "../populace/person.h"
#include "../story/story.h"
#include "../traffic/traffic.h"

#include "terrain.h"
#include "roadnet.h"
#include "zone.h"
#include "building.h"
#include "component.h"
#include "room.h"

#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>

#define BLOCK_SIZE 256


class Person;
class Traffic;

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

class Block {
public:
    Block(int x, int y);
    ~Block();

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

    // 设置资源路径
    void SetResourcePath(const std::string& path);

    // 读取 Mods
    void InitTerrains(std::unordered_map<std::string, HMODULE>& modHandles);
    void InitRoadnets(std::unordered_map<std::string, HMODULE>& modHandles);
    void InitZones(std::unordered_map<std::string, HMODULE>& modHandles);
    void InitBuildings(std::unordered_map<std::string, HMODULE>& modHandles);
    void InitComponents(std::unordered_map<std::string, HMODULE>& modHandles);
    void InitRooms(std::unordered_map<std::string, HMODULE>& modHandles);

    // 读取配置文件
    void ReadConfigs(const std::string& path) const;

    // 初始化全部地图
    int Init(int blockX, int blockY, Traffic* traffic);

    // 市民入驻
    void Checkin(std::vector<Person*> citizens, Time* time, AssetFactory* factory) const;

    // 释放空间
    void Destroy();

    // 时钟前进
    void Tick(int day, int hour, int min, int sec);

    // 输出当前地图
    void Print() const;

    // 保存/加载地图
    void Load(const std::string& path);
    void Save(const std::string& path) const;

    // 应用变更
    void ApplyChange(Change* change, Story* story,
        std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

    // 获取地图尺寸
    std::pair<int, int> GetSize() const;

    // 获取玩家坐标
    std::pair<float, float> GetPlayerPos() const;

    // 检查全局坐标是否在地图内
    bool CheckXY(int x, int y) const;

    // 获取地块
    std::shared_ptr<Block> GetBlock(int x, int y) const;

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
    Zone* GetZone(const std::string& name);
    Building* GetBuilding(const std::string& name);
    void SetZone(Zone* zone, const std::string& name);
    void SetBuilding(Building* building, const std::string& name);
    void SetBuilding(Building* building, const std::string& name, std::pair<float, float> offset);

    // 寻址
    Plot* LocatePlot(const std::string& address) const;
    Zone* LocateZone(const std::string& address) const;
    Building* LocateBuilding(const std::string& address) const;
    Room* LocateRoom(const std::string& address) const;

private:
    void ArrangePlots();

    std::string resourcePath;

    static TerrainFactory* terrainFactory;
    static RoadnetFactory* roadnetFactory;
    static ZoneFactory* zoneFactory;
    static BuildingFactory* buildingFactory;
    static ComponentFactory* componentFactory;
    static RoomFactory* roomFactory;

    int width;
    int height;
    std::vector<std::vector<std::shared_ptr<Block>>> blocks;
    std::pair<float, float> playerPos;

    Roadnet* roadnet;
    std::unordered_map<std::string, Zone*> zones;
    std::unordered_map<std::string, Building*> buildings;
    Layout* layout;
};
