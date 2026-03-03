#pragma once

#include "../common/quad.h"
#include "../industry/storage_base.h"
#include "../industry/manufacture_base.h"

#include "component_base.h"
#include "building_base.h"

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>


class Building;
class Component;

class Room : public Quad {
public:
    Room();
    virtual ~Room();

	// 子类实现方法

    // 动态返回房间静态信息
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 房间类型
    virtual bool IsResidential() const = 0;
    virtual bool IsWorkspace() const = 0;
    virtual bool IsStorage() const = 0;
    virtual bool IsManufacture() const = 0;
    virtual int ResidentialCapacity() const = 0;
    virtual int WorkspaceCapacity() const = 0;
    virtual std::pair<std::string, float> StorageConfig() const = 0;
    virtual std::vector< std::string> ManufactureTypes() const = 0;

	// 父类实现方法

    // 关联建筑
    void SetParent(Building* building);
    void SetParent(Component* component);
    Building* GetParentBuilding() const;
    Component* GetParentComponent() const;

    // 获取/设置房东
    int GetOwner() const;
    void SetOwner(int id);
    bool GetStateOwned() const;
    void SetStateOwned(bool state);

    // 获取/设置属性
    int GetLayer() const;
    void SetLayer(int layer);

    // 寻址
    void SetAddress(int number);
    std::string GetAddress() const;

	// 获取/设置租户/员工/仓储/生产
    std::vector<int> GetTenants();
	void AddTenant(int id);
    bool RemoveTenant(int id);
    std::vector<int> GetWorkers();
	void AddWorker(int id);
    bool RemoveWorker(int id);
    Storage* GetStorage();
	void SetStorage(Storage* storage);
    Storage* ClearStorage();
	std::vector<Manufacture*> GetManufactures();
	void AddManufacture(Manufacture* manufacture);
    Manufacture* RemoveManufacture(std::string manufacture);
    std::vector<Manufacture*> ClearManufactures();

protected:
    Building* parentBuilding;
    Component* parentComponent;

    bool stateOwned = false;
	int ownerId = -1;

    int layer;
    std::string address;

    std::vector<int> tenants;
	std::vector<int> workers;
	Storage* storage;
	std::vector<Manufacture*> manufactures;
};

class RoomFactory {
public:
    void RegisterRoom(const std::string& id, std::function<Room* ()> creator);
    Room* CreateRoom(const std::string& id);
    bool CheckRegistered(const std::string& id);
    void SetConfig(std::string name, bool config);

private:
    std::unordered_map<std::string, std::function<Room* ()>> registries;
    std::unordered_map<std::string, bool> configs;
};