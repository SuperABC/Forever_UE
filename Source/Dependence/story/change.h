#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include "condition.h"

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

// 范围循环
class ForRangeChange : public Change {
public:
	ForRangeChange(std::string var, std::string from, std::string to, std::string step,
		std::vector<Change*> changes);
	virtual ~ForRangeChange();

	virtual std::string GetType() const;

	std::string GetVar() const;
	std::string GetFrom() const;
	std::string GetTo() const;
	std::string GetStep() const;
	const std::vector<Change*>& GetChanges() const;

private:
	std::string var;
	std::string from;
	std::string to;
	std::string step;
	std::vector<Change*> changes;
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

// 游戏结束（已实现）
class GameEndChange : public Change {
public:
	GameEndChange();
	virtual ~GameEndChange();

	virtual std::string GetType() const;

private:
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

// 添加选项（已实现）
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

// 移除选项（已实现）
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

// 生成NPC（已实现）
class SpawnNpcChange : public Change {
public:
	SpawnNpcChange();
	SpawnNpcChange(std::string avatar, std::string name, std::string gender, std::string birthday, float height, float weight,
		std::string nick, int deposit, int phone, std::string home, std::vector<std::string> jobs, std::string scheduler);
	virtual ~SpawnNpcChange();

	virtual std::string GetType() const;

	void SetAvatar(std::string avatar);
	std::string GetAvatar() const;
	void SetName(std::string name);
	std::string GetName() const;
	void SetGender(std::string gender);
	std::string GetGender();
	void SetBirthday(std::string birthday);
	std::string GetBirthday();
	void SetHeight(float height);
	float GetHeight() const;
	void SetWeight(float weight);
	float GetWeight() const;
	void SetNick(std::string nick);
	std::string GetNick() const;
	void SetDeposit(int deposit);
	int GetDeposit() const;
	void SetPhone(int phone);
	int GetPhone() const;
	void SetHome(std::string home);
	std::string GetHome() const;
	void SetJobs(std::vector<std::string> jobs);
	std::vector<std::string> GetJobs() const;
	void SetScheduler(std::string scheduler);
	std::string GetScheduler() const;

private:
	std::string avatar;
	std::string name;
	std::string gender;
	std::string birthday;
	float height;
	float weight;
	std::string nick;
	int deposit;
	int phone;
	std::string home;
	std::vector<std::string> jobs;
	std::string scheduler;
};

// 移除NPC（已实现）
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

// 瞬移市民（已实现）
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

// NPC自动导航（已实现）
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

// 瞬移角色（已实现）
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

// 打开商店（已实现）
class OpenShopChange : public Change {
public:
	OpenShopChange();
	OpenShopChange(std::string saler);
	virtual ~OpenShopChange();

	virtual std::string GetType() const;

	void SetSaler(std::string saler);
	std::string GetSaler() const;

private:
	std::string saler;
};

// 启动小游戏（已实现）
class StartPuzzleChange : public Change {
public:
	StartPuzzleChange();
	StartPuzzleChange(std::string puzzle);
	virtual ~StartPuzzleChange();

	virtual std::string GetType() const;

	void SetPuzzle(std::string puzzle);
	std::string GetPuzzle() const;

private:
	std::string puzzle;
};

// 进入载具（已实现）
class EnterVehicleChange : public Change {
public:
	EnterVehicleChange();
	EnterVehicleChange(std::string vehicle);
	virtual ~EnterVehicleChange();

	virtual std::string GetType() const;

	void SetVehicle(std::string vehicle);
	std::string GetVehicle() const;

private:
	std::string vehicle;
};

// 离开载具（已实现）
class LeaveVehicleChange : public Change {
public:
	LeaveVehicleChange();
	LeaveVehicleChange(std::string vehicle);
	virtual ~LeaveVehicleChange();

	virtual std::string GetType() const;

	void SetVehicle(std::string vehicle);
	std::string GetVehicle() const;

private:
	std::string vehicle;
};

// 创建计时器（已实现）
class CreateTimerChange : public Change {
public:
	CreateTimerChange();
	CreateTimerChange(std::string name, std::string time, std::string category, std::string label);
	virtual ~CreateTimerChange();

	virtual std::string GetType() const;

	void SetName(std::string name);
	std::string GetName() const;
	void SetTime(std::string time);
	std::string GetTime();
	void SetCategory(std::string category);
	std::string GetCategory() const;
	void SetLabel(std::string label);
	std::string GetLabel() const;

private:
	std::string name;
	std::string time;
	std::string category;
	std::string label;
};

// 启动电梯（已实现）
class LaunchElevatorChange : public Change {
public:
	LaunchElevatorChange();
	LaunchElevatorChange(std::string building, std::string elevator, std::string command);
	virtual ~LaunchElevatorChange();

	virtual std::string GetType() const;

	void SetBuilding(std::string building);
	std::string GetBuilding() const;
	void SetElevator(std::string elevator);
	std::string GetElevator() const;
	void SetCommand(std::string command);
	std::string GetCommand() const;

private:
	std::string building;
	std::string elevator;
	std::string command;
};

// 存款收支
class BankTransactionChange : public Change {
public:
	BankTransactionChange();
	BankTransactionChange(std::string name, int amount);
	virtual ~BankTransactionChange();

	virtual std::string GetType() const;

	void SetName(std::string name);
	std::string GetName() const;
	void SetAmount(int amount);
	int GetAmount() const;

private:
	std::string name;
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

using ChangeValue = std::variant<
	GlobalMessageChange,
	GameEndChange,
	SetValueChange,
	RemoveValueChange,
	DeactivateMilestoneChange,
	AddOptionChange,
	RemoveOptionChange,
	SpawnNpcChange,
	RemoveNpcChange,
	TeleportCitizenChange,
	NPCNavigateChange,
	TeleportPlayerChange,
	OpenShopChange,
	StartPuzzleChange,
	EnterVehicleChange,
	LeaveVehicleChange,
	CreateTimerChange,
	LaunchElevatorChange,
	BankTransactionChange,
	GiveItemChange,
	RemoveItemChange,
	EnterBattleChange,
	PlayerInjuredChange,
	PlayerCuredChange,
	PlayerIllChange,
	PlayerRecoverChange,
	PlayerSleepChange,
	ChangeTimeChange,
	ChangeCultivationChange,
	ChangeWantedChange,
	ChangeWeatherChange,
	ChangePolicyChange
>;
