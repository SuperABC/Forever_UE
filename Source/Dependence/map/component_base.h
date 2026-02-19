#pragma once

#include "room_base.h"
#include "building_base.h"

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>


class Building;
class Room;

class Component {
public:
    Component() = default;
    virtual ~Component() = default;

	// 子类实现方法

    // 动态返回组合静态信息
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

	// 父类实现方法

    // 关联建筑
    void SetParent(Building* building);
    Building* GetParent() const;

    // 获取/添加房间
    std::vector<Room *>& GetRooms();
    void AddRoom(Room* room);

protected:
    Building* parentBuilding;

    std::vector<Room*> rooms;
};

class ComponentFactory {
public:
    void RegisterComponent(const std::string& id, std::function<Component* ()> creator);
    Component* CreateComponent(const std::string& id);
    bool CheckRegistered(const std::string& id);
    void SetConfig(std::string name, bool config);

private:
    std::unordered_map<std::string, std::function<Component* ()>> registries;
    std::unordered_map<std::string, bool> configs;
};