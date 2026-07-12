#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include "condition.h"
#include "change.h"

#include <vector>
#include <string>


class Dialog;
class Script;

class Option {
public:
	/*
	* 构造选项
	* @condition: 显示条件
	* @option: 选项文本
	* @dialogs: 选中后追加的对话列表
	* @changes: 选中后触发的变化列表
	*/
	Option(const Condition& condition, const std::string& option,
		const std::vector<Dialog*>& dialogs, const std::vector<Change*>& changes);

	/*
	* 析构选项
	*/
	~Option();

	/*
	* 获取显示条件
	*/
	const Condition& GetCondition() const;

	/*
	* 获取选项文本
	*/
	std::string GetOption() const;

	/*
	* 获取选中后追加的对话列表
	*/
	std::vector<Dialog*> GetDialogs() const;

	/*
	* 获取选中后触发的变化列表
	*/
	std::vector<Change*> GetChanges() const;

private:
	// 显示条件
	Condition condition;

	// 选项文本
	std::string option;

	// 选中后追加的对话列表
	std::vector<Dialog*> dialogs;

	// 选中后触发的变化列表
	std::vector<Change*> changes;

};

class Section {
public:
	/*
	* 构造对话段（普通台词）
	* @speaker, content, label: 发言者、内容与标签
	*/
	Section(std::string speaker, std::string content, std::string label);

	/*
	* 构造对话段（分支选项）
	* @options: 选项列表
	*/
	Section(std::vector<Option> options);

	/*
	* 析构对话段
	*/
	~Section();

	/*
	* 判断是否为分支选项段
	*/
	bool IsBranch() const;

	/*
	* 对台词文本中的表达式求值
	* @getValues: 值获取回调列表
	*/
	void EvaluateText(const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	/*
	* 获取台词（发言者, 内容, 标签）
	*/
	std::tuple<std::string, std::string, std::string> GetSpeaking() const;

	/*
	* 获取选项列表
	*/
	std::vector<Option> GetOptions() const;

	/*
	* 设置所属脚本
	* @script: 脚本对象
	*/
	void SetOwnerScript(Script* script);

	/*
	* 获取所属脚本
	*/
	Script* GetOwnerScript() const;

private:
	// 是否为分支选项
	bool branch;

	// 台词（发言者, 内容, 标签）
	std::tuple<std::string, std::string, std::string> speaking;

	// 选项列表
	std::vector<Option> options;

	// 所属脚本
	Script* ownerScript;

};

class Dialog {
public:
	/*
	* 构造对话
	*/
	Dialog();

	/*
	* 析构对话
	*/
	~Dialog();

	/*
	* 添加普通台词段
	* @speaker, content, label: 发言者、内容与标签
	*/
	void AddDialog(std::string speaker, std::string content, std::string label);

	/*
	* 添加分支选项段
	* @options: 选项列表
	*/
	void AddDialog(std::vector<Option> options);

	/*
	* 获取所有对话段
	*/
	std::vector<Section> GetDialogs();

	/*
	* 设置显示条件
	* @condition: 条件对象
	*/
	void SetCondition(Condition condition);

	/*
	* 获取显示条件
	*/
	const Condition& GetCondition() const;

private:
	// 对话段列表
	std::vector<Section> list;

	// 显示条件
	Condition condition;

};

