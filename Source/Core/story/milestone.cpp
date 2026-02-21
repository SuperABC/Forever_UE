#include "milestone.h"


using namespace std;

Milestone::Milestone(string name, vector<Event*> triggers, bool visible, Condition drop, string description,
	string goal, vector<Dialog> dialogs, vector<Change*> changes, vector<string> subsequences) :
	name(name), triggers(triggers), visible(visible), drop(drop), description(description),
	goal(goal), dialogs(dialogs), changes(changes), subsequences(subsequences) {

}

Milestone::~Milestone() {

}

vector<Dialog> Milestone::GetDialogs() {
	return dialogs;
}

vector<Change*> Milestone::GetChanges() {
	return changes;
}

vector<Event*> Milestone::GetTriggers() {
	return triggers;
}

bool Milestone::MatchTrigger(Event* e) {
	if (triggers.size() <= 0)return false;
	if (!e)return false;

	for(auto trigger : triggers) {
		if (trigger->GetType() != e->GetType())continue;
		if (trigger->operator==(e)) {
			return true;
		}
	}

	return false;
}

string Milestone::GetName() {
	return name;
}

bool Milestone::IsVisible() {
	return visible;
}

Condition Milestone::DropCondition() {
	return drop;
}

bool Milestone::DropSelf(function<pair<bool, ValueType>(const string&)> getValue) {
	return drop.EvaluateBool(getValue);
}

string Milestone::GetDescription() {
	return description;
}

string Milestone::GetGoal() {
	return goal;
}

vector<string> Milestone::GetSubsequences() {
	return subsequences;
}

MilestoneNode::MilestoneNode() : content(nullptr) {

}

MilestoneNode::MilestoneNode(Milestone* milestone) : content(milestone) {

}

