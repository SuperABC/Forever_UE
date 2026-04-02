#pragma once

#include "condition.h"

#include "../common/utility.h"
#include "../common/error.h"

#include <string>

#undef GetMessage


// 变化基类
class Change {
public:
	Change();
	virtual ~Change();

	// 变化类型
	virtual std::string GetType() const = 0;

	// 获取控制条件
	const Condition& GetCondition();

	// 设置控制条件
	void SetCondition(const Condition& condition);

private:
	Condition condition;
};

// 全局广播（已实现）
class GlobalMessageChange : public Change {
public:
	GlobalMessageChange();
	GlobalMessageChange(std::string message);
	virtual ~GlobalMessageChange();

	virtual std::string GetType() const;

	void SetMessage(std::string variable);
	std::string GetMessage() const;

private:
	std::string message;
};

// 变量赋值（已实现）
class SetValueChange : public Change {
public:
	SetValueChange();
	SetValueChange(std::string variable, std::string value);
	virtual ~SetValueChange();

	virtual std::string GetType() const;

	void SetVariable(std::string variable);
	std::string GetVariable() const;
	void SetValue(std::string value);
	std::string GetValue() const;

private:
	std::string variable;
	std::string value;
};

// 移除变量（已实现）
class RemoveValueChange : public Change {
public:
	RemoveValueChange();
	RemoveValueChange(std::string variable);
	virtual ~RemoveValueChange();

	virtual std::string GetType() const;

	void SetVariable(std::string variable);
	std::string GetVariable() const;

private:
	std::string variable;
};

// 停用里程碑（已实现）
class DeactivateMilestoneChange : public Change {
public:
	DeactivateMilestoneChange();
	DeactivateMilestoneChange(std::string milestone);
	virtual ~DeactivateMilestoneChange();

	virtual std::string GetType() const;

	void SetMilestone(std::string milestone);
	std::string GetMilestone() const;

private:
	std::string milestone;
};

// 添加选项
class AddOptionChange : public Change {
public:
	AddOptionChange();
	AddOptionChange(std::string name, std::string option);
	virtual ~AddOptionChange();

	virtual std::string GetType() const;

	void SetName(std::string name);
	std::string GetName() const;
	void SetOption(std::string option);
	std::string GetOption() const;

private:
	std::string name;
	std::string option;
};

// 移除选项
class RemoveOptionChange : public Change {
public:
	RemoveOptionChange();
	RemoveOptionChange(std::string name, std::string option);
	virtual ~RemoveOptionChange();

	virtual std::string GetType() const;

	void SetName(std::string name);
	std::string GetName() const;
	void SetOption(std::string option);
	std::string GetOption() const;

private:
	std::string name;
	std::string option;
};

// 生成NPC
class SpawnNpcChange : public Change {
public:
	SpawnNpcChange();
	SpawnNpcChange(std::string name, std::string gender, std::string birthday, std::string avatar);
	virtual ~SpawnNpcChange();

	virtual std::string GetType() const;

	void SetName(std::string name);
	std::string GetName() const;
	void SetGender(std::string gender);
	std::string GetGender();
	void SetBirthday(std::string birthday);
	std::string GetBirthday();
	void SetAvatar(std::string avatar);
	std::string GetAvatar() const;

private:
	std::string name;
	std::string gender;
	std::string birthday;
	std::string avatar;
};

// 移除NPC
class RemoveNpcChange : public Change {
public:
	RemoveNpcChange();
	RemoveNpcChange(std::string name);
	virtual ~RemoveNpcChange();

	virtual std::string GetType() const;

	void SetName(std::string name);
	std::string GetName() const;

private:
	std::string name;
};

// NPC自动导航
class NPCNavigateChange : public Change {
public:
	NPCNavigateChange();
	NPCNavigateChange(std::string name, std::string destination);
	virtual ~NPCNavigateChange();

	virtual std::string GetType() const;

	void SetName(std::string name);
	std::string GetName() const;
	void SetDestination(std::string destination);
	std::string GetDestination() const;

private:
	std::string name;
	std::string destination;
};

// 瞬移市民
class TeleportCitizenChange : public Change {
public:
	TeleportCitizenChange();
	TeleportCitizenChange(std::string name, std::string destination);
	virtual ~TeleportCitizenChange();

	virtual std::string GetType() const;

	void SetName(std::string name);
	std::string GetName() const;
	void SetDestination(std::string destination);
	std::string GetDestination() const;

private:
	std::string name;
	std::string destination;
};

// 瞬移角色
class TeleportPlayerChange : public Change {
public:
	TeleportPlayerChange();
	TeleportPlayerChange(std::string destination);
	virtual ~TeleportPlayerChange();

	virtual std::string GetType() const;

	void SetDestination(std::string destination);
	std::string GetDestination() const;

private:
	std::string destination;
};

// 存款收支
class BankTransactionChange : public Change {
public:
	BankTransactionChange();
	BankTransactionChange(int amount);
	virtual ~BankTransactionChange();

	virtual std::string GetType() const;

	void SetAmount(int amount);
	std::string GetAmount() const;

private:
	int amount;
};

// 现金收支
class CashTransactionChange : public Change {
public:
	CashTransactionChange();
	CashTransactionChange(int amount);
	virtual ~CashTransactionChange();

	virtual std::string GetType() const;

	void SetAmount(int amount);
	std::string GetAmount() const;

private:
	int amount;
};

// 给予物品
class GiveItemChange : public Change {
public:
	GiveItemChange();
	GiveItemChange(std::string item, int num);
	virtual ~GiveItemChange();

