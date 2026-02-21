#include "change.h"

using namespace std;

// SetValueChange 类
SetValueChange::SetValueChange() {

}

SetValueChange::SetValueChange(string variable, string value)
    : variable(variable), value(value) {

}

SetValueChange::~SetValueChange() {

}

string SetValueChange::GetId() {
    return "set_value";
}

string SetValueChange::GetType() const {
    return "set_value";
}

string SetValueChange::GetName() const {
    return "set_value";
}

vector<Change*> SetValueChange::ApplyChange() {
    return vector<Change*>();
}

void SetValueChange::SetVariable(string variable) {
    this->variable = variable;
}

string SetValueChange::GetVariable() {
    return variable;
}

void SetValueChange::SetValue(string value) {
    this->value = value;
}

string SetValueChange::GetValue() {
    return value;
}

// RemoveValueChange 类
RemoveValueChange::RemoveValueChange() {

}

RemoveValueChange::RemoveValueChange(string variable)
    : variable(variable) {

}

RemoveValueChange::~RemoveValueChange() {

}

string RemoveValueChange::GetId() {
    return "remove_value";
}

string RemoveValueChange::GetType() const {
    return "remove_value";
}

string RemoveValueChange::GetName() const {
    return "remove_value";
}

void RemoveValueChange::SetVariable(string variable) {
    this->variable = variable;
}

string RemoveValueChange::GetVariable() {
    return variable;
}

vector<Change*> RemoveValueChange::ApplyChange() {
    return vector<Change*>();
}

// DeactivateMileStone 类
DeactivateMilestoneChange::DeactivateMilestoneChange() {

}

DeactivateMilestoneChange::DeactivateMilestoneChange(string milestone)
    : milestone(milestone) {

}

DeactivateMilestoneChange::~DeactivateMilestoneChange() {

}

string DeactivateMilestoneChange::GetId() {
    return "deactivate_milestone";
}

string DeactivateMilestoneChange::GetType() const {
    return "deactivate_milestone";
}

string DeactivateMilestoneChange::GetName() const {
    return "deactivate_milestone";
}

void DeactivateMilestoneChange::SetMilestone(string milestone) {
    this->milestone = milestone;
}

string DeactivateMilestoneChange::GetMilestone() {
    return milestone;
}

vector<Change*> DeactivateMilestoneChange::ApplyChange() {
    return vector<Change*>();
}

// AddOptionChange 类
AddOptionChange::AddOptionChange() {

}

AddOptionChange::AddOptionChange(string target, string option)
    : target(target), option(option) {

}

AddOptionChange::~AddOptionChange() {

}

string AddOptionChange::GetId() {
    return "add_option";
}

string AddOptionChange::GetType() const {
    return "add_option";
}

string AddOptionChange::GetName() const {
    return "add_option";
}

void AddOptionChange::SetTarget(string target) {
    this->target = target;
}

string AddOptionChange::GetTarget() {
    return target;
}

void AddOptionChange::SetOption(string option) {
    this->option = option;
}

string AddOptionChange::GetOption() {
    return option;
}

vector<Change*> AddOptionChange::ApplyChange() {
    return vector<Change*>();
}

// RemoveOptionChange 类
RemoveOptionChange::RemoveOptionChange() {

}

RemoveOptionChange::RemoveOptionChange(string target, string option)
    : target(target), option(option) {

}

RemoveOptionChange::~RemoveOptionChange() {

}

string RemoveOptionChange::GetId() {
    return "remove_option";
}

string RemoveOptionChange::GetType() const {
    return "remove_option";
}

string RemoveOptionChange::GetName() const {
    return "remove_option";
}

void RemoveOptionChange::SetTarget(string target) {
    this->target = target;
}

string RemoveOptionChange::GetTarget() {
    return target;
}

void RemoveOptionChange::SetOption(string option) {
    this->option = option;
}

string RemoveOptionChange::GetOption() {
    return option;
}

vector<Change*> RemoveOptionChange::ApplyChange() {
    return vector<Change*>();
}

// SpawnNpcChange 类
SpawnNpcChange::SpawnNpcChange() {

}

SpawnNpcChange::SpawnNpcChange(string target, string gender, string birthday)
    : target(target), gender(gender), birthday(birthday) {

}

SpawnNpcChange::~SpawnNpcChange() {

}

