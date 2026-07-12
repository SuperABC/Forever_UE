#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


// 组合Mod基类
class ComponentMod {
public:
	/*
	* 构造组合
	*/
	ComponentMod();

	/*
	* 析构组合
	*/
	virtual ~ComponentMod();

	/*
	* Override
	* 组合静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 组合动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 组合实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 初始化组合
	*/
	COSTOM_INIT virtual void InitComponent() = 0;

	// 关联剧情与脚本
	std::pair<std::string, std::vector<std::string>> script;
};

class ComponentFactory {
public:
	/*
	* 注册组合
	* @id: 组合类型
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterComponent(const std::string& id,
		std::function<ComponentMod* ()> creator, std::function<void(ComponentMod*)> deleter);

	/*
	* 清空所有注册
	*/
	void RemoveAll();

	/*
	* 创建组合
	* @id: 组合类型
	*/
	ComponentMod* CreateComponent(const std::string& id) const;

	/*
	* 检查组合是否已注册
	* @id: 组合类型
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置组合启用配置
	* @name: 组合类型
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 析构组合
	* @componentMod: 待析构的组合
	*/
	void DestroyComponent(ComponentMod* componentMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<ComponentMod* ()>, std::function<void(ComponentMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

