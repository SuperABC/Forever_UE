#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include "condition.h"

#include <string>

#undef GetMessage


// 事件基类
class Event {
public:
	Event();
	virtual ~Event();

	// 事件类型
	virtual std::string GetType() const = 0;

	// 判断匹配
	virtual bool Match(Event* e,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) = 0;

	// 获取控制条件
	const Condition& GetCondition();

	// 设置控制条件
	void SetCondition(const Condition& condition);

private:
	Condition condition;
};


// 游戏开始（已实现）
class GameStartEvent : public Event {
public:
	GameStartEvent();
	virtual ~GameStartEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

private:
};

// 脚本消息（已实现）
class ScriptMessageEvent : public Event {
public:
	ScriptMessageEvent(std::string message);
	virtual ~ScriptMessageEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetMessage(std::string message);
	std::string GetMessage() const;

private:
	std::string message;
};

// 选项对话（已实现）
class OptionDialogEvent : public Event {
public:
	OptionDialogEvent(std::string name, std::string option);
	OptionDialogEvent(int idx, std::string option);
	virtual ~OptionDialogEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetIdx(int idx);
	int GetIdx() const;
	void SetName(std::string name);
	std::string GetName() const;
	void SetOption(std::string option);
	std::string GetOption() const;

private:
	int idx;
	std::string name;
	std::string option;
};

// NPC相遇
class NPCMeetEvent : public Event {
public:
	NPCMeetEvent(std::string npc);
	virtual ~NPCMeetEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetNPC(std::string npc);
	std::string GetNPC() const;

private:
	std::string npc;
};

// 市民出生
class CitizenBornEvent : public Event {
public:
	CitizenBornEvent(std::string name);
	virtual ~CitizenBornEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetName(std::string name);
	std::string GetName() const;

private:
	std::string name;
};

// 市民死亡
class CitizenDeceaseEvent : public Event {
public:
	CitizenDeceaseEvent(std::string name, std::string reason);
	virtual ~CitizenDeceaseEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetName(std::string name);
	std::string GetName() const;
	void SetReason(std::string result);
	std::string GetReason() const;

private:
	std::string name;
	std::string reason;
};

// 进入园区（已实现）
class EnterZoneEvent : public Event {
public:
	EnterZoneEvent(std::string zone);
	virtual ~EnterZoneEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetZone(std::string zone);
	std::string GetZone() const;

private:
	std::string zone;
};

// 离开园区（已实现）
class LeaveZoneEvent : public Event {
public:
	LeaveZoneEvent(std::string zone);
	virtual ~LeaveZoneEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetZone(std::string zone);
	std::string GetZone() const;

private:
	std::string zone;
};

// 进入建筑（已实现）
class EnterBuildingEvent : public Event {
public:
	EnterBuildingEvent(std::string zone, std::string building);
	virtual ~EnterBuildingEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetZone(std::string zone);
	std::string GetZone() const;
	void SetBuilding(std::string building);
	std::string GetBuilding() const;

private:
	std::string zone;
	std::string building;
};

// 离开建筑（已实现）
class LeaveBuildingEvent : public Event {
public:
	LeaveBuildingEvent(std::string zone, std::string building);
	virtual ~LeaveBuildingEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetZone(std::string zone);
	std::string GetZone() const;
	void SetBuilding(std::string building);
	std::string GetBuilding() const;

private:
	std::string zone;
	std::string building;
};

// 进入房间（已实现）
class EnterRoomEvent : public Event {
public:
	EnterRoomEvent(std::string zone, std::string building, std::string room);
	virtual ~EnterRoomEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetZone(std::string zone);
	std::string GetZone() const;
	void SetBuilding(std::string building);
	std::string GetBuilding() const;
	void SetRoom(std::string room);
	std::string GetRoom() const;

private:
	std::string zone;
	std::string building;
	std::string room;
};

// 离开房间（已实现）
class LeaveRoomEvent : public Event {
public:
	LeaveRoomEvent(std::string zone, std::string building, std::string room);
	virtual ~LeaveRoomEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetZone(std::string zone);
	std::string GetZone() const;
	void SetBuilding(std::string building);
	std::string GetBuilding() const;
	void SetRoom(std::string room);
	std::string GetRoom() const;

private:
	std::string zone;
	std::string building;
	std::string room;
};

// 存款变化
class DepositChangeEvent : public Event {
public:
	DepositChangeEvent(int result, int delta);
	virtual ~DepositChangeEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetResult(int result);
	int GetResult();
	void SetDelta(int delta);
	int GetDelta();

private:
	int result;
	int delta;
};

// 现金变化
class CashChangeEvent : public Event {
public:
	CashChangeEvent(int result, int delta);
	virtual ~CashChangeEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetResult(int result);
	int GetResult();
	void SetDelta(int delta);
	int GetDelta();

private:
	int result;
	int delta;
};

// 获取物品
class GetItemEvent : public Event {
public:
	GetItemEvent(std::string item, int num);
	virtual ~GetItemEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetItem(std::string item);
	std::string GetItem() const;
	void SetNum(int num);
	int GetNum();

private:
	std::string item;
	int num;
};

// 失去物品
class LoseItemEvent : public Event {
public:
	LoseItemEvent(std::string item, int num);
	virtual ~LoseItemEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetItem(std::string item);
	std::string GetItem() const;
	void SetNum(int num);
	int GetNum();

private:
	std::string item;
	int num;
};

