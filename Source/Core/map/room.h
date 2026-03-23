#pragma once

#include "room_mod.h"

#include "building.h"
#include "component.h"


// 子类注册函数
typedef void (*RegisterModRoomsFunc)(RoomFactory* factory);

enum FACE_DIRECTION : int;

// 房间实体
class Building;
class Component;
class Room {
public:
	using WallHole = std::unordered_map<
		FACE_DIRECTION,
		std::vector<std::pair<std::vector<float>, Quad>>
	>;

	Room() = delete;
	Room(RoomFactory* factory, std::string component);
	~Room();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 获取几何
	Quad* GetQuad();

	// 获取所在建筑
	Building* GetParentBuilding() const;

	// 获取所在组合
	Component* GetParentComponent() const;

	// 设置所在建筑
	void SetParent(Building* building);

	// 设置所在组合
	void SetParent(Component* component);

	// 获取所在层数
	int GetLayer() const;

	// 设置所在层数
	void SetLayer(int layer);

	// 获取是否由政府拥有
	bool GetStated() const;

	// 设置是否由政府拥有
	void SetStated(bool stated);

	// 获取私人房东ID
	//int GetOwner() const;

	// 设置私人房东ID
	//void SetOwner(int owner);

	// 获取门框
	const WallHole& GetDoors() const;

	// 设置门框
	void SetDoors(const WallHole& doors);

	// 获取窗框
	const WallHole& GetWindows() const;

	// 设置窗框
	void SetWindows(const WallHole& windows);

	// 获取门牌号
	std::string GetNumber() const;

	// 设置门牌号
	void SetNumber(int floor, int number);

	// 获取住户
	//const std::vector<int>& GetTenants() const;

	// 添加住户
	//void AddTenant(int id);

	// 移除住户
	//bool RemoveTenant(int id);

	// 获取工人
	//const std::vector<int>& GetWorkers() const;

	// 添加工人
	//void AddWorker(int id);

	// 移除工人
	//bool RemoveWorker(int id);

	// 获取仓库
	//Storage* GetStorage() const;

	// 设置仓库
	//void SetStorage(Storage* storage);

	// 移除仓库（返回值需要delete）
	//Storage* RemoveStorage();

	// 获取工坊
	//const std::vector<Manufacture*>& GetManufactures() const;

	// 添加工坊
	//void AddManufacture(Manufacture* manufacture);

	// 移除工坊（只操作第一个符合名称的对象，且返回值需要delete）
	//Manufacture* RemoveManufacture(const std::string& name);

	// 清空工坊（返回值需要delete）
	//std::vector<Manufacture*> ClearManufactures();

	// 获取世界坐标
	std::pair<float, float> GetPosition(float x, float y) const;

	// 获取完整地址
	std::string GetAddress() const;

	// 是否可作为住宅
	bool IsResidential() const;

	// 是否可作为工位
	bool IsWorkspace() const;

	// 是否可作为仓库
	bool IsStorage() const;

	// 是否可作为工坊
	bool IsManufacture() const;

	// 作为住宅容纳人数
	int ResidentialCapacity() const;

	// 作为工位容纳人数
	int WorkspaceCapacity() const;

	// 作为仓库各类产品容量
	std::pair<std::string, float> StorageConfig() const;

	// 作为工坊包含生产线种类
	std::vector<std::string> ManufactureTypes() const;

private:
	RoomMod* mod;
	RoomFactory* factory;

	std::string type;
	std::string name;

	Building* parentBuilding;
	Component* parentComponent;
	int layer;
	bool stated;
	//Person* owner;
	WallHole doors;
	WallHole windows;
	std::string number;
	//std::vector<Person*> tenants;
	//std::vector<Person*> workers;
	//VARIABLE_HOLDER Storage* storage;
	//VARIABLE_HOLDER std::vector<Manufacture*> manufactures;
};

// 空房间
class EmptyRoom : public RoomMod {
public:
	EmptyRoom();
	virtual ~EmptyRoom();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void ConfigRoom();

private:
	static int count;

	int id;
	std::string name;
};

