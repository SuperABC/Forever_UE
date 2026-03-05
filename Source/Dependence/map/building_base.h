#pragma once

#include "../common/quad.h"
#include "../common/plot.h"

#include "room_base.h"
#include "component_base.h"

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>


enum AREA_TYPE : int;

class Plot;
class Zone;
class Component;
class Room;

class ComponentFactory;
class RoomFactory;

// 建筑方向
enum FACE_DIRECTION : int {
	FACE_WEST,
	FACE_EAST,
	FACE_NORTH,
	FACE_SOUTH
};
static char faceAbbr[4] = { 'w', 'e', 'n', 's' };

class Stair : public Quad {
public:
	Stair(std::vector<float> params);

	void SetDirection(FACE_DIRECTION direction);
	FACE_DIRECTION GetDirection() const;

	void InstanciateQuad(float width, float height);

private:
	FACE_DIRECTION direction;

	std::vector<float> params;
};

class Ceiling : public Quad {
public:
	Ceiling(std::vector<float> params);

	void InstanciateQuad(float width, float height);

private:
	std::vector<float> params;
};

class Corridor : public Quad {
public:
	Corridor(std::vector<float> params);

	void AddWall(int direction);
	bool GetWall(int direction);
	void AddDoor(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetDoors();
	void AddWindow(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetWindows();

	void InstanciateQuad(float width, float height);

private:
	std::vector<bool> walls;
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> doors;
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> windows;

	std::vector<float> params;
};

class Single : public Quad {
public:
	Single(std::vector<float> params);

	void AddDoor(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetDoors();
	void AddWindow(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetWindows();

	void InstanciateQuad(float width, float height);

private:
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> doors;
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> windows;

	std::vector<float> params;
};

class Row : public Quad {
public:
	Row(std::vector<float> params);

	void SetDirection(FACE_DIRECTION direction);
	FACE_DIRECTION GetDirection() const;

	void AddDoor(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetDoors();
	void AddWindow(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetWindows();

	void InstanciateQuad(float width, float height);

private:
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> doors;
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> windows;

	FACE_DIRECTION direction;
	std::vector<float> params;
};

// 楼层
class Floor : public Quad {
public:
	Floor(int level, float width, float height);
	~Floor();

	void AddStair(Stair stair);
	void AddCeiling(Ceiling ceiling);
	void AddCorridor(Corridor corridor);
	void AddSingle(Single single);
	void AddRow(Row row);

	// 获取楼层
	int GetLevel() const;

	// 访问组件
	std::vector<Stair>& GetStairs();
	std::vector<Ceiling>& GetCeilings();
	std::vector<Corridor>& GetCorridors();
	std::vector<Single>& GetSingles();
	std::vector<Row>& GetRows();

	// 分配门牌号
	int AssignNumber();

private:
	int level;

	std::vector<Stair> stairs;
	std::vector<Ceiling> ceilings;
	std::vector<Corridor> corridors;
	std::vector<Single> singles;
	std::vector<Row> rows;

	int number = 0;
};

class Layout {
public:
	std::unordered_map<std::string, std::vector<std::vector<Stair>>> templateStairs;
	std::unordered_map<std::string, std::vector<std::vector<Ceiling>>> templateCeilings;
	std::unordered_map<std::string, std::vector<std::vector<Corridor>>> templateCorridors;
	std::unordered_map<std::string, std::vector<std::vector<Single>>> templateSingles;
	std::unordered_map<std::string, std::vector<std::vector<Row>>> templateRows;
};

class Building : public Quad {
public:
    Building();
	virtual ~Building();;

	// 子类实现方法

    // 动态返回建筑静态信息
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 功能区中的建筑权重
    static std::vector<float> GetPower();

    // 建筑面积范围
    virtual float RandomAcreage() const = 0;
    virtual float GetAcreageMin() const = 0;
    virtual float GetAcreageMax() const = 0;

    // 建筑房间布局
	virtual Quad LayoutConstruction() = 0;
    virtual void LayoutRooms(
		ComponentFactory* componentFactory, RoomFactory* roomFactory, Layout* layout) = 0;

	// 父类实现方法

	// 关联地块
	void SetParent(Plot* plot);
	void SetParent(Zone* zone);
	Plot* GetParentPlot() const;
	Zone* GetParentZone() const;

	// 获取/设置房东
	int GetOwner() const;
	void SetOwner(int id);
	bool GetStateOwned() const;
	void SetStateOwned(bool state);

	// 获取/设置属性
	int GetLayers() const;
	int GetBasements() const;
	float GetHeight() const;
	const Quad GetConstruction() const;

	// 获取/设置组织/房间/楼层
	std::vector<Component*>& GetComponents();
	std::vector<Room*>& GetRooms();
	Floor* GetFloor(int level) const;

	// 补充初始化
	void FinishInit();

	// 动态位置
	std::pair<float, float> GetPosition() const;

	// 读入布局模板
	static Layout* ReadTemplates(std::string path);
	static std::vector<float> InverseParams(std::vector<float>& params, int face);
	static int InverseDirection(int direction, int face);

protected:
	// 根据布局文件分配房间
	void ReadFloor(int level, int face, std::string name, Layout* layout);
	void ReadFloors(int face, std::string name, Layout* layout);
	void ReadFloors(int face, std::vector<std::string> names, Layout* layout);
	void AssignRoom(int level, int slot, std::string name, Component* component, RoomFactory* factory);
	void ArrangeRow(int level, int slot, std::string name, float acreage, Component* component, RoomFactory* factory);

	// 建筑中添加组织
	Component* CreateComponent(std::string name, ComponentFactory* factory);

	Zone* parentZone = nullptr;
	Plot* parentPlot = nullptr;

	bool stateOwned = false;
	int ownerId = -1;

	std::vector<Floor*> floors;
	std::vector<Component*> components;
	std::vector<Room*> rooms;

	int layers = 1;
	int basements = 0;
	float height = 0.4f;
	Quad construction;

};

class BuildingFactory {
public:
    void RegisterBuilding(const std::string& id,
		std::function<Building* ()> creator, std::vector<float> powers);
	Building* CreateBuilding(const std::string& id);
    bool CheckRegistered(const std::string& id);
    void SetConfig(std::string name, bool config);
    const std::unordered_map<std::string, std::vector<float>>& GetPowers() const;

private:
    std::unordered_map<std::string, std::function<Building *()>> registries;
    std::unordered_map<std::string, bool> configs;
    std::unordered_map<std::string, std::vector<float>> powers;
};