#include "../common/utility.h"
#include "../common/error.h"

#include "change.h"


using namespace std;

Change::Change() {

}

Change::~Change() {

}

const Condition& Change::GetCondition() {
	return condition;
}

void Change::SetCondition(const Condition& condition) {
	this->condition = condition;
}

GlobalMessageChange::GlobalMessageChange() {

}

GlobalMessageChange::GlobalMessageChange(string message)
	: message(message) {

}

GlobalMessageChange::~GlobalMessageChange() {

}

string GlobalMessageChange::GetType() const {
	return "get_message";
}

void GlobalMessageChange::SetMessage(string message) {
	this->message = message;
}

string GlobalMessageChange::GetMessage() const {
	return message;
}

SetValueChange::SetValueChange() {

}

SetValueChange::SetValueChange(string variable, string value)
	: variable(variable), value(value) {

}

SetValueChange::~SetValueChange() {

}

string SetValueChange::GetType() const {
	return "set_value";
}

void SetValueChange::SetVariable(string variable) {
	this->variable = variable;
}

string SetValueChange::GetVariable() const {
	return variable;
}

void SetValueChange::SetValue(string value) {
	this->value = value;
}

string SetValueChange::GetValue() const {
	return value;
}

RemoveValueChange::RemoveValueChange() {

}

RemoveValueChange::RemoveValueChange(string variable)
	: variable(variable) {

}

RemoveValueChange::~RemoveValueChange() {

}

string RemoveValueChange::GetType() const {
	return "remove_value";
}

void RemoveValueChange::SetVariable(string variable) {
	this->variable = variable;
}

string RemoveValueChange::GetVariable() const {
	return variable;
}

DeactivateMilestoneChange::DeactivateMilestoneChange() {

}

DeactivateMilestoneChange::DeactivateMilestoneChange(string milestone)
	: milestone(milestone) {

}

DeactivateMilestoneChange::~DeactivateMilestoneChange() {

}

string DeactivateMilestoneChange::GetType() const {
	return "deactivate_milestone";
}

void DeactivateMilestoneChange::SetMilestone(string milestone) {
	this->milestone = milestone;
}

string DeactivateMilestoneChange::GetMilestone() const {
	return milestone;
}

AddOptionChange::AddOptionChange() {

}

AddOptionChange::AddOptionChange(string name, string option)
	: name(name), option(option) {

}

AddOptionChange::~AddOptionChange() {

}

string AddOptionChange::GetType() const {
	return "add_option";
}

void AddOptionChange::SetName(string name) {
	this->name = name;
}

string AddOptionChange::GetName() const {
	return name;
}

void AddOptionChange::SetOption(string option) {
	this->option = option;
}

string AddOptionChange::GetOption() const {
	return option;
}

RemoveOptionChange::RemoveOptionChange() {

}

RemoveOptionChange::RemoveOptionChange(string name, string option)
	: name(name), option(option) {

}

RemoveOptionChange::~RemoveOptionChange() {

}

string RemoveOptionChange::GetType() const {
	return "remove_option";
}

void RemoveOptionChange::SetName(string name) {
	this->name = name;
}

string RemoveOptionChange::GetName() const {
	return name;
}

void RemoveOptionChange::SetOption(string option) {
	this->option = option;
}

string RemoveOptionChange::GetOption() const {
	return option;
}

SpawnNpcChange::SpawnNpcChange() {

}

SpawnNpcChange::SpawnNpcChange(string name, string avatar)
	: name(name), avatar(avatar) {

}

SpawnNpcChange::~SpawnNpcChange() {

}

string SpawnNpcChange::GetType() const {
	return "spawn_npc";
}

void SpawnNpcChange::SetName(string name) {
	this->name = name;
}

string SpawnNpcChange::GetName() const {
	return name;
}

void SpawnNpcChange::SetAvatar(string avatar) {
	this->avatar = avatar;
}

string SpawnNpcChange::GetAvatar() const {
	return avatar;
}

RemoveNpcChange::RemoveNpcChange() {

}

RemoveNpcChange::RemoveNpcChange(string name)
	: name(name) {

}

RemoveNpcChange::~RemoveNpcChange() {

}

string RemoveNpcChange::GetType() const {
	return "remove_npc";
}

void RemoveNpcChange::SetName(string name) {
	this->name = name;
}

string RemoveNpcChange::GetName() const {
	return name;
}

