#pragma once

#include "../populace/person.h"
#include "event_base.h"
#include "condition.h"

#include <string>
#include <vector>


enum GENDER_TYPE : int;

// 子类注册函数
typedef void (*RegisterModEventsFunc)(EventFactory* factory);

// 游戏开始
class GameStartEvent : public Event {
public:
    GameStartEvent();
    virtual ~GameStartEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

private:
};

// 游戏成功
class GameSuccessEvent : public Event {
public:
    GameSuccessEvent(std::string result);
    virtual ~GameSuccessEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetResult(std::string result);
    std::string GetResult();

private:
    std::string result;
};

// 游戏失败
class GameFailEvent : public Event {
public:
    GameFailEvent(std::string result);
    virtual ~GameFailEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetResult(std::string result);
    std::string GetResult();

private:
    std::string result;
};

// 选项对话
class OptionDialogEvent : public Event {
public:
    OptionDialogEvent(std::string target, std::string option);
    OptionDialogEvent(int idx, std::string option);
    virtual ~OptionDialogEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetIdx(int idx);
    int GetIdx() const;
    void SetTarget(std::string target);
    std::string GetTarget() const;
    void SetOption(std::string option);
    std::string GetOption() const;

private:
    int idx;
    std::string target;
    std::string option;
};

// NPC相遇
class NPCMeetEvent : public Event {
public:
    NPCMeetEvent(std::string npc);
    virtual ~NPCMeetEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetNPC(std::string npc);
    std::string GetNPC() const;

private:
    std::string npc;
};

// 市民出生
class CitizenBornEvent : public Event {
public:
    CitizenBornEvent(std::string name, GENDER_TYPE gender);
    virtual ~CitizenBornEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetName(std::string result);
    std::string GetName();
    void SetGender(GENDER_TYPE gender);
    std::string GetGender();

private:
    std::string name;
    GENDER_TYPE gender;
};

// 市民死亡
class CitizenDeceaseEvent : public Event {
public:
    CitizenDeceaseEvent(std::string name, std::string reason);
    virtual ~CitizenDeceaseEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetName(std::string result);
    std::string GetName();
    void SetReason(std::string result);
    std::string GetReason();

private:
    std::string name;
    std::string reason;
};

// 进入园区
class EnterZoneEvent : public Event {
public:
    EnterZoneEvent(std::string zone);
    virtual ~EnterZoneEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetZone(std::string zone);
    std::string GetZone();

private:
    std::string zone;
};

// 离开园区
class LeaveZoneEvent : public Event {
public:
    LeaveZoneEvent(std::string zone);
    virtual ~LeaveZoneEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetZone(std::string zone);
    std::string GetZone();

private:
    std::string zone;
};

// 进入建筑
class EnterBuildingEvent : public Event {
public:
    EnterBuildingEvent(std::string zone, std::string building);
    virtual ~EnterBuildingEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetZone(std::string zone);
    std::string GetZone();
    void SetBuilding(std::string building);
    std::string GetBuilding();

private:
    std::string zone;
    std::string building;
};

// 离开建筑
class LeaveBuildingEvent : public Event {
public:
    LeaveBuildingEvent(std::string zone, std::string building);
    virtual ~LeaveBuildingEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetZone(std::string zone);
    std::string GetZone();
    void SetBuilding(std::string building);
    std::string GetBuilding();

private:
    std::string zone;
    std::string building;
};

// 进入房间
class EnterRoomEvent : public Event {
public:
    EnterRoomEvent(std::string zone, std::string building, std::string room);
    virtual ~EnterRoomEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetZone(std::string zone);
    std::string GetZone();
    void SetBuilding(std::string building);
    std::string GetBuilding();
    void SetRoom(std::string room);
    std::string GetRoom();

private:
    std::string zone;
    std::string building;
    std::string room;
};

