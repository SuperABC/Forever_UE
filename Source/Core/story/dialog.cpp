#include "dialog.h"


using namespace std;

Option::Option(Condition condition, string option, vector<Dialog> dialogs,
	vector<Change*> changes) :
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

vector<Dialog> Option::GetDialogs() const {
	return dialogs;
}

vector<Change*> Option::GetChanges() const {
	return changes;
}

Section::Section(string speaker, string content) : speaking({speaker, content}) {
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

pair<string, string> Section::GetSpeaking() const {
	return speaking;
}

vector<Option> Section::GetOptions() const {
	return options;
}

Dialog::Dialog() {

}

Dialog::~Dialog() {

}

void Dialog::AddDialog(string speaker, string content) {
	list.push_back(Section(speaker, content));
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
