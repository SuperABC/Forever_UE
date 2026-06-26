#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class ComponentMod {
public:
	/*
	* 无构造
	*/
	ComponentMod();

	/*
	* 无析构
	*/
	virtual ~ComponentMod();

	/*
	* Override
	* 组合静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 组合动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 组合实例唯一名称
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
	* 注册组合及其创建与销毁方式
	* @id: 组合静态类型标识
	* @creator, deleter: 创建/销毁组合实例的方法
	*/
	void RegisterComponent(const std::string& id,
		std::function<ComponentMod* ()> creator, std::function<void(ComponentMod*)> deleter);

	/*
	* 清空所有启用的组合配置
	*/
	void RemoveAll();

	/*
	* 按静态类型标识创建组合实例
	* @id: 组合静态类型标识
	*/
	ComponentMod* CreateComponent(const std::string& id) const;

	/*
	* 检查组合是否已注册
	* @id: 组合静态类型标识
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置组合的启用配置
	* @name: 组合静态类型标识
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 销毁组合实例
	* @componentMod: 待销毁的组合实例
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

