#include "dialog.h"


using namespace std;

Option::Option(const Condition& condition, const string& option,
	const vector<Dialog*>& dialogs, const vector<Change*>& changes) :
	condition(condition), option(option), dialogs(dialogs), changes(changes){

}

Option::~Option() {

}

Condition& Option::GetCondition() {
	return condition;
}

string Option::GetOption() const {
	return option;
}

vector<Dialog*> Option::GetDialogs() const {
	return dialogs;
}

vector<Change*> Option::GetChanges() const {
	return changes;
}

Section::Section(string speaker, string content, string label) : speaking({speaker, content, label}) {
	branch = false;
}

Section::Section(vector<Option> options) : options(options) {
	branch = true;
}

Section::~Section() {

}

bool Section::IsBranch() const {
	return branch;
}

tuple<string, string, string> Section::GetSpeaking() const {
	return speaking;
}

vector<Option> Section::GetOptions() const {
	return options;
}

Dialog::Dialog() {

}

Dialog::~Dialog() {

}

void Dialog::AddDialog(string speaker, string content, string label) {
	list.push_back(Section(speaker, content, label));
}

void Dialog::AddDialog(vector<Option> options) {
	list.push_back(Section(options));
}

vector<Section> Dialog::GetDialogs() {
	return list;
}

void Dialog::SetCondition(Condition condition) {
	this->condition = condition;
}
Condition& Dialog::GetCondition() {
	return condition;
}