string SpawnNpcChange::GetId() {
    return "spawn_npc";
}

string SpawnNpcChange::GetType() const {
    return "spawn_npc";
}

string SpawnNpcChange::GetName() const {
    return "spawn_npc";
}

void SpawnNpcChange::SetTarget(string target) {
    this->target = target;
}

string SpawnNpcChange::GetTarget() {
    return target;
}

void SpawnNpcChange::SetGender(string gender) {
    this->gender = gender;
}

string SpawnNpcChange::GetGender() {
    return gender;
}

void SpawnNpcChange::SetBirthday(string birthday) {
    this->birthday = birthday;
}

string SpawnNpcChange::GetBirthday() {
    return birthday;
}

vector<Change*> SpawnNpcChange::ApplyChange() {
    return vector<Change*>();
}

// RemoveNpcChange 类
RemoveNpcChange::RemoveNpcChange() {

}

RemoveNpcChange::RemoveNpcChange(string target)
    : target(target) {

}

RemoveNpcChange::~RemoveNpcChange() {

}

string RemoveNpcChange::GetId() {
    return "remove_npc";
}

string RemoveNpcChange::GetType() const {
    return "remove_npc";
}

string RemoveNpcChange::GetName() const {
    return "remove_npc";
}

void RemoveNpcChange::SetTarget(string target) {
    this->target = target;
}

string RemoveNpcChange::GetTarget() {
    return target;
}

vector<Change*> RemoveNpcChange::ApplyChange() {
    return vector<Change*>();
}

// NPCNavigateChange 类
NPCNavigateChange::NPCNavigateChange() {

}

NPCNavigateChange::NPCNavigateChange(string target, string destination)
    : target(target), destination(destination) {

}

NPCNavigateChange::~NPCNavigateChange() {

}

string NPCNavigateChange::GetId() {
    return "npc_navigate";
}

string NPCNavigateChange::GetType() const {
    return "npc_navigate";
}

string NPCNavigateChange::GetName() const {
    return "npc_navigate";
}

void NPCNavigateChange::SetTarget(string target) {
    this->target = target;
}

string NPCNavigateChange::GetTarget() {
    return target;
}

void NPCNavigateChange::SetDestination(string destination) {
    this->destination = destination;
}

string NPCNavigateChange::GetDestination() {
    return destination;
}

vector<Change*> NPCNavigateChange::ApplyChange() {
    return vector<Change*>();
}

// TeleportCitizenChange 类
TeleportCitizenChange::TeleportCitizenChange() {

}

TeleportCitizenChange::TeleportCitizenChange(string target, string destination)
    : target(target), destination(destination) {

}

TeleportCitizenChange::~TeleportCitizenChange() {

}

string TeleportCitizenChange::GetId() {
    return "teleport_citizen";
}

string TeleportCitizenChange::GetType() const {
    return "teleport_citizen";
}

string TeleportCitizenChange::GetName() const {
    return "teleport_citizen";
}

void TeleportCitizenChange::SetTarget(string target) {
    this->target = target;
}

string TeleportCitizenChange::GetTarget() {
    return target;
}

void TeleportCitizenChange::SetDestination(string destination) {
    this->destination = destination;
}

string TeleportCitizenChange::GetDestination() {
    return destination;
}

vector<Change*> TeleportCitizenChange::ApplyChange() {
    return vector<Change*>();
}

// TeleportPlayerChange 类
TeleportPlayerChange::TeleportPlayerChange() {

}

TeleportPlayerChange::TeleportPlayerChange(string destination)
    : destination(destination) {

}

TeleportPlayerChange::~TeleportPlayerChange() {

}

string TeleportPlayerChange::GetId() {
    return "teleport_player";
}

string TeleportPlayerChange::GetType() const {
    return "teleport_player";
}

string TeleportPlayerChange::GetName() const {
    return "teleport_player";
}

void TeleportPlayerChange::SetDestination(string destination) {
    this->destination = destination;
}

string TeleportPlayerChange::GetDestination() {
    return destination;
}

vector<Change*> TeleportPlayerChange::ApplyChange() {
    return vector<Change*>();
}

// BankTransactionChange 类
BankTransactionChange::BankTransactionChange() {

}

BankTransactionChange::BankTransactionChange(int amount)
    : amount(amount) {

}