NPCNavigateChange::NPCNavigateChange() {

}

NPCNavigateChange::NPCNavigateChange(string name, string destination)
	: name(name), destination(destination) {

}

NPCNavigateChange::~NPCNavigateChange() {

}

string NPCNavigateChange::GetType() const {
	return "npc_navigate";
}

void NPCNavigateChange::SetName(string name) {
	this->name = name;
}

string NPCNavigateChange::GetName() const {
	return name;
}

void NPCNavigateChange::SetDestination(string destination) {
	this->destination = destination;
}

string NPCNavigateChange::GetDestination() const {
	return destination;
}

TeleportCitizenChange::TeleportCitizenChange() {

}

TeleportCitizenChange::TeleportCitizenChange(string name, string destination)
	: name(name), destination(destination) {

}

TeleportCitizenChange::~TeleportCitizenChange() {

}

string TeleportCitizenChange::GetType() const {
	return "teleport_citizen";
}

void TeleportCitizenChange::SetName(string name) {
	this->name = name;
}

string TeleportCitizenChange::GetName() const {
	return name;
}

void TeleportCitizenChange::SetDestination(string destination) {
	this->destination = destination;
}

string TeleportCitizenChange::GetDestination() const {
	return destination;
}

TeleportPlayerChange::TeleportPlayerChange() {

}

TeleportPlayerChange::TeleportPlayerChange(string destination)
	: destination(destination) {

}

TeleportPlayerChange::~TeleportPlayerChange() {

}

string TeleportPlayerChange::GetType() const {
	return "teleport_player";
}

void TeleportPlayerChange::SetDestination(string destination) {
	this->destination = destination;
}

string TeleportPlayerChange::GetDestination() const {
	return destination;
}

BankTransactionChange::BankTransactionChange() {

}

BankTransactionChange::BankTransactionChange(int amount)
	: amount(amount) {

}

BankTransactionChange::~BankTransactionChange() {

}

string BankTransactionChange::GetType() const {
	return "bank_transaction";
}

void BankTransactionChange::SetAmount(int amount) {
	this->amount = amount;
}

string BankTransactionChange::GetAmount() const {
	return to_string(amount);
}

CashTransactionChange::CashTransactionChange() {

}

CashTransactionChange::CashTransactionChange(int amount)
	: amount(amount) {

}

CashTransactionChange::~CashTransactionChange() {

}

string CashTransactionChange::GetType() const {
	return "cash_transaction";
}

void CashTransactionChange::SetAmount(int amount) {
	this->amount = amount;
}

string CashTransactionChange::GetAmount() const {
	return to_string(amount);
}

GiveItemChange::GiveItemChange() {

}

GiveItemChange::GiveItemChange(string item, int num)
	: item(item), num(num) {

}

GiveItemChange::~GiveItemChange() {

}

string GiveItemChange::GetType() const {
	return "give_item";
}

void GiveItemChange::SetItem(string item) {
	this->item = item;
}

string GiveItemChange::GetItem() const {
	return item;
}

void GiveItemChange::SetNum(int num) {
	this->num = num;
}

int GiveItemChange::GetNum() {
	return num;
}

RemoveItemChange::RemoveItemChange() {

}

RemoveItemChange::RemoveItemChange(string item, int num)
	: item(item), num(num) {

}

RemoveItemChange::~RemoveItemChange() {

}

string RemoveItemChange::GetType() const {
	return "remove_item";
}

void RemoveItemChange::SetItem(string item) {
	this->item = item;
}

string RemoveItemChange::GetItem() const {
	return item;
}

void RemoveItemChange::SetNum(int num) {
	this->num = num;
}

int RemoveItemChange::GetNum() {
	return num;
}

PlayerInjuredChange::PlayerInjuredChange() {

}

PlayerInjuredChange::PlayerInjuredChange(string wound)
	: wound(wound) {

}

PlayerInjuredChange::~PlayerInjuredChange() {

}

string PlayerInjuredChange::GetType() const {
	return "player_injured";
}

void PlayerInjuredChange::SetWound(string wound) {
	this->wound = wound;
}

string PlayerInjuredChange::GetWound() const {
	return wound;
}

PlayerCuredChange::PlayerCuredChange() {

}

PlayerCuredChange::PlayerCuredChange(string wound)
	: wound(wound) {

}

PlayerCuredChange::~PlayerCuredChange() {

}

