#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>


class Name {
public:
	Name();
	virtual ~Name();

	// 子类实现方法

	// 统一类型定义
	static std::string GetId();
	virtual std::string GetType() const = 0;
	virtual std::string GetName() const = 0;

	// 获取姓
	virtual std::string GetSurname(const std::string& name) = 0;

	// 生成完整姓名
	virtual std::string GenerateName(bool male = true, bool female = true, bool neutral = true) = 0;

	// 已有姓，生成名
	virtual std::string GenerateName(const std::string& surname, bool male = true, bool female = true, bool neutral = true) = 0;

	// 父类实现方法

	// 占用名字
	bool ReserveName(const std::string& name);

	// 使用名字
	bool RegisterName(const std::string& name);

private:
	std::unordered_set<std::string> reserve;
	std::unordered_set<std::string> roll;
};

class NameFactory {
public:
    // 注册姓名
    void RegisterName(const std::string& id,
        std::function<Name* ()> creator, std::function<void(Name*)> deleter);

    // 创建姓名（包含new操作）
    Name* CreateName(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 获取启用姓名
    Name* GetName() const;

    // 析构姓名（包含delete操作）
    void DestroyName(Name* name) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Name* ()>, std::function<void(Name*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
};