BankTransactionChange::~BankTransactionChange() {

}

string BankTransactionChange::GetId() {
    return "bank_transaction";
}

string BankTransactionChange::GetType() const {
    return "bank_transaction";
}

string BankTransactionChange::GetName() const {
    return "bank_transaction";
}

void BankTransactionChange::SetAmount(int amount) {
    this->amount = amount;
}

string BankTransactionChange::GetAmount() {
    return to_string(amount);
}

vector<Change*> BankTransactionChange::ApplyChange() {
    return vector<Change*>();
}

// CashTransactionChange 类
CashTransactionChange::CashTransactionChange() {

}

CashTransactionChange::CashTransactionChange(int amount)
    : amount(amount) {

}

CashTransactionChange::~CashTransactionChange() {

}

string CashTransactionChange::GetId() {
    return "cash_transaction";
}

string CashTransactionChange::GetType() const {
    return "cash_transaction";
}

string CashTransactionChange::GetName() const {
    return "cash_transaction";
}

void CashTransactionChange::SetAmount(int amount) {
    this->amount = amount;
}

string CashTransactionChange::GetAmount() {
    return to_string(amount);
}

vector<Change*> CashTransactionChange::ApplyChange() {
    return vector<Change*>();
}

// GiveItemChange 类
GiveItemChange::GiveItemChange() {

}

GiveItemChange::GiveItemChange(string item, int num)
    : item(item), num(num) {

}

GiveItemChange::~GiveItemChange() {

}

string GiveItemChange::GetId() {
    return "give_item";
}

string GiveItemChange::GetType() const {
    return "give_item";
}

string GiveItemChange::GetName() const {
    return "give_item";
}

void GiveItemChange::SetItem(string item) {
    this->item = item;
}

string GiveItemChange::GetItem() {
    return item;
}

void GiveItemChange::SetNum(int num) {
    this->num = num;
}

int GiveItemChange::GetNum() {
    return num;
}

vector<Change*> GiveItemChange::ApplyChange() {
    return vector<Change*>();
}

// RemoveItemChange 类
RemoveItemChange::RemoveItemChange() {

}

RemoveItemChange::RemoveItemChange(string item, int num)
    : item(item), num(num) {

}

RemoveItemChange::~RemoveItemChange() {

}

string RemoveItemChange::GetId() {
    return "remove_item";
}

string RemoveItemChange::GetType() const {
    return "remove_item";
}

string RemoveItemChange::GetName() const {
    return "remove_item";
}

void RemoveItemChange::SetItem(string item) {
    this->item = item;
}

string RemoveItemChange::GetItem() {
    return item;
}

void RemoveItemChange::SetNum(int num) {
    this->num = num;
}

int RemoveItemChange::GetNum() {
    return num;
}

vector<Change*> RemoveItemChange::ApplyChange() {
    return vector<Change*>();
}

// PlayerInjuredChange 类
PlayerInjuredChange::PlayerInjuredChange() {

}

PlayerInjuredChange::PlayerInjuredChange(string wound)
    : wound(wound) {

}

PlayerInjuredChange::~PlayerInjuredChange() {

}

string PlayerInjuredChange::GetId() {
    return "player_injured";
}

string PlayerInjuredChange::GetType() const {
    return "player_injured";
}

string PlayerInjuredChange::GetName() const {
    return "player_injured";
}

void PlayerInjuredChange::SetWound(string wound) {
    this->wound = wound;
}

string PlayerInjuredChange::GetWound() {
    return wound;
}

vector<Change*> PlayerInjuredChange::ApplyChange() {
    return vector<Change*>();
}

// PlayerCuredChange 类
PlayerCuredChange::PlayerCuredChange() {

}

PlayerCuredChange::PlayerCuredChange(string wound)
    : wound(wound) {

}

PlayerCuredChange::~PlayerCuredChange() {

}

string PlayerCuredChange::GetId() {
    return "player_cured";
}

string PlayerCuredChange::GetType() const {
    return "player_cured";
}

string PlayerCuredChange::GetName() const {
    return "player_cured";
}

void PlayerCuredChange::SetWound(string wound) {
    this->wound = wound;
}

string PlayerCuredChange::GetWound() {
    return wound;
}

vector<Change*> PlayerCuredChange::ApplyChange() {
    return vector<Change*>();
}

