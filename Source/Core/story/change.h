#pragma once

#include "change_base.h"
#include "condition.h"

#include <string>
#include <vector>


// 子类注册函数
typedef void (*RegisterModChangesFunc)(ChangeFactory* factory);

// 变量赋值
class SetValueChange : public Change {
public:
    SetValueChange();
    SetValueChange(std::string variable, std::string value);
    virtual ~SetValueChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetVariable(std::string variable);
    std::string GetVariable();
    void SetValue(std::string value);
    std::string GetValue();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string variable;
    std::string value;
};

// 移除变量
class RemoveValueChange : public Change {
public:
    RemoveValueChange();
    RemoveValueChange(std::string variable);
    virtual ~RemoveValueChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetVariable(std::string variable);
    std::string GetVariable();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string variable;
};

// 停用里程碑
class DeactivateMilestoneChange : public Change {
public:
    DeactivateMilestoneChange();
    DeactivateMilestoneChange(std::string milestone);
    virtual ~DeactivateMilestoneChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetMilestone(std::string milestone);
    std::string GetMilestone();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string milestone;
};

// 添加选项
class AddOptionChange : public Change {
public:
    AddOptionChange();
    AddOptionChange(std::string target, std::string option);
    virtual ~AddOptionChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetTarget(std::string target);
    std::string GetTarget();
    void SetOption(std::string option);
    std::string GetOption();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string target;
    std::string option;
};

// 移除选项
class RemoveOptionChange : public Change {
public:
    RemoveOptionChange();
    RemoveOptionChange(std::string target, std::string option);
    virtual ~RemoveOptionChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetTarget(std::string target);
    std::string GetTarget();
    void SetOption(std::string option);
    std::string GetOption();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string target;
    std::string option;
};

// 生成NPC
class SpawnNpcChange : public Change {
public:
    SpawnNpcChange();
    SpawnNpcChange(std::string target, std::string gender, std::string birthday);
    virtual ~SpawnNpcChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetTarget(std::string target);
    std::string GetTarget();
    void SetGender(std::string gender);
    std::string GetGender();
    void SetBirthday(std::string birthday);
    std::string GetBirthday();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string target;
    std::string gender;
    std::string birthday;
};

// 移除NPC
class RemoveNpcChange : public Change {
public:
    RemoveNpcChange();
    RemoveNpcChange(std::string target);
    virtual ~RemoveNpcChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetTarget(std::string target);
    std::string GetTarget();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string target;
};

// NPC自动导航
class NPCNavigateChange : public Change {
public:
    NPCNavigateChange();
    NPCNavigateChange(std::string target, std::string destination);
    virtual ~NPCNavigateChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetTarget(std::string target);
    std::string GetTarget();
    void SetDestination(std::string destination);
    std::string GetDestination();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string target;
    std::string destination;
};

// 瞬移市民
class TeleportCitizenChange : public Change {
public:
    TeleportCitizenChange();
    TeleportCitizenChange(std::string target, std::string destination);
    virtual ~TeleportCitizenChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetTarget(std::string target);
    std::string GetTarget();
    void SetDestination(std::string destination);
    std::string GetDestination();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string target;
    std::string destination;
};

// 瞬移角色
class TeleportPlayerChange : public Change {
public:
    TeleportPlayerChange();
    TeleportPlayerChange(std::string destination);
    virtual ~TeleportPlayerChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetDestination(std::string destination);
    std::string GetDestination();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string destination;
};

// 存款收支
class BankTransactionChange : public Change {
public:
    BankTransactionChange();
    BankTransactionChange(int amount);
    virtual ~BankTransactionChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetAmount(int amount);
    std::string GetAmount();

    virtual std::vector<Change*> ApplyChange() override;

private:
    int amount;
};

// 现金收支
class CashTransactionChange : public Change {
public:
    CashTransactionChange();
    CashTransactionChange(int amount);
    virtual ~CashTransactionChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetAmount(int amount);
    std::string GetAmount();

    virtual std::vector<Change*> ApplyChange() override;

private:
    int amount;
};

// 给予物品
class GiveItemChange : public Change {
public:
    GiveItemChange();
    GiveItemChange(std::string item, int num);
    virtual ~GiveItemChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetItem(std::string item);
    std::string GetItem();
    void SetNum(int num);
    int GetNum();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string item;
    int num;
};

// 扣除物品
class RemoveItemChange : public Change {
public:
    RemoveItemChange();
    RemoveItemChange(std::string item, int num);
    virtual ~RemoveItemChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetItem(std::string item);
    std::string GetItem();
    void SetNum(int num);
    int GetNum();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string item;
    int num;
};

// 受伤
class PlayerInjuredChange : public Change {
public:
    PlayerInjuredChange();
    PlayerInjuredChange(std::string wound);
    virtual ~PlayerInjuredChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetWound(std::string wound);
    std::string GetWound();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string wound;
};

// 痊愈
class PlayerCuredChange : public Change {
public:
    PlayerCuredChange();
    PlayerCuredChange(std::string wound);
    virtual ~PlayerCuredChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetWound(std::string wound);
    std::string GetWound();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string wound;
};

