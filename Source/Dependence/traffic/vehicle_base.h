#pragma once

#include <string>
#include <functional>
#include <memory>


class Vehicle {
public:
    Vehicle() = default;
    virtual ~Vehicle() = default;

    // 子类实现方法

    // 动态返回组合静态信息
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 父类实现方法

protected:
};

class VehicleFactory {
public:
    void RegisterVehicle(const std::string& id, std::function<Vehicle* ()> creator);
    Vehicle* CreateVehicle(const std::string& id);
    bool CheckRegistered(const std::string& id);
    void SetConfig(std::string name, bool config);

private:
    std::unordered_map<std::string, std::function<Vehicle* ()>> registries;
    std::unordered_map<std::string, bool> configs;
};