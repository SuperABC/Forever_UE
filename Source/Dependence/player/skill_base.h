#pragma once

#include <string>
#include <functional>
#include <memory>


class Skill {
public:
    Skill();
    virtual ~Skill();

    // 子类实现方法

    // 全成员默认构造
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 父类实现方法

private:
};

class SkillFactory {
public:
    // 清空注册
    void RemoveAll();

    // 注册技能
    void RegisterSkill(const std::string& id,
        std::function<Skill*()> creator, std::function<void(Skill*)> deleter);

    // 创建技能（包含new操作）
    Skill* CreateSkill(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 获取所有启用的技能
    std::vector<Skill*> GetSkills() const;

    // 析构技能（包含delete操作）
    void DestroySkill(Skill* skill) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Skill*()>, std::function<void(Skill*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
};

