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
	Option(const Condition& condition, const std::string& option,
		const std::vector<Dialog*>& dialogs, const std::vector<Change*>& changes);
	~Option();

	Condition& GetCondition();
	std::string GetOption() const;
	std::vector<Dialog*> GetDialogs() const;
	std::vector<Change*> GetChanges() const;

private:
	Condition condition;
	std::string option;
	std::vector<Dialog*> dialogs;
	std::vector<Change*> changes;
};

class Section {
public:
	Section(std::string speaker, std::string content, std::string label);
	Section(std::vector<Option> options);
	~Section();

	bool IsBranch() const;

	void EvaluateText(std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues);

	std::tuple<std::string, std::string, std::string> GetSpeaking() const;
	std::vector<Option> GetOptions() const;

	void SetOwnerScript(Script* script);
	Script* GetOwnerScript() const;

private:
	bool branch;

	std::tuple<std::string, std::string, std::string> speaking;
	std::vector<Option> options;

	Script* ownerScript = nullptr;
};

class Dialog {
public:
	Dialog();
	~Dialog();

	void AddDialog(std::string speaker, std::string content, std::string label);
	void AddDialog(std::vector<Option> options);
	std::vector<Section> GetDialogs();

	void SetCondition(Condition condition);
	Condition& GetCondition();

private:
	std::vector<Section> list;

	Condition condition;
};