string PlayerCuredChange::GetType() const {
	return "player_cured";
}

void PlayerCuredChange::SetWound(string wound) {
	this->wound = wound;
}

string PlayerCuredChange::GetWound() const {
	return wound;
}

PlayerIllChange::PlayerIllChange() {

}

PlayerIllChange::PlayerIllChange(string illness)
	: illness(illness) {

}

PlayerIllChange::~PlayerIllChange() {

}

string PlayerIllChange::GetType() const {
	return "player_ill";
}

void PlayerIllChange::SetIllness(string illness) {
	this->illness = illness;
}

string PlayerIllChange::GetIllness() const {
	return illness;
}

PlayerRecoverChange::PlayerRecoverChange() {

}

PlayerRecoverChange::PlayerRecoverChange(string illness)
	: illness(illness) {

}

PlayerRecoverChange::~PlayerRecoverChange() {

}

string PlayerRecoverChange::GetType() const {
	return "player_recover";
}

void PlayerRecoverChange::SetIllness(string illness) {
	this->illness = illness;
}

string PlayerRecoverChange::GetIllness() const {
	return illness;
}

PlayerSleepChange::PlayerSleepChange() {

}

PlayerSleepChange::PlayerSleepChange(int hour)
	: hour(hour) {

}

PlayerSleepChange::~PlayerSleepChange() {

}

string PlayerSleepChange::GetType() const {
	return "player_sleep";
}

void PlayerSleepChange::SetHour(int hour) {
	this->hour = hour;
}

int PlayerSleepChange::GetHour() {
	return hour;
}

CreateTimerChange::CreateTimerChange() {

}

CreateTimerChange::CreateTimerChange(string timer, Time duration, int loop)
	: timer(timer), duration(duration), loop(loop) {

}

CreateTimerChange::~CreateTimerChange() {

}

string CreateTimerChange::GetType() const {
	return "create_timer";
}

void CreateTimerChange::SetTimer(string timer) {
	this->timer = timer;
}

string CreateTimerChange::GetTimer() const {
	return timer;
}

void CreateTimerChange::SetDuration(Time duration) {
	this->duration = duration;
}

Time CreateTimerChange::GetDuration() {
	return duration;
}

void CreateTimerChange::SetLoop(int loop) {
	this->loop = loop;
}

int CreateTimerChange::GetLoop() {
	return loop;
}

PauseTimerChange::PauseTimerChange() {

}

PauseTimerChange::PauseTimerChange(string timer)
	: timer(timer) {

}

PauseTimerChange::~PauseTimerChange() {

}

string PauseTimerChange::GetType() const {
	return "pause_timer";
}

void PauseTimerChange::SetTimer(string timer) {
	this->timer = timer;
}

string PauseTimerChange::GetTimer() const {
	return timer;
}

ResumeTimerChange::ResumeTimerChange() {

}

ResumeTimerChange::ResumeTimerChange(string timer)
	: timer(timer) {

}

ResumeTimerChange::~ResumeTimerChange() {

}

string ResumeTimerChange::GetType() const {
	return "resume_timer";
}

void ResumeTimerChange::SetTimer(string timer) {
	this->timer = timer;
}

string ResumeTimerChange::GetTimer() const {
	return timer;
}

RemoveTimerChange::RemoveTimerChange() {

}

RemoveTimerChange::RemoveTimerChange(string timer)
	: timer(timer) {

}

RemoveTimerChange::~RemoveTimerChange() {

}

string RemoveTimerChange::GetType() const {
	return "remove_timer";
}

void RemoveTimerChange::SetTimer(string timer) {
	this->timer = timer;
}

string RemoveTimerChange::GetTimer() const {
	return timer;
}

ResetTimerChange::ResetTimerChange() {

}

ResetTimerChange::ResetTimerChange(string timer)
	: timer(timer) {

}

ResetTimerChange::~ResetTimerChange() {

}

string ResetTimerChange::GetType() const {
	return "reset_timer";
}

void ResetTimerChange::SetTimer(string timer) {
	this->timer = timer;
}

string ResetTimerChange::GetTimer() const {
	return timer;
}

CreateCounterChange::CreateCounterChange() {

}

CreateCounterChange::CreateCounterChange(string counter, int count)
	: counter(counter), count(count) {

}

CreateCounterChange::~CreateCounterChange() {

}

string CreateCounterChange::GetType() const {
	return "create_counter";
}

