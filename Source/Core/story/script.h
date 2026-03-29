#pragma once

#include "script_mod.h"

#include "milestone.h"


// 子类注册函数
typedef void (*RegisterModScriptsFunc)(ScriptFactory* factory);

// 事件实体
class Script {
public:
	// 禁止默认构造
	Script() = delete;

	// 通过类型从工厂构造
	Script(ScriptFactory* factory, const std::string& script);

	// 析构事件
	~Script();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 获取变量
	std::pair<bool, ValueType> GetValue(const std::string& name) const;

	// 设置变量
	void SetValue(const std::string& name, ValueType value);

	// 移除变量
	void RemoveValue(const std::string& name);

	// 脚本前逻辑
	std::vector<Action> PreTrigger(Event* event);

	// 脚本后逻辑
	std::vector<Action> PostTrigger(Event* event);

	// 读取剧本
	static void ReadScript(const std::string& path);

	// 读取占用名
	static std::vector<std::string> ReadNames(const std::string& path);

	// 读取里程碑
	void ReadMilestones(const std::string& path);

	// 匹配事件
	std::vector<Action> MatchEvent(Event* event,
		std::vector < std::function<std::pair<bool, ValueType>(const std::string&)>> getValues);

	// 停用里程碑
	void DeactivateMilestone(const std::string& name);

private:
	// 读取事件
	static std::vector<Event*> BuildEvent(JsonValue root);

	// 读取对话
	static std::vector<Dialog*> BuildDialogs(JsonValue root);

	// 读取变化
	static std::vector<Change*> BuildChanges(JsonValue root);

	// 读取条件
	static Condition BuildCondition(JsonValue root);

	// 读取数组
	static std::vector<std::string> BuildSubsequences(JsonValue root);

	// 模组对象
	OBJECT_HOLDER ScriptMod* mod;

	// 工厂
	ScriptFactory* factory;

	// 事件类型
	std::string type;

	// 事件名称
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
	EmptyScript();
	virtual ~EmptyScript();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static bool MainStory();

	virtual void SetScript();

	virtual void PreTrigger(const Event* event);
	
	virtual void PostTrigger(const Event* event);

private:
	static int count;

	int id;
	std::string name;
};