// 受伤
class PlayerInjuredEvent : public Event {
public:
	PlayerInjuredEvent(std::string wound);
	virtual ~PlayerInjuredEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetWound(std::string wound);
	std::string GetWound() const;

private:
	std::string wound;
};

// 痊愈
class PlayerCuredEvent : public Event {
public:
	PlayerCuredEvent(std::string wound);
	virtual ~PlayerCuredEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetWound(std::string wound);
	std::string GetWound() const;

private:
	std::string wound;
};

// 生病
class PlayerIllEvent : public Event {
public:
	PlayerIllEvent(std::string illness);
	virtual ~PlayerIllEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetIllness(std::string illness);
	std::string GetIllness() const;

private:
	std::string illness;
};

// 康复
class PlayerRecoverEvent : public Event {
public:
	PlayerRecoverEvent(std::string illness);
	virtual ~PlayerRecoverEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetIllness(std::string illness);
	std::string GetIllness() const;

private:
	std::string illness;
};

// 短暂休息
class PlayerRestEvent : public Event {
public:
	PlayerRestEvent(int minute);
	virtual ~PlayerRestEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetMinute(int minute);
	int GetMinute();

private:
	int minute;
};

// 睡觉
class PlayerSleepEvent : public Event {
public:
	PlayerSleepEvent(int hour);
	virtual ~PlayerSleepEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetHour(int hour);
	int GetHour();

private:
	int hour;
};

// 计时器
class TimeUpEvent : public Event {
public:
	TimeUpEvent(std::string timer);
	virtual ~TimeUpEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetTimer(std::string timer);
	std::string GetTimer() const;

private:
	std::string timer;
};

// 计数器
class CountUpEvent : public Event {
public:
	CountUpEvent(std::string counter);
	virtual ~CountUpEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetCounter(std::string counter);
	std::string GetCounter() const;

private:
	std::string counter;
};

// 使用技能
class UseSkillEvent : public Event {
public:
	UseSkillEvent(std::string skill);
	virtual ~UseSkillEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetSkill(std::string skill);
	std::string GetSkill() const;

private:
	std::string skill;
};

// 技能变化
class SkillChangeEvent : public Event {
public:
	SkillChangeEvent(std::string skill, int level);
	virtual ~SkillChangeEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetSkill(std::string skill);
	std::string GetSkill() const;
	void SetLevel(int level);
	int GetLevel();

private:
	std::string skill;
	int level;
};

// 战斗胜利
class BattleWinEvent : public Event {
public:
	BattleWinEvent(std::string enemy);
	virtual ~BattleWinEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetEnemy(std::string enemy);
	std::string GetEnemy() const;

private:
	std::string enemy;
};

// 战斗失败
class BattleLoseEvent : public Event {
public:
	BattleLoseEvent(std::string enemy);
	virtual ~BattleLoseEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetEnemy(std::string enemy);
	std::string GetEnemy() const;

private:
	std::string enemy;
};

// 逃跑成功
class EscapeSuccessEvent : public Event {
public:
	EscapeSuccessEvent(std::string enemy);
	virtual ~EscapeSuccessEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetEnemy(std::string enemy);
	std::string GetEnemy() const;

private:
	std::string enemy;
};

// 逃跑失败
class EscapeFailEvent : public Event {
public:
	EscapeFailEvent(std::string enemy);
	virtual ~EscapeFailEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetEnemy(std::string enemy);
	std::string GetEnemy() const;

private:
	std::string enemy;
};

// 修炼变化
class CultivationChangeEvent : public Event {
public:
	CultivationChangeEvent(std::string method, int level);
	virtual ~CultivationChangeEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetMethod(std::string method);
	std::string GetMethod() const;
	void SetLevel(int level);
	int GetLevel();

private:
	std::string method;
	int level;
};

// 通缉变化
class WantedChangeEvent : public Event {
public:
	WantedChangeEvent(std::string reason, int level);
	virtual ~WantedChangeEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetReason(std::string reason);
	std::string GetReason() const;
	void SetLevel(int level);
	int GetLevel();

private:
	std::string reason;
	int level;
};

// 被捕
class PlayerArrestedEvent : public Event {
public:
	PlayerArrestedEvent(std::string reason);
	virtual ~PlayerArrestedEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetReason(std::string reason);
	std::string GetReason() const;

private:
	std::string reason;
};

// 释放
class PlayerReleasedEvent : public Event {
public:
	PlayerReleasedEvent(std::string reason);
	virtual ~PlayerReleasedEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetReason(std::string reason);
	std::string GetReason() const;

private:
	std::string reason;
};

// 天气变化
class WeatherChangeEvent : public Event {
public:
	WeatherChangeEvent(std::string weather);
	virtual ~WeatherChangeEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetWeather(std::string weather);
	std::string GetWeather() const;

private:
	std::string weather;
};

// 政策变化
class PolicyChangeEvent : public Event {
public:
	PolicyChangeEvent(std::string policy, bool status);
	virtual ~PolicyChangeEvent();

	virtual std::string GetType() const;

	virtual bool Match(Event* e,
			std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) override;

	void SetPolicy(std::string policy);
	std::string GetPolicy() const;
	void SetStatus(bool status);
	bool GetStatus();

private:
	std::string policy;
	bool status;
};