void CreateCounterChange::SetCounter(string counter) {
	this->counter = counter;
}

string CreateCounterChange::GetCounter() const {
	return counter;
}

void CreateCounterChange::SetCount(int count) {
	this->count = count;
}

int CreateCounterChange::GetCount() {
	return count;
}

CallCounterChange::CallCounterChange() {

}

CallCounterChange::CallCounterChange(string counter, int delta)
	: counter(counter), delta(delta) {

}

CallCounterChange::~CallCounterChange() {

}

string CallCounterChange::GetType() const {
	return "call_counter";
}

void CallCounterChange::SetCounter(string counter) {
	this->counter = counter;
}

string CallCounterChange::GetCounter() const {
	return counter;
}

void CallCounterChange::SetDelta(int delta) {
	this->delta = delta;
}

int CallCounterChange::GetDelta() {
	return delta;
}

RemoveCounterChange::RemoveCounterChange() {

}

RemoveCounterChange::RemoveCounterChange(string counter)
	: counter(counter) {

}

RemoveCounterChange::~RemoveCounterChange() {

}

string RemoveCounterChange::GetType() const {
	return "remove_counter";
}

void RemoveCounterChange::SetCounter(string counter) {
	this->counter = counter;
}

string RemoveCounterChange::GetCounter() const {
	return counter;
}

EnterBattleChange::EnterBattleChange() {

}

EnterBattleChange::EnterBattleChange(string enemy)
	: enemy(enemy) {

}

EnterBattleChange::~EnterBattleChange() {

}

string EnterBattleChange::GetType() const {
	return "enter_battle";
}

void EnterBattleChange::SetEnemy(string enemy) {
	this->enemy = enemy;
}

string EnterBattleChange::GetEnemy() const {
	return enemy;
}

ChangeTimeChange::ChangeTimeChange() {

}

ChangeTimeChange::ChangeTimeChange(Time delta)
	: delta(delta) {

}

ChangeTimeChange::~ChangeTimeChange() {

}

string ChangeTimeChange::GetType() const {
	return "change_time";
}

void ChangeTimeChange::SetDelta(Time delta) {
	this->delta = delta;
}

Time ChangeTimeChange::GetDelta() {
	return delta;
}

ChangeCultivationChange::ChangeCultivationChange() {

}

ChangeCultivationChange::ChangeCultivationChange(string method, int level)
	: method(method), level(level) {

}

ChangeCultivationChange::~ChangeCultivationChange() {

}

string ChangeCultivationChange::GetType() const {
	return "change_cultivation";
}

void ChangeCultivationChange::SetMethod(string method) {
	this->method = method;
}

string ChangeCultivationChange::GetMethod() const {
	return method;
}

void ChangeCultivationChange::SetLevel(int level) {
	this->level = level;
}

int ChangeCultivationChange::GetLevel() {
	return level;
}

ChangeWantedChange::ChangeWantedChange() {

}

ChangeWantedChange::ChangeWantedChange(string reason, int level)
	: reason(reason), level(level) {

}

ChangeWantedChange::~ChangeWantedChange() {

}

string ChangeWantedChange::GetType() const {
	return "change_wanted";
}

void ChangeWantedChange::SetReason(string reason) {
	this->reason = reason;
}

string ChangeWantedChange::GetReason() const {
	return reason;
}

void ChangeWantedChange::SetLevel(int level) {
	this->level = level;
}

int ChangeWantedChange::GetLevel() {
	return level;
}

ChangeWeatherChange::ChangeWeatherChange() {

}

ChangeWeatherChange::ChangeWeatherChange(string weather)
	: weather(weather) {

}

ChangeWeatherChange::~ChangeWeatherChange() {

}

string ChangeWeatherChange::GetType() const {
	return "change_weather";
}

void ChangeWeatherChange::SetWeather(string weather) {
	this->weather = weather;
}

string ChangeWeatherChange::GetWeather() const {
	return weather;
}

ChangePolicyChange::ChangePolicyChange() {

}

ChangePolicyChange::ChangePolicyChange(string policy)
	: policy(policy) {

}

ChangePolicyChange::~ChangePolicyChange() {

}

string ChangePolicyChange::GetType() const {
	return "change_policy";
}

void ChangePolicyChange::SetPolicy(string policy) {
	this->policy = policy;
}

string ChangePolicyChange::GetPolicy() const {
	return policy;
}