// 离开房间
class LeaveRoomEvent : public Event {
public:
    LeaveRoomEvent(std::string zone, std::string building, std::string room);
    virtual ~LeaveRoomEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetZone(std::string zone);
    std::string GetZone();
    void SetBuilding(std::string building);
    std::string GetBuilding();
    void SetRoom(std::string room);
    std::string GetRoom();

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

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

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

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

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

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetItem(std::string item);
    std::string GetItem();
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

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetItem(std::string item);
    std::string GetItem();
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

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetWound(std::string wound);
    std::string GetWound();

private:
    std::string wound;
};

// 痊愈
class PlayerCuredEvent : public Event {
public:
    PlayerCuredEvent(std::string wound);
    virtual ~PlayerCuredEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetWound(std::string wound);
    std::string GetWound();

private:
    std::string wound;
};

// 生病
class PlayerIllEvent : public Event {
public:
    PlayerIllEvent(std::string illness);
    virtual ~PlayerIllEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetIllness(std::string illness);
    std::string GetIllness();

private:
    std::string illness;
};

// 康复
class PlayerRecoverEvent : public Event {
public:
    PlayerRecoverEvent(std::string illness);
    virtual ~PlayerRecoverEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetIllness(std::string illness);
    std::string GetIllness();

private:
    std::string illness;
};

// 短暂休息
class PlayerRestEvent : public Event {
public:
    PlayerRestEvent(int minute);
    virtual ~PlayerRestEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

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

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

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

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetTimer(std::string timer);
    std::string GetTimer();

private:
    std::string timer;
};

// 计数器
class CountUpEvent : public Event {
public:
    CountUpEvent(std::string counter);
    virtual ~CountUpEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetCounter(std::string counter);
    std::string GetCounter();

private:
    std::string counter;
};

// 使用技能
class UseSkillEvent : public Event {
public:
    UseSkillEvent(std::string skill);
    virtual ~UseSkillEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetSkill(std::string skill);
    std::string GetSkill();

private:
    std::string skill;
};

// 技能变化
class SkillChangeEvent : public Event {
public:
    SkillChangeEvent(std::string skill, int level);
    virtual ~SkillChangeEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetSkill(std::string skill);
    std::string GetSkill();
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

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetEnemy(std::string enemy);
    std::string GetEnemy();

private:
    std::string enemy;
};

// 战斗失败
class BattleLoseEvent : public Event {
public:
    BattleLoseEvent(std::string enemy);
    virtual ~BattleLoseEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetEnemy(std::string enemy);
    std::string GetEnemy();

private:
    std::string enemy;
};

// 逃跑成功
class EscapeSuccessEvent : public Event {
public:
    EscapeSuccessEvent(std::string enemy);
    virtual ~EscapeSuccessEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetEnemy(std::string enemy);
    std::string GetEnemy();

private:
    std::string enemy;
};

// 逃跑失败
class EscapeFailEvent : public Event {
public:
    EscapeFailEvent(std::string enemy);
    virtual ~EscapeFailEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetEnemy(std::string enemy);
    std::string GetEnemy();

private:
    std::string enemy;
};

// 修炼变化
class CultivationChangeEvent : public Event {
public:
    CultivationChangeEvent(std::string method, int level);
    virtual ~CultivationChangeEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetMethod(std::string method);
    std::string GetMethod();
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

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetReason(std::string reason);
    std::string GetReason();
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

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetReason(std::string reason);
    std::string GetReason();

private:
    std::string reason;
};

// 释放
class PlayerReleasedEvent : public Event {
public:
    PlayerReleasedEvent(std::string reason);
    virtual ~PlayerReleasedEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetReason(std::string reason);
    std::string GetReason();

private:
    std::string reason;
};

// 天气变化
class WeatherChangeEvent : public Event {
public:
    WeatherChangeEvent(std::string weather);
    virtual ~WeatherChangeEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetWeather(std::string weather);
    std::string GetWeather();

private:
    std::string weather;
};

// 政策变化
class PolicyChangeEvent : public Event {
public:
    PolicyChangeEvent(std::string policy, bool status);
    virtual ~PolicyChangeEvent();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual bool operator==(Event* e) override;

    void SetPolicy(std::string policy);
    std::string GetPolicy();
    void SetStatus(bool status);
    bool GetStatus();

private:
    std::string policy;
    bool status;
};
