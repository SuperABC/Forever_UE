#pragma once

#include "milestone.h"
#include "change.h"
#include "story.h"
#include "json.h"
#include "error.h"


class JsonValue;
class Story;
class Person;
class MilestoneNode;

class Script {
public:
	Script();
	Script(Script* script);
	~Script();

	// 读取剧本
	std::vector<std::string> ReadNames(std::string path) const;
	void ReadScript(std::string path,
		EventFactory* eventFactory, ChangeFactory* changeFactory);

	// 匹配事件
	std::pair<std::vector<Dialog>, std::vector<Change*>> MatchEvent(
		Event* event, Story* story);
	std::pair<std::vector<Dialog>, std::vector<Change*>> MatchEvent(
		Event* event, Story* story, Person* person);

	// 运行时操作
	void DeactivateMilestone(const std::string& name);

private:
	std::vector<MilestoneNode> milestones;
	std::vector<MilestoneNode*> actives;

	// 复合对象读取
	std::vector<Event*> BuildEvent(JsonValue root, EventFactory* factory) const;
	std::vector<Dialog> BuildDialogs(JsonValue root, ChangeFactory* factory) const;
	std::vector<Change*> BuildChanges(JsonValue root, ChangeFactory* factory) const;
	Condition BuildCondition(JsonValue root) const;

};