// PlayerIllChange 类
PlayerIllChange::PlayerIllChange() {

}

PlayerIllChange::PlayerIllChange(string illness)
    : illness(illness) {

}

PlayerIllChange::~PlayerIllChange() {

}

string PlayerIllChange::GetId() {
    return "player_ill";
}

string PlayerIllChange::GetType() const {
    return "player_ill";
}

string PlayerIllChange::GetName() const {
    return "player_ill";
}

void PlayerIllChange::SetIllness(string illness) {
    this->illness = illness;
}

string PlayerIllChange::GetIllness() {
    return illness;
}

vector<Change*> PlayerIllChange::ApplyChange() {
    return vector<Change*>();
}

// PlayerRecoverChange 类
PlayerRecoverChange::PlayerRecoverChange() {

}

PlayerRecoverChange::PlayerRecoverChange(string illness)
    : illness(illness) {

}

PlayerRecoverChange::~PlayerRecoverChange() {

}

string PlayerRecoverChange::GetId() {
    return "player_recover";
}

string PlayerRecoverChange::GetType() const {
    return "player_recover";
}

string PlayerRecoverChange::GetName() const {
    return "player_recover";
}

void PlayerRecoverChange::SetIllness(string illness) {
    this->illness = illness;
}

string PlayerRecoverChange::GetIllness() {
    return illness;
}

vector<Change*> PlayerRecoverChange::ApplyChange() {
    return vector<Change*>();
}

// PlayerSleepChange 类
PlayerSleepChange::PlayerSleepChange() {

}

PlayerSleepChange::PlayerSleepChange(int hour)
    : hour(hour) {

}

PlayerSleepChange::~PlayerSleepChange() {

}

string PlayerSleepChange::GetId() {
    return "player_sleep";
}

string PlayerSleepChange::GetType() const {
    return "player_sleep";
}

string PlayerSleepChange::GetName() const {
    return "player_sleep";
}

void PlayerSleepChange::SetHour(int hour) {
    this->hour = hour;
}

int PlayerSleepChange::GetHour() {
    return hour;
}

vector<Change*> PlayerSleepChange::ApplyChange() {
    return vector<Change*>();
}

// CreateTimerChange 类
CreateTimerChange::CreateTimerChange() {

}

CreateTimerChange::CreateTimerChange(string timer, Time duration, int loop)
    : timer(timer), duration(duration), loop(loop) {

}

CreateTimerChange::~CreateTimerChange() {

}

string CreateTimerChange::GetId() {
    return "create_timer";
}

string CreateTimerChange::GetType() const {
    return "create_timer";
}

string CreateTimerChange::GetName() const {
    return "create_timer";
}

void CreateTimerChange::SetTimer(string timer) {
    this->timer = timer;
}