	virtual std::string GetType() const;

	void SetItem(std::string item);
	std::string GetItem() const;
	void SetNum(int num);
	int GetNum();

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

	virtual std::string GetType() const;

	void SetItem(std::string item);
	std::string GetItem() const;
	void SetNum(int num);
	int GetNum();

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

	virtual std::string GetType() const;

	void SetWound(std::string wound);
	std::string GetWound() const;

private:
	std::string wound;
};

// 痊愈
class PlayerCuredChange : public Change {
public:
	PlayerCuredChange();
	PlayerCuredChange(std::string wound);
	virtual ~PlayerCuredChange();

	virtual std::string GetType() const;

	void SetWound(std::string wound);
	std::string GetWound() const;

private:
	std::string wound;
};

// 生病
class PlayerIllChange : public Change {
public:
	PlayerIllChange();
	PlayerIllChange(std::string illness);
	virtual ~PlayerIllChange();

	virtual std::string GetType() const;

	void SetIllness(std::string illness);
	std::string GetIllness() const;

private:
	std::string illness;
};

// 康复
class PlayerRecoverChange : public Change {
public:
	PlayerRecoverChange();
	PlayerRecoverChange(std::string illness);
	virtual ~PlayerRecoverChange();

	virtual std::string GetType() const;

	void SetIllness(std::string illness);
	std::string GetIllness() const;

private:
	std::string illness;
};

// 强制睡眠
class PlayerSleepChange : public Change {
public:
	PlayerSleepChange();
	PlayerSleepChange(int hour);
	virtual ~PlayerSleepChange();

	virtual std::string GetType() const;

	void SetHour(int hour);
	int GetHour();

private:
	int hour;
};

// 创建计时器
class CreateTimerChange : public Change {
public:
	CreateTimerChange();
	CreateTimerChange(std::string timer, Time duration, int loop = 1);
	virtual ~CreateTimerChange();

	virtual std::string GetType() const;

	void SetTimer(std::string timer);
	std::string GetTimer() const;
	void SetDuration(Time duration);
	Time GetDuration();
	void SetLoop(int loop);
	int GetLoop();

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

	virtual std::string GetType() const;

	void SetTimer(std::string timer);
	std::string GetTimer() const;

private:
	std::string timer;
};

// 恢复计时器
class ResumeTimerChange : public Change {
public:
	ResumeTimerChange();
	ResumeTimerChange(std::string timer);
	virtual ~ResumeTimerChange();

	virtual std::string GetType() const;

	void SetTimer(std::string timer);
	std::string GetTimer() const;

private:
	std::string timer;
};

// 删除计时器
class RemoveTimerChange : public Change {
public:
	RemoveTimerChange();
	RemoveTimerChange(std::string timer);
	virtual ~RemoveTimerChange();

	virtual std::string GetType() const;

	void SetTimer(std::string timer);
	std::string GetTimer() const;

private:
	std::string timer;
};

// 重置计时器
class ResetTimerChange : public Change {
public:
	ResetTimerChange();
	ResetTimerChange(std::string timer);
	virtual ~ResetTimerChange();

	virtual std::string GetType() const;

	void SetTimer(std::string timer);
	std::string GetTimer() const;

private:
	std::string timer;
};

// 创建计数器
class CreateCounterChange : public Change {
public:
	CreateCounterChange();
	CreateCounterChange(std::string counter, int count);
	virtual ~CreateCounterChange();

	virtual std::string GetType() const;

	void SetCounter(std::string counter);
	std::string GetCounter() const;
	void SetCount(int count);
	int GetCount();

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

	virtual std::string GetType() const;

	void SetCounter(std::string counter);
	std::string GetCounter() const;
	void SetDelta(int delta);
	int GetDelta();

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

	virtual std::string GetType() const;

	void SetCounter(std::string counter);
	std::string GetCounter() const;

private:
	std::string counter;
};

// 进入战斗
class EnterBattleChange : public Change {
public:
	EnterBattleChange();
	EnterBattleChange(std::string enemy);
	virtual ~EnterBattleChange();

	virtual std::string GetType() const;

	void SetEnemy(std::string enemy);
	std::string GetEnemy() const;

private:
	std::string enemy;
};

// 变化时间
class ChangeTimeChange : public Change {
public:
	ChangeTimeChange();
	ChangeTimeChange(Time delta);
	virtual ~ChangeTimeChange();

	virtual std::string GetType() const;

	void SetDelta(Time delta);
	Time GetDelta();

private:
	Time delta;
};

// 变化修炼
class ChangeCultivationChange : public Change {
public:
	ChangeCultivationChange();
	ChangeCultivationChange(std::string method, int level);
	virtual ~ChangeCultivationChange();

	virtual std::string GetType() const;

	void SetMethod(std::string method);
	std::string GetMethod() const;
	void SetLevel(int level);
	int GetLevel();

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

	virtual std::string GetType() const;

	void SetReason(std::string reason);
	std::string GetReason() const;
	void SetLevel(int level);
	int GetLevel();

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

	virtual std::string GetType() const;

	void SetWeather(std::string weather);
	std::string GetWeather() const;

private:
	std::string weather;
};

// 变化政策
class ChangePolicyChange : public Change {
public:
	ChangePolicyChange();
	ChangePolicyChange(std::string policy);
	virtual ~ChangePolicyChange();

	virtual std::string GetType() const;

	void SetPolicy(std::string policy);
	std::string GetPolicy() const;

private:
	std::string policy;
};

