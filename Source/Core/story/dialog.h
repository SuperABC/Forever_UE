#pragma once

#include "condition.h"
#include "change.h"

#include <vector>
#include <string>


class Dialog;

class Option {
public:
	Option(Condition condition, std::string option, std::vector<Dialog> dialogs,
		std::vector<Change*> changes);
	~Option();

	// 获取选项内容
	Condition& GetCondition();
	std::string GetOption() const;
	std::vector<Dialog> GetDialogs() const;
	std::vector<Change*> GetChanges() const;

private:
	Condition condition;
	std::string option;
	std::vector<Dialog> dialogs;
	std::vector<Change*> changes;
};

class Section {
public:
	Section(std::string speaker, std::string content);
	Section(std::vector<Option> options);
	~Section();

	// 获取类型
	bool IsBranch() const;

	// 获取对应内容
	std::pair<std::string, std::string> GetSpeaking() const;
	std::vector<Option> GetOptions() const;

private:
	bool branch;

	std::pair<std::string, std::string> speaking;
	std::vector<Option> options;
};

class Dialog {
public:
	Dialog();
	~Dialog();

	// 添加/获取对话
	void AddDialog(std::string speaker, std::string content);
	void AddDialog(std::vector<Option> options);
	std::vector<Section> GetDialogs();

	// 设置/获取条件
	void SetCondition(Condition condition);
	Condition& GetCondition();

private:
	std::vector<Section> list;

	Condition condition;
};