string CreateTimerChange::GetTimer() {
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

vector<Change*> CreateTimerChange::ApplyChange() {
    return vector<Change*>();
}

// PauseTimerChange 类
PauseTimerChange::PauseTimerChange() {

}

PauseTimerChange::PauseTimerChange(string timer)
    : timer(timer) {

}

PauseTimerChange::~PauseTimerChange() {

}

string PauseTimerChange::GetId() {
    return "pause_timer";
}

string PauseTimerChange::GetType() const {
    return "pause_timer";
}

string PauseTimerChange::GetName() const {
    return "pause_timer";
}

void PauseTimerChange::SetTimer(string timer) {
    this->timer = timer;
}

string PauseTimerChange::GetTimer() {
    return timer;
}

vector<Change*> PauseTimerChange::ApplyChange() {
    return vector<Change*>();
}

// ResumeTimerChange 类
ResumeTimerChange::ResumeTimerChange() {

}

ResumeTimerChange::ResumeTimerChange(string timer)
    : timer(timer) {

}

ResumeTimerChange::~ResumeTimerChange() {

}

string ResumeTimerChange::GetId() {
    return "resume_timer";
}

string ResumeTimerChange::GetType() const {
    return "resume_timer";
}

string ResumeTimerChange::GetName() const {
    return "resume_timer";
}

void ResumeTimerChange::SetTimer(string timer) {
    this->timer = timer;
}

string ResumeTimerChange::GetTimer() {
    return timer;
}

vector<Change*> ResumeTimerChange::ApplyChange() {
    return vector<Change*>();
}

// RemoveTimerChange 类
RemoveTimerChange::RemoveTimerChange() {

}

RemoveTimerChange::RemoveTimerChange(string timer)
    : timer(timer) {

}

RemoveTimerChange::~RemoveTimerChange() {

}

string RemoveTimerChange::GetId() {
    return "remove_timer";
}

string RemoveTimerChange::GetType() const {
    return "remove_timer";
}

string RemoveTimerChange::GetName() const {
    return "remove_timer";
}

void RemoveTimerChange::SetTimer(string timer) {
    this->timer = timer;
}

string RemoveTimerChange::GetTimer() {
    return timer;
}

vector<Change*> RemoveTimerChange::ApplyChange() {
    return vector<Change*>();
}

// ResetTimerChange 类
ResetTimerChange::ResetTimerChange() {

}

ResetTimerChange::ResetTimerChange(string timer)
    : timer(timer) {

}

ResetTimerChange::~ResetTimerChange() {

}

string ResetTimerChange::GetId() {
    return "reset_timer";
}

string ResetTimerChange::GetType() const {
    return "reset_timer";
}

string ResetTimerChange::GetName() const {
    return "reset_timer";
}

void ResetTimerChange::SetTimer(string timer) {
    this->timer = timer;
}

string ResetTimerChange::GetTimer() {
    return timer;
}

vector<Change*> ResetTimerChange::ApplyChange() {
    return vector<Change*>();
}

// CreateCounterChange 类
CreateCounterChange::CreateCounterChange() {

}

CreateCounterChange::CreateCounterChange(string counter, int count)
    : counter(counter), count(count) {

}

CreateCounterChange::~CreateCounterChange() {

}

string CreateCounterChange::GetId() {
    return "create_counter";
}

string CreateCounterChange::GetType() const {
    return "create_counter";
}

string CreateCounterChange::GetName() const {
    return "create_counter";
}

void CreateCounterChange::SetCounter(string counter) {
    this->counter = counter;
}

string CreateCounterChange::GetCounter() {
    return counter;
}

void CreateCounterChange::SetCount(int count) {
    this->count = count;
}

int CreateCounterChange::GetCount() {
    return count;
}

vector<Change*> CreateCounterChange::ApplyChange() {
    return vector<Change*>();
}

// CallCounterChange 类
CallCounterChange::CallCounterChange() {

}

CallCounterChange::CallCounterChange(string counter, int delta)
    : counter(counter), delta(delta) {

}

CallCounterChange::~CallCounterChange() {

}

string CallCounterChange::GetId() {
    return "call_counter";
}

string CallCounterChange::GetType() const {
    return "call_counter";
}

string CallCounterChange::GetName() const {
    return "call_counter";
}

void CallCounterChange::SetCounter(string counter) {
    this->counter = counter;
}

string CallCounterChange::GetCounter() {
    return counter;
}

void CallCounterChange::SetDelta(int delta) {
    this->delta = delta;
}

int CallCounterChange::GetDelta() {
    return delta;
}

vector<Change*> CallCounterChange::ApplyChange() {
    return vector<Change*>();
}

// RemoveCounterChange 类
RemoveCounterChange::RemoveCounterChange() {

}

RemoveCounterChange::RemoveCounterChange(string counter)
    : counter(counter) {

}

RemoveCounterChange::~RemoveCounterChange() {

}

string RemoveCounterChange::GetId() {
    return "remove_counter";
}

string RemoveCounterChange::GetType() const {
    return "remove_counter";
}

string RemoveCounterChange::GetName() const {
    return "remove_counter";
}

void RemoveCounterChange::SetCounter(string counter) {
    this->counter = counter;
}

string RemoveCounterChange::GetCounter() {
    return counter;
}

vector<Change*> RemoveCounterChange::ApplyChange() {
    return vector<Change*>();
}

// EnterBattleChange 类
EnterBattleChange::EnterBattleChange() {

}

EnterBattleChange::EnterBattleChange(string enemy)
    : enemy(enemy) {

}

EnterBattleChange::~EnterBattleChange() {

}

string EnterBattleChange::GetId() {
    return "enter_battle";
}

string EnterBattleChange::GetType() const {
    return "enter_battle";
}

string EnterBattleChange::GetName() const {
    return "enter_battle";
}

void EnterBattleChange::SetEnemy(string enemy) {
    this->enemy = enemy;
}

string EnterBattleChange::GetEnemy() {
    return enemy;
}

vector<Change*> EnterBattleChange::ApplyChange() {
    return vector<Change*>();
}

// ChangeTimeChange 类
ChangeTimeChange::ChangeTimeChange() {

}

ChangeTimeChange::ChangeTimeChange(Time delta)
    : delta(delta) {

}

ChangeTimeChange::~ChangeTimeChange() {

}

string ChangeTimeChange::GetId() {
    return "change_time";
}

string ChangeTimeChange::GetType() const {
    return "change_time";
}

string ChangeTimeChange::GetName() const {
    return "change_time";
}

void ChangeTimeChange::SetDelta(Time delta) {
    this->delta = delta;
}

Time ChangeTimeChange::GetDelta() {
    return delta;
}

vector<Change*> ChangeTimeChange::ApplyChange() {
    return vector<Change*>();
}

// ChangeCultivationChange 类
ChangeCultivationChange::ChangeCultivationChange() {

}

ChangeCultivationChange::ChangeCultivationChange(string method, int level)
    : method(method), level(level) {

}

ChangeCultivationChange::~ChangeCultivationChange() {

}

string ChangeCultivationChange::GetId() {
    return "change_cultivation";
}

string ChangeCultivationChange::GetType() const {
    return "change_cultivation";
}

string ChangeCultivationChange::GetName() const {
    return "change_cultivation";
}

void ChangeCultivationChange::SetMethod(string method) {
    this->method = method;
}

string ChangeCultivationChange::GetMethod() {
    return method;
}

void ChangeCultivationChange::SetLevel(int level) {
    this->level = level;
}

int ChangeCultivationChange::GetLevel() {
    return level;
}

vector<Change*> ChangeCultivationChange::ApplyChange() {
    return vector<Change*>();
}

// ChangeWantedChange 类
ChangeWantedChange::ChangeWantedChange() {

}

ChangeWantedChange::ChangeWantedChange(string reason, int level)
    : reason(reason), level(level) {

}

ChangeWantedChange::~ChangeWantedChange() {

}

string ChangeWantedChange::GetId() {
    return "change_wanted";
}

string ChangeWantedChange::GetType() const {
    return "change_wanted";
}

string ChangeWantedChange::GetName() const {
    return "change_wanted";
}

void ChangeWantedChange::SetReason(string reason) {
    this->reason = reason;
}

string ChangeWantedChange::GetReason() {
    return reason;
}

void ChangeWantedChange::SetLevel(int level) {
    this->level = level;
}

int ChangeWantedChange::GetLevel() {
    return level;
}

vector<Change*> ChangeWantedChange::ApplyChange() {
    return vector<Change*>();
}

// ChangeWeatherChange 类
ChangeWeatherChange::ChangeWeatherChange() {

}

ChangeWeatherChange::ChangeWeatherChange(string weather)
    : weather(weather) {

}

ChangeWeatherChange::~ChangeWeatherChange() {

}

string ChangeWeatherChange::GetId() {
    return "change_weather";
}

string ChangeWeatherChange::GetType() const {
    return "change_weather";
}

string ChangeWeatherChange::GetName() const {
    return "change_weather";
}

void ChangeWeatherChange::SetWeather(string weather) {
    this->weather = weather;
}

string ChangeWeatherChange::GetWeather() {
    return weather;
}

vector<Change*> ChangeWeatherChange::ApplyChange() {
    return vector<Change*>();
}

// ChangePolicyChange 类
ChangePolicyChange::ChangePolicyChange() {

}

ChangePolicyChange::ChangePolicyChange(string policy)
    : policy(policy) {

}

ChangePolicyChange::~ChangePolicyChange() {

}

string ChangePolicyChange::GetId() {
    return "change_policy";
}

string ChangePolicyChange::GetType() const {
    return "change_policy";
}

string ChangePolicyChange::GetName() const {
    return "change_policy";
}

void ChangePolicyChange::SetPolicy(string policy) {
    this->policy = policy;
}

string ChangePolicyChange::GetPolicy() {
    return policy;
}

vector<Change*> ChangePolicyChange::ApplyChange() {
    return vector<Change*>();
}
