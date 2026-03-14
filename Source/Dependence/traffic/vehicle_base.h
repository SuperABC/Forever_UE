#pragma once

#include <string>
#include <functional>
#include <memory>


class Vehicle {
public:
    Vehicle();
    virtual ~Vehicle();

    // 子类实现方法

    // 统一类型定义
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 父类实现方法

private:
};

class VehicleFactory {
public:
    // 注册载具
    void RegisterVehicle(const std::string& id,
        std::function<Vehicle*()> creator, std::function<void(Vehicle*)> deleter);

    // 创建载具（包含new操作）
    Vehicle* CreateVehicle(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 析转载具（包含delete操作）
    void DestroyVehicle(Vehicle* vehicle) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Vehicle*()>, std::function<void(Vehicle*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
};

