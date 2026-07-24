#include "milestone.h"

#include "story/event.h"
#include "story/dialog.h"
#include "story/change.h"


using namespace std;

Milestone::Milestone(string name, vector<Event*> triggers, bool visible, Condition drop, string description,
	string goal, vector<Dialog*> dialogs, vector<Change*> changes, vector<string> subsequences) :
	name(name), triggers(triggers), visible(visible), drop(drop), description(description),
	goal(goal), dialogs(dialogs), changes(changes), subsequences(subsequences) {

}

Milestone::~Milestone() {
	for (auto &trigger : triggers) {
		delete trigger;
	}
	triggers.clear();
	for (auto &dialog : dialogs) {
		delete dialog;
	}
	dialogs.clear();
	for (auto &change : changes) {
		delete change;
	}
	changes.clear();
}

vector<Dialog*> Milestone::GetDialogs() const {
	return dialogs;
}

vector<Change*> Milestone::GetChanges() const {
	return changes;
}

vector<Event*> Milestone::GetTriggers() const {
	return triggers;
}

bool Milestone::MatchTrigger(Event* e,
	const vector<function<pair<bool, ValueType>(const string&)>>& getValues) const {
	if (triggers.size() <= 0)return false;
	if (!e)return false;

	for (auto trigger : triggers) {
		if (!trigger) continue;
		if (trigger->GetType() != e->GetType()) continue;
		if (trigger->Match(e, getValues)) {
			return true;
		}
	}

	return false;
}

string Milestone::GetName() const {
	return name;
}

bool Milestone::IsVisible() const {
	return visible;
}

Condition Milestone::DropCondition() const {
	return drop;
}

bool Milestone::DropSelf(const vector<function<pair<bool, ValueType>(const string&)>>& getValues) const {
	return drop.EvaluateBool(getValues);
}

string Milestone::GetDescription() const {
	return description;
}

string Milestone::GetGoal() const {
	return goal;
}

vector<string> Milestone::GetSubsequences() const {
	return subsequences;
}

MilestoneNode::MilestoneNode() :
	content(nullptr),
	premise(0) {

}

MilestoneNode::MilestoneNode(Milestone* milestone) :
	content(milestone),
	premise(0) {

}

