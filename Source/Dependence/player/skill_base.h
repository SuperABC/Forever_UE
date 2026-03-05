#pragma once

#include <string>
#include <functional>
#include <memory>


class Skill {
public:
    Skill() = default;
    virtual ~Skill() = default;

    // 子类实现方法

    // 动态返回组合静态信息
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 父类实现方法

protected:
};

class SkillFactory {
public:
    void RegisterSkill(const std::string& id,
        std::function<Skill*()> creator, std::function<void(Skill*)> deleter);
    Skill* CreateSkill(const std::string& id);
    bool CheckRegistered(const std::string& id);
    void SetConfig(std::string name, bool config);
    std::vector<Skill*> GetSkills();
    void DestroySkill(Skill* skill) const;

private:
    std::unordered_map<std::string, std::pair<std::function<Skill*()>, std::function<void(Skill*)>>> registries;
    std::unordered_map<std::string, bool> configs;
};