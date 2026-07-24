#pragma once

#include "../common/utility.h"
#include "../common/error.h"
#include "../common/json.h"
#include "../common/handle.h"

#include "condition.h"

#include <string>
#include <queue>
#include <deque>
#include <variant>
#include <functional>


class Event;
class Dialog;
class Change;

using ScriptAction = std::variant<const Dialog*, const Change*>;

// 脚本Mod基类
class ScriptMod {
public:
	/*
	* 构造脚本
	*/
	ScriptMod();

	/*
	* 析构脚本
	*/
	virtual ~ScriptMod();

	/*
	* Override
	* 脚本静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 脚本动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 脚本实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 是否为主线剧情脚本
	*/
	COSTOM_INIT static bool MainStory();

	/*
	* Override
	* 设置脚本文件，将脚本路径写入scriptPath字段
	*/
	COSTOM_INIT virtual void SetScript() = 0;

	/*
	* Override
	* 脚本逻辑重载，往actionStack里塞入mod自己长期持有的Dialog/Change指针；每次调用都会执行。
	* 重载时应在函数开头先调用AutoCopy(actions)，把这一层要处理的动作先搬进actionStack，
	* 再按需用FindLabel找到PlaceHolderChange的位置、替换成mod自己持有的指针。
	* mod往actionStack里塞的指针必须是mod自己长期维护、永不delete的对象——
	* 就像Milestone永久持有Core自己的Dialog/Change一样，Core这边也绝不会delete这些指针。
	* 默认实现：AutoCopy(actions)，即原样透传，不做任何改写
	* @event: 触发事件
	* @actions: 只读动作列表
	* @getValues: 值获取回调列表
	* @post: 向Core发起查询的句柄
	*/
	COSTOM_RUNTIME virtual void WrapScript(const Event* event, const std::vector<ScriptAction>& actions,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues,
		PostHandle* post);

	/*
	* Override
	* 处理完当前层的所有动作之后，Core会调用它来弹出WrapScript这次压入的那一层；
	* 是虚函数，即使mod不重载，也会走mod自己所在模块链接的这份默认实现，弹出操作
	* 天然发生在构造这个ScriptMod对象的那个模块里，不需要mod额外做任何事。
	* 默认实现：actionStack.pop_back()
	*/
	COSTOM_RUNTIME virtual void AutoPop();

	/*
	* Tool
	* 在actionStack里新增一层，把这一层要处理的动作列表搬进去；应在WrapScript重载开头调用
	* @actions: 只读动作列表
	*/
	void AutoCopy(const std::vector<ScriptAction>& actions);

	/*
	* Tool
	* 在actionStack当前层（最上层）里查找label匹配的PlaceHolderChange，
	* label允许重复，返回第一个匹配到的下标；找不到返回-1
	* @label: 要查找的标签
	*/
	int FindLabel(const std::string& label) const;

	// 脚本文件路径
	std::string scriptPath;

	// 脚本动作栈：每次WrapScript调用对应一层。指针全部是const的，Dialog的生命周期由Milestone
	// 永久持有，Change要么由Milestone永久持有、要么由mod自己永久持有，这里绝不delete任何指针
	std::deque<std::vector<ScriptAction>> actionStack;

};

class ScriptFactory {
public:
	/*
	* 注册脚本
	* @id: 脚本类型
	* @main: 是否为主线剧情脚本
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterScript(const std::string& id, bool main,
		std::function<ScriptMod* ()> creator, std::function<void(ScriptMod*)> deleter);

	/*
	* 清空所有注册
	*/
	void RemoveAll();

	/*
	* 创建脚本
	* @id: 脚本类型
	*/
	ScriptMod* CreateScript(const std::string& id) const;

	/*
	* 检查脚本是否已注册
	* @id: 脚本类型
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置脚本启用配置
	* @name: 脚本类型
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 获取主线剧情脚本类型标识
	*/
	std::string GetMain() const;

	/*
	* 析构脚本
	* @scriptMod: 待析构的脚本
	*/
	void DestroyScript(ScriptMod* scriptMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<ScriptMod* ()>, std::function<void(ScriptMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;

	// 主线剧情脚本类型标识
	std::string main;

};
