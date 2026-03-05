#pragma once

#include <string>
#include <unordered_set>
#include <functional>
#include <memory>
#include <unordered_map>


class Name {
public:
    Name() = default;
    virtual ~Name() = default;

    // 子类实现方法

    // 动态返回姓名静态信息
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 生成随机姓名
    virtual std::string GetSurname(std::string name) = 0;
    virtual std::string GenerateName(bool male = true, bool female = true, bool neutral = true) = 0;
    virtual std::string GenerateName(std::string surname, bool male = true, bool female = true, bool neutral = true) = 0;

    // 父类实现方法
    bool ReserveName(std::string name);
    bool UseName(std::string name);
    bool RegisterName(std::string name);

protected:
    std::unordered_set<std::string> reserve;
    std::unordered_set<std::string> roll;
};

class NameFactory {
public:
    void RegisterName(const std::string& id,
                      std::function<Name*()> creator, std::function<void(Name*)> deleter);
    Name* CreateName(const std::string& id);
    bool CheckRegistered(const std::string& id);
    void SetConfig(std::string name, bool config);
    Name* GetName() const; // 注意：此方法已调整以适配新的registries结构
    void DestroyName(Name* name) const; // 新增销毁方法

private:
    std::unordered_map<std::string, std::pair<std::function<Name*()>, std::function<void(Name*)>>> registries;
    std::unordered_map<std::string, bool> configs;
};