#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include "condition.h"
#include "change.h"

#include <vector>
#include <string>


class Dialog;

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

	std::tuple<std::string, std::string, std::string> GetSpeaking() const;
	std::vector<Option> GetOptions() const;

private:
	bool branch;

	std::tuple<std::string, std::string, std::string> speaking;
	std::vector<Option> options;
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

