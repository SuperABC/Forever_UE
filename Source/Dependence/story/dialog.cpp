#include "dialog.h"


using namespace std;

Option::Option(const Condition& condition, const string& option,
	const vector<Dialog*>& dialogs, const vector<Change*>& changes) :
	condition(condition), option(option), dialogs(dialogs), changes(changes.begin(), changes.end()) {

}

Option::~Option() {

}

const Condition& Option::GetCondition() const {
	return condition;
}

string Option::GetOption() const {
	return option;
}

vector<Dialog*> Option::GetDialogs() const {
	return dialogs;
}

vector<const Change*> Option::GetChanges() const {
	return changes;
}

Section::Section(string speaker, string content, string label, string voice) : speaking({speaker, content, label, voice}), ownerScript(nullptr) {
	branch = false;
}

Section::Section(vector<Option> options) : options(options), ownerScript(nullptr) {
	branch = true;
}

Section::~Section() {

}

bool Section::IsBranch() const {
	return branch;
}

void Section::EvaluateText(const vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	Condition condition;

	condition.ParseCondition(get<0>(speaking));
	get<0>(speaking) = ToString(condition.EvaluateValue(getValues));
	condition.ParseCondition(get<1>(speaking));
	get<1>(speaking) = ToString(condition.EvaluateValue(getValues));
	condition.ParseCondition(get<2>(speaking));
	get<2>(speaking) = ToString(condition.EvaluateValue(getValues));
	condition.ParseCondition(get<3>(speaking));
	get<3>(speaking) = ToString(condition.EvaluateValue(getValues));
	for(auto& option : options) {
		condition.ParseCondition(option.GetOption());
		option.GetOption() = ToString(condition.EvaluateValue(getValues));
	}
}

tuple<string, string, string, string> Section::GetSpeaking() const {
	return speaking;
}

vector<Option> Section::GetOptions() const {
	return options;
}

void Section::SetOwnerScript(Script* script) {
	ownerScript = script;
}

Script* Section::GetOwnerScript() const {
	return ownerScript;
}

Dialog::Dialog() {

}

Dialog::~Dialog() {

}

void Dialog::AddDialog(string speaker, string content, string label, string voice) {
	list.push_back(Section(speaker, content, label, voice));
}

void Dialog::AddDialog(vector<Option> options) {
	list.push_back(Section(options));
}

vector<Section> Dialog::GetDialogs() const {
	return list;
}

void Dialog::SetCondition(Condition condition) {
	this->condition = condition;
}
const Condition& Dialog::GetCondition() const {
	return condition;
}