// 生病
class PlayerIllChange : public Change {
public:
    PlayerIllChange();
    PlayerIllChange(std::string illness);
    virtual ~PlayerIllChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetIllness(std::string illness);
    std::string GetIllness();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string illness;
};

// 康复
class PlayerRecoverChange : public Change {
public:
    PlayerRecoverChange();
    PlayerRecoverChange(std::string illness);
    virtual ~PlayerRecoverChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetIllness(std::string illness);
    std::string GetIllness();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string illness;
};

// 强制睡眠
class PlayerSleepChange : public Change {
public:
    PlayerSleepChange();
    PlayerSleepChange(int hour);
    virtual ~PlayerSleepChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetHour(int hour);
    int GetHour();

    virtual std::vector<Change*> ApplyChange() override;

private:
    int hour;
};

// 创建计时器
class CreateTimerChange : public Change {
public:
    CreateTimerChange();
    CreateTimerChange(std::string timer, Time duration, int loop = 1);
    virtual ~CreateTimerChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetTimer(std::string timer);
    std::string GetTimer();
    void SetDuration(Time duration);
    Time GetDuration();
    void SetLoop(int loop);
    int GetLoop();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string timer;
    Time duration;
    int loop;
};

// 暂停计时器
class PauseTimerChange : public Change {
public:
    PauseTimerChange();
    PauseTimerChange(std::string timer);
    virtual ~PauseTimerChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetTimer(std::string timer);
    std::string GetTimer();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string timer;
};

// 恢复计时器
class ResumeTimerChange : public Change {
public:
    ResumeTimerChange();
    ResumeTimerChange(std::string timer);
    virtual ~ResumeTimerChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetTimer(std::string timer);
    std::string GetTimer();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string timer;
};

// 删除计时器
class RemoveTimerChange : public Change {
public:
    RemoveTimerChange();
    RemoveTimerChange(std::string timer);
    virtual ~RemoveTimerChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetTimer(std::string timer);
    std::string GetTimer();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string timer;
};

// 重置计时器
class ResetTimerChange : public Change {
public:
    ResetTimerChange();
    ResetTimerChange(std::string timer);
    virtual ~ResetTimerChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetTimer(std::string timer);
    std::string GetTimer();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string timer;
};

// 创建计数器
class CreateCounterChange : public Change {
public:
    CreateCounterChange();
    CreateCounterChange(std::string counter, int count);
    virtual ~CreateCounterChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetCounter(std::string counter);
    std::string GetCounter();
    void SetCount(int count);
    int GetCount();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string counter;
    int count;
};

// 调用计数器
class CallCounterChange : public Change {
public:
    CallCounterChange();
    CallCounterChange(std::string counter, int delta);
    virtual ~CallCounterChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetCounter(std::string counter);
    std::string GetCounter();
    void SetDelta(int delta);
    int GetDelta();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string counter;
    int delta;
};

// 删除计数器
class RemoveCounterChange : public Change {
public:
    RemoveCounterChange();
    RemoveCounterChange(std::string counter);
    virtual ~RemoveCounterChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetCounter(std::string counter);
    std::string GetCounter();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string counter;
};

// 进入战斗
class EnterBattleChange : public Change {
public:
    EnterBattleChange();
    EnterBattleChange(std::string enemy);
    virtual ~EnterBattleChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetEnemy(std::string enemy);
    std::string GetEnemy();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string enemy;
};

// 变化时间
class ChangeTimeChange : public Change {
public:
    ChangeTimeChange();
    ChangeTimeChange(Time delta);
    virtual ~ChangeTimeChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetDelta(Time delta);
    Time GetDelta();

    virtual std::vector<Change*> ApplyChange() override;

private:
    Time delta;
};

// 变化修炼
class ChangeCultivationChange : public Change {
public:
    ChangeCultivationChange();
    ChangeCultivationChange(std::string method, int level);
    virtual ~ChangeCultivationChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetMethod(std::string method);
    std::string GetMethod();
    void SetLevel(int level);
    int GetLevel();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string method;
    int level;
};

// 变化通缉
class ChangeWantedChange : public Change {
public:
    ChangeWantedChange();
    ChangeWantedChange(std::string reason, int level);
    virtual ~ChangeWantedChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetReason(std::string reason);
    std::string GetReason();
    void SetLevel(int level);
    int GetLevel();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string reason;
    int level;
};

// 变化天气
class ChangeWeatherChange : public Change {
public:
    ChangeWeatherChange();
    ChangeWeatherChange(std::string weather);
    virtual ~ChangeWeatherChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetWeather(std::string weather);
    std::string GetWeather();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string weather;
};

// 变化政策
class ChangePolicyChange : public Change {
public:
    ChangePolicyChange();
    ChangePolicyChange(std::string policy);
    virtual ~ChangePolicyChange();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    void SetPolicy(std::string policy);
    std::string GetPolicy();

    virtual std::vector<Change*> ApplyChange() override;

private:
    std::string policy;
};