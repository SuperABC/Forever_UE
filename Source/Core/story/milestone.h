#pragma once

#include "utility.h"
#include "dialog.h"
#include "change.h"
#include "event.h"

#include <string>


class Event;
class Dialog;
class Change;

class Milestone {
public:
    Milestone(std::string name, std::vector<Event*> triggers, bool visible, Condition keep,
        std::string description, std::string goal, std::vector<Dialog> dialogs, std::vector<Change*> changes);
    ~Milestone();

    // 复合对象类型
    std::vector<Event*> GetTriggers();
    std::vector<Dialog> GetDialogs();
    std::vector<Change*> GetChanges();

    // 匹配事件
    bool MatchTrigger(Event* e);

    // 获取参数
    std::string GetName();
    bool IsVisible();
	Condition DropCondition();
    bool DropSelf(std::function<std::pair<bool, ValueType>(const std::string&)> getValue);
    std::string GetDescription();
    std::string GetGoal();

private:
    std::string name;
    std::vector<Event*> triggers;
    bool visible;
    Condition drop;
    std::string description;
    std::string goal;
    std::vector<Dialog> dialogs;
    std::vector<Change*> changes;
};

class MilestoneNode {
public:
	MilestoneNode(Milestone milestone);

	Milestone content;

    // 前置数量
	int premise = 0;

    // 后置里程碑
	std::vector<MilestoneNode *> subsequents;
};
