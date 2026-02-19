#pragma once

#include "../common/quad.h"

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
    Room() = default;
    virtual ~Room() = default;

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
    virtual int GetLivingCapacity() const = 0;
    virtual int GetPersonnelCapacity() const = 0;
    virtual std::pair<std::string, float> GetIndustryCapacity() const = 0;
    virtual std::string GetFactoryPipeline() const = 0;

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
    int GetFace() const;
    void SetFace(int face);

protected:
    Building* parentBuilding;
    Component* parentComponent;

    bool stateOwned = false;
	int ownerId = -1;

    int layer;
    int face;
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