#pragma once

#include "story/script_mod.h"

#include "class.h"

#include "milestone.h"


// 子类注册函数
typedef void (*RegisterModScriptsFunc)(ScriptFactory* factory);

// 事件实体
class Script : public Container {
public:
	/*
	* 禁止默认构造
	*/
	Script() = delete;

	/*
	* 通过类型从工厂构造
	* @factory: 脚本工厂
	* @script: 脚本静态类型标识
	*/
	Script(ScriptFactory* factory, const std::string& script);

	/*
	* 析构脚本
	*/
	~Script();

	/*
	* 获取类型
	*/
	std::string GetType() const;

	/*
	* 获取名称
	*/
	std::string GetName() const;

	/*
	* 获取变量值，返回 {是否存在, 值}
	* @name: 变量名称
	*/
	std::pair<bool, ValueType> GetValue(const std::string& name) const override;

	/*
	* 正则匹配获取变量
	* @reg: 正则表达式
	*/
	std::map<std::string, ValueType> GetValues(const std::string& reg) const;

	/*
	* 设置变量值
	* @name: 变量名称
	* @value: 变量值
	*/
	void SetValue(const std::string& name, ValueType value) override;

	/*
	* 移除变量
	* @name: 变量名称
	*/
	void RemoveValue(const std::string& name);

	/*
	* 获取任务描述
	*/
	std::string GetTask();

	/*
	* 脚本逻辑重载入口
	* @event: 触发事件
	* @actions: 当前动作队列
	* @getValues: 值获取回调列表
	*/
	std::vector<ScriptAction>& WrapScript(Event* event, std::vector<ScriptAction>& actions,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	/*
	* 读取剧本文件到缓存
	* @path: 剧本文件路径
	*/
	static void ReadScript(const std::string& path);

	/*
	* 读取剧本中的占用名列表
	* @path: 剧本文件路径
	*/
	static std::vector<std::string> ReadNames(const std::string& path);

	/*
	* 从缓存加载里程碑到当前脚本
	* @path: 剧本文件路径
	*/
	void ReadMilestones(const std::string& path);

	/*
	* 匹配事件，返回触发的动作列表
	* @event: 触发事件
	* @getValues: 值获取回调列表
	*/
	std::vector<ScriptAction> MatchEvent(Event* event,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	/*
	* 停用指定里程碑
	* @name: 里程碑名称
	*/
	void DeactivateMilestone(const std::string& name);

	/*
	* 清空里程碑、活动状态与变量
	*/
	void ClearContext();

private:
	/*
	* 从JSON解析事件列表
	* @root: JSON节点
	*/
	static std::vector<Event*> BuildEvent(JsonValue root);

	/*
	* 从JSON解析对话列表
	* @root: JSON节点
	*/
	static std::vector<Dialog*> BuildDialogs(JsonValue root);

	/*
	* 从JSON解析变化列表
	* @root: JSON节点
	*/
	static std::vector<Change*> BuildChanges(JsonValue root);

	/*
	* 从JSON解析条件
	* @root: JSON节点
	*/
	static Condition BuildCondition(JsonValue root);

	/*
	* 从JSON解析后续里程碑名列表
	* @root: JSON节点
	*/
	static std::vector<std::string> BuildSubsequences(JsonValue root);

	// 模组对象
	OBJECT_HOLDER ScriptMod* mod;

	// 工厂
	ScriptFactory* factory;

	// 脚本类型
	std::string type;

	// 脚本名称
	std::string name;

	// 脚本缓存
	OBJECT_HOLDER static std::unordered_map<std::string, std::pair<std::vector<std::string>, std::unordered_map<std::string, Milestone*>>> caches;

	// 里程碑列表
	std::unordered_map<std::string, MilestoneNode> milestones;

	// 活动里程碑
	std::vector<MilestoneNode*> actives;

	// 变量列表
	std::unordered_map<std::string, ValueType> variables;

};

// 空脚本
class EmptyScript : public ScriptMod {
public:
	/*
	* 构造空脚本
	*/
	EmptyScript();

	/*
	* 析构空脚本
	*/
	virtual ~EmptyScript();

	/*
	* Override
	* 脚本静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 脚本动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 脚本实例名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 是否为主线剧情脚本
	*/
	static bool MainStory();

	/*
	* Override
	* 设置脚本文件
	*/
	virtual void SetScript() override;

	/*
	* Override
	* 脚本逻辑重载
	* @event: 触发事件
	* @getValues: 值获取回调列表
	*/
	virtual void WrapScript(const Event* event,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;

};

