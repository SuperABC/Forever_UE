#pragma once

#include "building_mod.h"

#include "component.h"
#include "room.h"
#include "block.h"


// 子类注册函数
typedef void (*RegisterModBuildingsFunc)(BuildingFactory* factory);

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

// 建筑实体
class Block;
class Zone;
class Building {
public:
	Building() = delete;
	Building(BuildingFactory* factory, std::string building);
	~Building();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 获取几何
	Quad* GetQuad();

	// 采样面积
	float RandomAcreage();

	// 面积下限
	float GetAcreageMin();

	// 面积上限
	float GetAcreageMax();

	// 放置楼栋
	void PlaceConstruction();

	// 获取所在地块
	Block* GetParentBlock() const;

	// 设置所在地块
	void SetParent(Block* block);

	// 获取所在园区
	Zone* GetParentZone() const;

	// 设置所在园区
	void SetParent(Zone* zone);

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

	// 获取私人房东ID
	//int GetOwner() const;

	// 设置私人房东ID
	//void SetOwner(int owner);

	// 获取建筑中心世界位置
	void GetPosition(float& x, float& y) const;

	// 获取地址
	std::string GetAddress();

	// 布局内部对象
	void LayoutBuilding(Layout* layout, ComponentFactory* componentFactory, RoomFactory* roomFactory);

	// 读取所有布局模板
	static Layout* ReadTemplates(std::vector<std::string> paths);

private:
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

	// 根据转向修改矩形参数
	static std::vector<float> InverseParams(std::vector<float>& params, int face);

	// 根据转向修改朝向参数
	static int InverseDirection(int direction, int face);

	BuildingMod* mod;
	BuildingFactory* factory;

	std::string type;
	std::string name;

	Zone* parentZone;
	Block* parentBlock;
	int layers;
	int basements;
	float height;
	Quad construction;
	bool stated;
	//Person* owner;
	std::string fullAddress;
	OBJECT_HOLDER std::vector<Floor*> floors;
	OBJECT_HOLDER std::vector<Component*> components;
	OBJECT_HOLDER std::vector<Room*> rooms;
};

// 空建筑
class EmptyBuilding : public BuildingMod {
public:
	EmptyBuilding();
	virtual ~EmptyBuilding();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static std::vector<float> GetPowers();

	static std::function<int(Lot*)> BuildingAssigner;

	virtual float RandomAcreage();
	virtual void LayoutBuilding();
	virtual void PlaceConstruction();

private:
	static int count;

	int id;
	std::string name;
};

