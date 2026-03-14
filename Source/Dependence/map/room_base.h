#pragma once

#include "../common/quad.h"
#include "../industry/storage_base.h"
#include "../industry/manufacture_base.h"

#include "component_base.h"
#include "building_base.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>


enum FACE_DIRECTION : int;

class Building;
class Component;

class Room : public Quad {
public:
	using WallHole = std::unordered_map<
		FACE_DIRECTION,
		std::vector<std::pair<std::vector<float>, Quad>>
	>;

	Room();
	virtual ~Room();

	// 子类实现方法

	// 统一类型定义
	static std::string GetId();
	virtual std::string GetType() const = 0;
	virtual std::string GetName() const = 0;

	// 是否可作为住宅
	virtual bool IsResidential() const = 0;

	// 是否可作为工位
	virtual bool IsWorkspace() const = 0;

	// 是否可作为仓库
	virtual bool IsStorage() const = 0;

	// 是否可作为工坊
	virtual bool IsManufacture() const = 0;

	// 作为住宅容纳人数
	virtual int ResidentialCapacity() const = 0;

	// 作为工位容纳人数
	virtual int WorkspaceCapacity() const = 0;

	// 作为仓库各类产品容量
	virtual std::pair<std::string, float> StorageConfig() const = 0;

	// 作为工坊包含生产线种类
	virtual std::vector<std::string> ManufactureTypes() const = 0;

	// 父类实现方法

	// 获取所在建筑
	Building* GetParentBuilding() const;

	// 获取所在组合
	Component* GetParentComponent() const;

	// 设置所在建筑
	void SetParent(Building* building);

	// 设置所在组合
	void SetParent(Component* component);

	// 获取私人房东ID
	int GetOwner() const;

	// 设置私人房东ID
	void SetOwner(int owner);

	// 获取是否由政府拥有
	bool GetStated() const;

	// 设置是否由政府拥有
	void SetStated(bool stated);

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

	// 获取完整地址
	std::string GetAddress() const;

	// 获取住户
	const std::vector<int>& GetTenants() const;

	// 添加住户
	void AddTenant(int id);

	// 移除住户
	bool RemoveTenant(int id);

	// 获取工人
	const std::vector<int>& GetWorkers() const;

	// 添加工人
	void AddWorker(int id);

	// 移除工人
	bool RemoveWorker(int id);

	// 获取仓库
	Storage* GetStorage() const;

	// 设置仓库
	void SetStorage(Storage* storage);

	// 移除仓库（返回值需要delete）
	Storage* RemoveStorage();

	// 获取工坊
	const std::vector<Manufacture*>& GetManufactures() const;

	// 添加工坊
	void AddManufacture(Manufacture* manufacture);

	// 移除工坊（只操作第一个符合名称的对象，且返回值需要delete）
	Manufacture* RemoveManufacture(const std::string& name);

	// 清空工坊（返回值需要delete）
	std::vector<Manufacture*> ClearManufactures();

	// 获取世界坐标
	std::pair<float, float> GetPosition(float x, float y) const;

private:
	// 所在建筑
	Building* parentBuilding;

	// 所在组合
	Component* parentComponent;

	// 是否由政府拥有
	bool stated;

	// 私人房东ID
	int owner;

	// 所在层数
	int layer;

	// 门窗框
	WallHole doors;
	WallHole windows;

	// 门牌号
	std::string address;

	// 屋内住户ID
	std::vector<int> tenants;

	// 屋内工人ID
	std::vector<int> workers;

	// 屋内仓库
	Storage* storage;

	// 屋内工坊
	std::vector<Manufacture*> manufactures;
};

class RoomFactory {
public:
    // 注册房间
    void RegisterRoom(const std::string& id,
        std::function<Room* ()> creator, std::function<void(Room*)> deleter);

    // 创建房间（包含new操作）
    Room* CreateRoom(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 析构房间（包含delete操作）
    void DestroyRoom(Room* room) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Room* ()>, std::function<void(Room*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
};

