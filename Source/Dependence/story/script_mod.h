#pragma once

#include "../common/utility.h"
#include "../common/error.h"
#include "../common/json.h"
#include "../common/handle.h"

#include "condition.h"

#include <string>
#include <queue>
#include <variant>
#include <functional>


class Event;
class Dialog;
class Change;

using ScriptAction = std::variant<const Dialog*, Change*>;
using ReadOnlyScriptAction = std::variant<const Dialog*, const Change*>;

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
	* 是否启用WrapScript对动作队列的改写，默认不启用；mod需要修改actionQueue时应重载为返回true
	*/
	COSTOM_RUNTIME virtual bool EnableWrapping() const;

	/*
	* Override
	* 脚本逻辑重载，修改actionQueue字段；仅在EnableWrapping()返回true时才会被调用。
	* 重载时应在函数开头先调用AutoClean()清理上一次调用遗留的actionQueue内容，
	* 再通过DeepCopy将本次的只读动作列表拷贝进actionQueue
	* @event: 触发事件
	* @actions: 只读动作列表，需要写入actionQueue的内容请通过DeepCopy拷贝
	* @getValues: 值获取回调列表
	* @post: 向Core发起查询的句柄
	*/
	COSTOM_RUNTIME virtual void WrapScript(const Event* event, const std::vector<ReadOnlyScriptAction>& actions,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues,
		PostHandle* post);

	/*
	* Tool
	* 将只读动作列表深拷贝进actionQueue：Change克隆为独立副本（需通过AutoClean清理），
	* Dialog直接透传只读指针（其生命周期由Milestone永久持有，无需拷贝或清理）；
	* actionQueue里的指针都是const的，只能读取，不能调用setter修改内部字段——
	* 因为这些克隆对象的堆归属仍然是构造它们的那个模块（通常是Core），跨模块调用setter
	* （尤其是修改std::string/std::vector/std::map这类自己管理堆内存的成员）会导致
	* 分配和释放走不同模块的分配器，引发堆损坏。确实需要修改内容的话，只能new一个全新对象整体替换
	* @actions: 只读动作列表
	*/
	void DeepCopy(const std::vector<ReadOnlyScriptAction>& actions);

	/*
	* Tool
	* 遍历actionQueue：Change直接delete（对应DeepCopy克隆出的副本及mod自行new出的内容），
	* Dialog跳过不处理（其生命周期由Milestone永久持有），最后清空actionQueue；
	* 应在WrapScript重载开头调用，清理上一次调用遗留的内容
	*/
	void AutoClean();

	// 脚本文件路径
	std::string scriptPath;

	// 脚本动作队列（脚本前逻辑 + 脚本逻辑 + 脚本后逻辑）；元素均为const指针，只能读取，
	// 需要修改内容时必须new一个新对象整体替换，不能调用setter（详见DeepCopy的说明）
	std::vector<ReadOnlyScriptAction> actionQueue;

	// 是否正在处理WrapScript，用于防止同一脚本实例被递归重入（级联事件触发同一脚本的WrapScript）
	bool wrapping;

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
	std::string GetMain();

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

