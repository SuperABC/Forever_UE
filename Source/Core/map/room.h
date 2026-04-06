#pragma once

#include "map/room_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModRoomsFunc)(RoomFactory* factory);

// 房间实体
class Room : public Quad {
public:
	// 门窗类
	using WallHole = std::unordered_map<
		FACE_DIRECTION,
		std::vector<std::pair<std::vector<float>, Quad>>
	>;

	// 禁止默认构造
	Room() = delete;

	// 通过类型从工厂构造
	Room(RoomFactory* factory, const std::string& component);

	// 析构房间
	~Room();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 获取所在建筑
	Building* GetParentBuilding() const;

	// 设置所在建筑
	void SetParent(Building* building);

	// 获取所在组合
	Component* GetParentComponent() const;

	// 设置所在组合
	void SetParent(Component* component);

	// 获取所在层数
	int GetLayer() const;

	// 设置所在层数
	void SetLayer(int layer);

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

	// 读取房间配置
	void ConfigRoom();

	// 获取完整地址
	std::string GetAddress() const;

	// 获取世界坐标
	std::pair<float, float> GetPosition(float x, float y) const;

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

	// 作为仓库各分区容量
	std::unordered_map<std::string, float> StorageConfig() const;

	// 作为工坊包含生产线种类
	std::vector<std::string> ManufactureTypes() const;

	// 获取是否由政府拥有
	bool GetStated() const;

	// 设置是否由政府拥有
	void SetStated(bool state);

	// 获取私人房东
	Person* GetOwner() const;

	// 设置私人房东
	void SetOwner(Person* owner);

	// 获取住户
	const std::vector<Person*>& GetTenants() const;

	// 添加住户
	void AddTenant(Person* person);

	// 移除住户
	bool RemoveTenant(const std::string& name);

	// 获取工人
	const std::vector<Person*>& GetWorkers() const;

	// 添加工人
	void AddWorker(Person* person);

	// 移除工人
	bool RemoveWorker(const std::string& name);

	// 获取仓库
	std::vector<Storage*> GetStorage() const;

	// 添加仓库
	void AddStorage(Storage* storage);

	// 清空仓库
	void ClearStorages();

	// 获取工坊
	std::vector<Manufacture*> GetManufactures() const;

	// 添加工坊
	void AddManufacture(Manufacture* manufacture);

	// 清空工坊
	void ClearManufactures();

private:
	// 模组对象
	RoomMod* mod;

	// 工厂
	RoomFactory* factory;

	// 房间类型
	std::string type;

	// 房间名称
	std::string name;

	// 所在建筑
	Building* parentBuilding;

	// 所在组合
	Component* parentComponent;

	// 所在楼层
	int layer;

	// 门
	WallHole doors;

	// 窗
	WallHole windows;

	// 门牌号
	std::string number;
	
	// 是否由政府拥有
	bool stated;

	// 私人房东
	Person* owner;

	// 屋内住户
	std::vector<Person*> tenants;

	// 屋内工人
	std::vector<Person*> workers;

	// 屋内仓库
	OBJECT_HOLDER std::vector<Storage*> storages;

	// 屋内工坊
	OBJECT_HOLDER std::vector<Manufacture*> manufactures;
};

// 空房间
class EmptyRoom : public RoomMod {
public:
	EmptyRoom();
	virtual ~EmptyRoom();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void ConfigRoom() override;

private:
	static int count;

	int id;
	std::string name;
};

