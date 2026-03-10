#pragma once

#include "../common/quad.h"
#include "../common/plot.h"

#include "room_base.h"
#include "component_base.h"

#include <functional>
#include <memory>
#include <string>
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

// 楼梯
class Stair : public Quad {
public:
	Stair(std::vector<float> params);
	~Stair();

	FACE_DIRECTION GetDirection() const;
	void SetDirection(FACE_DIRECTION direction);

	void InstanciateQuad(float width, float height);

private:
	FACE_DIRECTION direction;
	std::vector<float> params;
};

// 天花板
class Ceiling : public Quad {
public:
	Ceiling(std::vector<float> params);
	~Ceiling();

	void InstanciateQuad(float width, float height);

private:
	std::vector<float> params;
};

// 走廊
class Corridor : public Quad {
public:
	Corridor(std::vector<float> params);
	~Corridor();

	bool GetWall(int direction) const;
	void AddWall(int direction);

	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetDoors() const;
	void AddDoor(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);

	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetWindows() const;
	void AddWindow(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);

	void InstanciateQuad(float width, float height);

private:
	std::vector<bool> walls;
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> doors;
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> windows;
	std::vector<float> params;
};

// 独立房间
class Single : public Quad {
public:
	Single(std::vector<float> params);
	~Single();

	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetDoors() const;
	void AddDoor(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);

	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetWindows() const;
	void AddWindow(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);

	void InstanciateQuad(float width, float height);

private:
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> doors;
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> windows;
	std::vector<float> params;
};

// 联排房间
class Row : public Quad {
public:
	Row(std::vector<float> params);
	~Row();

	FACE_DIRECTION GetDirection() const;
	void SetDirection(FACE_DIRECTION direction);

	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetDoors() const;
	void AddDoor(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);

	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetWindows() const;
	void AddWindow(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);

	void InstanciateQuad(float width, float height);

private:
	FACE_DIRECTION direction;
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> doors;
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> windows;
	std::vector<float> params;
};

// 楼层
class Floor : public Quad {
public:
	Floor(int level, float width, float height);
	~Floor();

	int GetLevel() const;

	std::vector<Stair>& GetStairs();
	std::vector<Ceiling>& GetCeilings();
	std::vector<Corridor>& GetCorridors();
	std::vector<Single>& GetSingles();
	std::vector<Row>& GetRows();

	void AddStair(Stair stair);
	void AddCeiling(Ceiling ceiling);
	void AddCorridor(Corridor corridor);
	void AddSingle(Single single);
	void AddRow(Row row);

	int AssignNumber();

private:
	int level;
	int number;

	std::vector<Stair> stairs;
	std::vector<Ceiling> ceilings;
	std::vector<Corridor> corridors;
	std::vector<Single> singles;
	std::vector<Row> rows;
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
	virtual ~Building();

	// 子类实现方法

	// 统一类型定义
	static std::string GetId();
	virtual std::string GetType() const = 0;
	virtual std::string GetName() const = 0;

	// 获取全类型地块权重
	static std::vector<float> GetPower();

	// 随机生成参考面积（实际面积会存在小偏差）
	virtual float RandomAcreage() const = 0;

	// 限制最小面积
	virtual float GetAcreageMin() const = 0;

	// 限制最大面积
	virtual float GetAcreageMax() const = 0;

	// 设置建筑楼体范围
	virtual Quad LayoutConstruction() = 0;

	// 布局内部组合与房间
	virtual void LayoutRooms(
		ComponentFactory* componentFactory, RoomFactory* roomFactory, Layout* layout) = 0;

	// 父类实现方法

	// 获取所在地块
	Plot* GetParentPlot() const;

	// 设置所在地块
	void SetParent(Plot* plot);

	// 获取所在园区
	Zone* GetParentZone() const;

	// 设置所在园区
	void SetParent(Zone* zone);

	// 获取私人房东ID
	int GetOwner() const;

	// 设置私人房东ID
	void SetOwner(int owner);

	// 获取是否由政府拥有
	bool GetStated() const;

	// 设置是否由政府拥有
	void SetStated(bool stated);

	// 获取总地面层数
	int GetLayers() const;

	// 设置总地面层数
	void SetLayers(int layers);

	// 获取总地下层数
	int GetBasements() const;

	// 设置总地下层数
	void SetBasements(int basements);

	// 获取层高
	float GetHeight() const;

	// 设置层高
	void SetHeight(float height);

	// 获取建筑楼体范围
	const Quad GetConstruction() const;

	// 获取楼内全部组合	
	std::vector<Component*>& GetComponents();

	// 获取楼内全部房间
	std::vector<Room*>& GetRooms();

	// 获取楼层
	Floor* GetFloor(int level) const;

	// 完成初始化
	void FinishInit();

	// 获取建筑中心世界位置
	std::pair<float, float> GetPosition() const;

	// 获取地址
	std::string GetAddress() const;

	// 读取所有布局模板
	static Layout* ReadTemplates(std::string path);

protected:
	// 分配楼层空间（将floors操作移动至对应dll，否则析构时崩溃）
	void AllocateFloors();

	// 根据模板生成一层楼层
	void ReadFloor(int level, int face, std::string name, Layout* layout);

	// 按照单一模板生成所有楼层
	void ReadFloors(int face, std::string name, Layout* layout);

	// 按照层数个模板生成所有楼层
	void ReadFloors(int face, std::vector<std::string> names, Layout* layout);

	// 为模板中第slot个独立房间生成房间
	void AssignRoom(int level, int slot, std::string name,
		Component* component, RoomFactory* factory);

	// 为模板中第slot个联排房间生成房间
	void ArrangeRow(int level, int slot, std::string name, float acreage,
		Component* component, RoomFactory* factory);

	// 在建筑内生成空组合
	Component* CreateComponent(std::string name, ComponentFactory* factory);

private:
	// 根据转向修改矩形参数
	static std::vector<float> InverseParams(std::vector<float>& params, int face);

	// 根据转向修改朝向参数
	static int InverseDirection(int direction, int face);

	// 所在园区
	Zone* parentZone;

	// 所在地块
	Plot* parentPlot;

	// 是否由政府拥有
	bool stated;

	// 私人房东ID
	int owner;

	// 地面层数
	int layers;

	// 地下层数
	int basements;

	// 层高
	float height;

	// 建筑楼体范围
	Quad construction;

	// 楼内楼层
	std::vector<Floor*> floors;

	// 楼内组合
	std::vector<Component*> components;

	// 楼内房间
	std::vector<Room*> rooms;
};

class BuildingFactory {
public:
	// 注册建筑
	void RegisterBuilding(const std::string& id, const std::vector<float>& powers,
		std::function<Building* ()> creator, std::function<void(Building*)> deleter);

	// 创建建筑（包含new操作）
	Building* CreateBuilding(const std::string& id);

	// 检查是否注册
	bool CheckRegistered(const std::string& id);

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 获取所有建筑全地块权重
	const std::unordered_map<std::string, std::vector<float>> GetPowers();

	// 析构组合（包含delete操作）
	void DestroyBuilding(Building* building) const;

private:
	std::unordered_map<
		std::string,
		std::pair<std::function<Building* ()>, std::function<void(Building*)>>
	> registries;
	std::unordered_map<std::string, bool> configs;
	std::unordered_map<std::string, std::vector<float>> powers;
};
