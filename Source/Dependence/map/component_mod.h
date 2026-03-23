#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class ComponentMod {
public:
	// 无构造
	ComponentMod();

	// 无析构
	virtual ~ComponentMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;
};

class ComponentFactory {
public:
	// 注册组合
	void RegisterComponent(const std::string& id,
		std::function<ComponentMod* ()> creator, std::function<void(ComponentMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建组合
	ComponentMod* CreateComponent(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 析构组合
	void DestroyComponent(ComponentMod* terrain) const;

private:
	std::unordered_map<
		std::string,
		std::pair<std::function<ComponentMod* ()>, std::function<void(ComponentMod*)>>
	> registries;
	std::unordered_map<std::string, bool> configs;
};

