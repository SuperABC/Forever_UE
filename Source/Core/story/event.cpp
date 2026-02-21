#include "event.h"


using namespace std;

GameStartEvent::GameStartEvent() {

}

GameStartEvent::~GameStartEvent() {

}

string GameStartEvent::GetId() {
    return "game_start";
}

string GameStartEvent::GetType() const {
    return "game_start";
}

string GameStartEvent::GetName() const {
    return "game_start";
}

bool GameStartEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;
    return true;
}

GameSuccessEvent::GameSuccessEvent(string result) : result(result) {

}

GameSuccessEvent::~GameSuccessEvent() {

}

string GameSuccessEvent::GetId() {
    return "game_success";
}

string GameSuccessEvent::GetType() const {
    return "game_success";
}

string GameSuccessEvent::GetName() const {
    return "game_success";
}

bool GameSuccessEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<GameSuccessEvent*>(e);
    if (!other) return false;

    return result == other->result;
}

void GameSuccessEvent::SetResult(string result) {
    this->result = result;
}

string GameSuccessEvent::GetResult() {
    return result;
}

GameFailEvent::GameFailEvent(string result) : result(result) {

}

GameFailEvent::~GameFailEvent() {

}

string GameFailEvent::GetId() {
    return "game_fail";
}

string GameFailEvent::GetType() const {
    return "game_fail";
}

string GameFailEvent::GetName() const {
    return "game_fail";
}

bool GameFailEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<GameFailEvent*>(e);
    if (!other) return false;

    return result == other->result;
}

void GameFailEvent::SetResult(string result) {
    this->result = result;
}

string GameFailEvent::GetResult() {
    return result;
}

OptionDialogEvent::OptionDialogEvent(string target, string option)
    : idx(-1), target(target), option(option) {

}

OptionDialogEvent::OptionDialogEvent(int idx, string option)
    : idx(idx), target(""), option(option) {

}

OptionDialogEvent::~OptionDialogEvent() {

}

string OptionDialogEvent::GetId() {
    return "option_dialog";
}

string OptionDialogEvent::GetType() const {
    return "option_dialog";
}

string OptionDialogEvent::GetName() const {
    return "option_dialog";
}

bool OptionDialogEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<OptionDialogEvent*>(e);
    if (!other) return false;

    if (idx == -1 && target == "" ||
        other->idx == -1 && other->target == "") {
        return option == other->option;
    }

    return target == other->target && option == other->option;
}

void OptionDialogEvent::SetIdx(int idx) {
    this->idx = idx;
}

int OptionDialogEvent::GetIdx() const {
    return idx;
}

void OptionDialogEvent::SetTarget(string target) {
    this->target = target;
}

string OptionDialogEvent::GetTarget() const {
    return target;
}

void OptionDialogEvent::SetOption(string option) {
    this->option = option;
}

string OptionDialogEvent::GetOption() const {
    return option;
}

NPCMeetEvent::NPCMeetEvent(string npc) : npc(npc) {

}

NPCMeetEvent::~NPCMeetEvent() {

}

string NPCMeetEvent::GetId() {
    return "npc_meet";
}

string NPCMeetEvent::GetType() const {
    return "npc_meet";
}

string NPCMeetEvent::GetName() const {
    return "npc_meet";
}

bool NPCMeetEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<NPCMeetEvent*>(e);
    if (!other) return false;

    return npc == other->npc;
}

void NPCMeetEvent::SetNPC(string npc) {
    this->npc = npc;
}

string NPCMeetEvent::GetNPC() const {
    return npc;
}

CitizenBornEvent::CitizenBornEvent(string name, GENDER_TYPE gender)
    : name(name), gender(gender) {

}

CitizenBornEvent::~CitizenBornEvent() {

}

string CitizenBornEvent::GetId() {
    return "citizen_born";
}

string CitizenBornEvent::GetType() const {
    return "citizen_born";
}

string CitizenBornEvent::GetName() const {
    return "citizen_born";
}

bool CitizenBornEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<CitizenBornEvent*>(e);
    if (!other) return false;

    return name == other->name && gender == other->gender;
}

void CitizenBornEvent::SetName(string name) {
    this->name = name;
}

string CitizenBornEvent::GetName() {
    return name;
}

void CitizenBornEvent::SetGender(GENDER_TYPE gender) {
    this->gender = gender;
}

string CitizenBornEvent::GetGender() {
    switch (gender) {
    case GENDER_MALE: return "male";
    case GENDER_FEMALE: return "female";
    default: return "unknown";
    }
}

CitizenDeceaseEvent::CitizenDeceaseEvent(string name, string reason)
    : name(name), reason(reason) {

}

CitizenDeceaseEvent::~CitizenDeceaseEvent() {

}

string CitizenDeceaseEvent::GetId() {
    return "citizen_decease";
}

string CitizenDeceaseEvent::GetType() const {
    return "citizen_decease";
}

string CitizenDeceaseEvent::GetName() const {
    return "citizen_decease";
}

bool CitizenDeceaseEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<CitizenDeceaseEvent*>(e);
    if (!other) return false;

    return name == other->name && reason == other->reason;
}

void CitizenDeceaseEvent::SetName(string name) {
    this->name = name;
}

string CitizenDeceaseEvent::GetName() {
    return name;
}

void CitizenDeceaseEvent::SetReason(string reason) {
    this->reason = reason;
}

string CitizenDeceaseEvent::GetReason() {
    return reason;
}

EnterZoneEvent::EnterZoneEvent(string zone) : zone(zone) {

}

EnterZoneEvent::~EnterZoneEvent() {

}

string EnterZoneEvent::GetId() {
    return "enter_zone";
}

string EnterZoneEvent::GetType() const {
    return "enter_zone";
}

string EnterZoneEvent::GetName() const {
    return "enter_zone";
}

bool EnterZoneEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<EnterZoneEvent*>(e);
    if (!other) return false;

    return zone == other->zone;
}

void EnterZoneEvent::SetZone(string zone) {
    this->zone = zone;
}

string EnterZoneEvent::GetZone() {
    return zone;
}

LeaveZoneEvent::LeaveZoneEvent(string zone) : zone(zone) {

}

LeaveZoneEvent::~LeaveZoneEvent() {

}

string LeaveZoneEvent::GetId() {
    return "leave_zone";
}

string LeaveZoneEvent::GetType() const {
    return "leave_zone";
}

string LeaveZoneEvent::GetName() const {
    return "leave_zone";
}

bool LeaveZoneEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<LeaveZoneEvent*>(e);
    if (!other) return false;

    return zone == other->zone;
}

void LeaveZoneEvent::SetZone(string zone) {
    this->zone = zone;
}

string LeaveZoneEvent::GetZone() {
    return zone;
}

EnterBuildingEvent::EnterBuildingEvent(string zone, string building)
    : zone(zone), building(building) {

}

EnterBuildingEvent::~EnterBuildingEvent() {

}

string EnterBuildingEvent::GetId() {
    return "enter_building";
}

string EnterBuildingEvent::GetType() const {
    return "enter_building";
}

string EnterBuildingEvent::GetName() const {
    return "enter_building";
}

bool EnterBuildingEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<EnterBuildingEvent*>(e);
    if (!other) return false;

    return zone == other->zone && building == other->building;
}

void EnterBuildingEvent::SetZone(string zone) {
    this->zone = zone;
}

string EnterBuildingEvent::GetZone() {
    return zone;
}

void EnterBuildingEvent::SetBuilding(string building) {
    this->building = building;
}

string EnterBuildingEvent::GetBuilding() {
    return building;
}

LeaveBuildingEvent::LeaveBuildingEvent(string zone, string building)
    : zone(zone), building(building) {

}

LeaveBuildingEvent::~LeaveBuildingEvent() {

}

string LeaveBuildingEvent::GetId() {
    return "leave_building";
}

string LeaveBuildingEvent::GetType() const {
    return "leave_building";
}

string LeaveBuildingEvent::GetName() const {
    return "leave_building";
}

bool LeaveBuildingEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<LeaveBuildingEvent*>(e);
    if (!other) return false;

    return zone == other->zone && building == other->building;
}

void LeaveBuildingEvent::SetZone(string zone) {
    this->zone = zone;
}

string LeaveBuildingEvent::GetZone() {
    return zone;
}

void LeaveBuildingEvent::SetBuilding(string building) {
    this->building = building;
}

string LeaveBuildingEvent::GetBuilding() {
    return building;
}

EnterRoomEvent::EnterRoomEvent(string zone, string building, string room)
    : zone(zone), building(building), room(room) {

}

EnterRoomEvent::~EnterRoomEvent() {

}

string EnterRoomEvent::GetId() {
    return "enter_room";
}

string EnterRoomEvent::GetType() const {
    return "enter_room";
}

string EnterRoomEvent::GetName() const {
    return "enter_room";
}

bool EnterRoomEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<EnterRoomEvent*>(e);
    if (!other) return false;

    return zone == other->zone && building == other->building && room == other->room;
}

void EnterRoomEvent::SetZone(string zone) {
    this->zone = zone;
}

string EnterRoomEvent::GetZone() {
    return zone;
}

void EnterRoomEvent::SetBuilding(string building) {
    this->building = building;
}

string EnterRoomEvent::GetBuilding() {
    return building;
}

void EnterRoomEvent::SetRoom(string room) {
    this->room = room;
}

string EnterRoomEvent::GetRoom() {
    return room;
}

LeaveRoomEvent::LeaveRoomEvent(string zone, string building, string room)
    : zone(zone), building(building), room(room) {

}

LeaveRoomEvent::~LeaveRoomEvent() {

}

string LeaveRoomEvent::GetId() {
    return "leave_room";
}

string LeaveRoomEvent::GetType() const {
    return "leave_room";
}

string LeaveRoomEvent::GetName() const {
    return "leave_room";
}

bool LeaveRoomEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<LeaveRoomEvent*>(e);
    if (!other) return false;

    return zone == other->zone && building == other->building && room == other->room;
}

void LeaveRoomEvent::SetZone(string zone) {
    this->zone = zone;
}

string LeaveRoomEvent::GetZone() {
    return zone;
}

void LeaveRoomEvent::SetBuilding(string building) {
    this->building = building;
}

string LeaveRoomEvent::GetBuilding() {
    return building;
}

void LeaveRoomEvent::SetRoom(string room) {
    this->room = room;
}

string LeaveRoomEvent::GetRoom() {
    return room;
}

DepositChangeEvent::DepositChangeEvent(int result, int delta)
    : result(result), delta(delta) {

}

DepositChangeEvent::~DepositChangeEvent() {

}

string DepositChangeEvent::GetId() {
    return "deposit_change";
}

string DepositChangeEvent::GetType() const {
    return "deposit_change";
}

string DepositChangeEvent::GetName() const {
    return "deposit_change";
}

bool DepositChangeEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<DepositChangeEvent*>(e);
    if (!other) return false;

    return result == other->result && delta == other->delta;
}

void DepositChangeEvent::SetResult(int result) {
    this->result = result;
}

int DepositChangeEvent::GetResult() {
    return result;
}

void DepositChangeEvent::SetDelta(int delta) {
    this->delta = delta;
}

int DepositChangeEvent::GetDelta() {
    return delta;
}

CashChangeEvent::CashChangeEvent(int result, int delta)
    : result(result), delta(delta) {

}

CashChangeEvent::~CashChangeEvent() {

}

string CashChangeEvent::GetId() {
    return "cash_change";
}

string CashChangeEvent::GetType() const {
    return "cash_change";
}

string CashChangeEvent::GetName() const {
    return "cash_change";
}

bool CashChangeEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<CashChangeEvent*>(e);
    if (!other) return false;

    return result == other->result && delta == other->delta;
}

void CashChangeEvent::SetResult(int result) {
    this->result = result;
}

int CashChangeEvent::GetResult() {
    return result;
}

void CashChangeEvent::SetDelta(int delta) {
    this->delta = delta;
}

int CashChangeEvent::GetDelta() {
    return delta;
}

GetItemEvent::GetItemEvent(string item, int num)
    : item(item), num(num) {

}

GetItemEvent::~GetItemEvent() {

}

string GetItemEvent::GetId() {
    return "get_item";
}

string GetItemEvent::GetType() const {
    return "get_item";
}

string GetItemEvent::GetName() const {
    return "get_item";
}

bool GetItemEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<GetItemEvent*>(e);
    if (!other) return false;

    return item == other->item && num == other->num;
}

void GetItemEvent::SetItem(string item) {
    this->item = item;
}

string GetItemEvent::GetItem() {
    return item;
}

void GetItemEvent::SetNum(int num) {
    this->num = num;
}

int GetItemEvent::GetNum() {
    return num;
}

LoseItemEvent::LoseItemEvent(string item, int num)
    : item(item), num(num) {

}

LoseItemEvent::~LoseItemEvent() {

}

string LoseItemEvent::GetId() {
    return "lose_item";
}

string LoseItemEvent::GetType() const {
    return "lose_item";
}

string LoseItemEvent::GetName() const {
    return "lose_item";
}

bool LoseItemEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<LoseItemEvent*>(e);
    if (!other) return false;

    return item == other->item && num == other->num;
}

void LoseItemEvent::SetItem(string item) {
    this->item = item;
}

string LoseItemEvent::GetItem() {
    return item;
}

void LoseItemEvent::SetNum(int num) {
    this->num = num;
}

int LoseItemEvent::GetNum() {
    return num;
}

PlayerInjuredEvent::PlayerInjuredEvent(string wound) : wound(wound) {

}

PlayerInjuredEvent::~PlayerInjuredEvent() {

}

string PlayerInjuredEvent::GetId() {
    return "player_injured";
}

string PlayerInjuredEvent::GetType() const {
    return "player_injured";
}

string PlayerInjuredEvent::GetName() const {
    return "player_injured";
}

bool PlayerInjuredEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<PlayerInjuredEvent*>(e);
    if (!other) return false;

    return wound == other->wound;
}

void PlayerInjuredEvent::SetWound(string wound) {
    this->wound = wound;
}

string PlayerInjuredEvent::GetWound() {
    return wound;
}

PlayerCuredEvent::PlayerCuredEvent(string wound) : wound(wound) {

}

PlayerCuredEvent::~PlayerCuredEvent() {

}

string PlayerCuredEvent::GetId() {
    return "player_cured";
}

string PlayerCuredEvent::GetType() const {
    return "player_cured";
}

string PlayerCuredEvent::GetName() const {
    return "player_cured";
}

bool PlayerCuredEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<PlayerCuredEvent*>(e);
    if (!other) return false;

    return wound == other->wound;
}

void PlayerCuredEvent::SetWound(string wound) {
    this->wound = wound;
}

string PlayerCuredEvent::GetWound() {
    return wound;
}

PlayerIllEvent::PlayerIllEvent(string illness) : illness(illness) {

}

PlayerIllEvent::~PlayerIllEvent() {

}

string PlayerIllEvent::GetId() {
    return "player_ill";
}

string PlayerIllEvent::GetType() const {
    return "player_ill";
}

string PlayerIllEvent::GetName() const {
    return "player_ill";
}

bool PlayerIllEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<PlayerIllEvent*>(e);
    if (!other) return false;

    return illness == other->illness;
}

void PlayerIllEvent::SetIllness(string illness) {
    this->illness = illness;
}

string PlayerIllEvent::GetIllness() {
    return illness;
}

PlayerRecoverEvent::PlayerRecoverEvent(string illness) : illness(illness) {

}

PlayerRecoverEvent::~PlayerRecoverEvent() {

}

string PlayerRecoverEvent::GetId() {
    return "player_recover";
}

string PlayerRecoverEvent::GetType() const {
    return "player_recover";
}

string PlayerRecoverEvent::GetName() const {
    return "player_recover";
}

bool PlayerRecoverEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<PlayerRecoverEvent*>(e);
    if (!other) return false;

    return illness == other->illness;
}

void PlayerRecoverEvent::SetIllness(string illness) {
    this->illness = illness;
}

string PlayerRecoverEvent::GetIllness() {
    return illness;
}

PlayerRestEvent::PlayerRestEvent(int minute) : minute(minute) {

}

PlayerRestEvent::~PlayerRestEvent() {

}

string PlayerRestEvent::GetId() {
    return "player_rest";
}

string PlayerRestEvent::GetType() const {
    return "player_rest";
}

string PlayerRestEvent::GetName() const {
    return "player_rest";
}

bool PlayerRestEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<PlayerRestEvent*>(e);
    if (!other) return false;

    return minute == other->minute;
}

void PlayerRestEvent::SetMinute(int minute) {
    this->minute = minute;
}

int PlayerRestEvent::GetMinute() {
    return minute;
}

PlayerSleepEvent::PlayerSleepEvent(int hour) : hour(hour) {

}

PlayerSleepEvent::~PlayerSleepEvent() {

}

string PlayerSleepEvent::GetId() {
    return "player_sleep";
}

string PlayerSleepEvent::GetType() const {
    return "player_sleep";
}

string PlayerSleepEvent::GetName() const {
    return "player_sleep";
}

bool PlayerSleepEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<PlayerSleepEvent*>(e);
    if (!other) return false;

    return hour == other->hour;
}

void PlayerSleepEvent::SetHour(int hour) {
    this->hour = hour;
}

int PlayerSleepEvent::GetHour() {
    return hour;
}

TimeUpEvent::TimeUpEvent(std::string timer) : timer(timer) {

}

TimeUpEvent::~TimeUpEvent() {

}

string TimeUpEvent::GetId() {
    return "time_up";
}

string TimeUpEvent::GetType() const {
    return "time_up";
}

string TimeUpEvent::GetName() const {
    return "time_up";
}

bool TimeUpEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<TimeUpEvent*>(e);
    if (!other) return false;

    return timer == other->timer;
}

void TimeUpEvent::SetTimer(std::string timer) {
    this->timer = timer;
}

std::string TimeUpEvent::GetTimer() {
    return timer;
}

CountUpEvent::CountUpEvent(std::string counter) : counter(counter) {

}

CountUpEvent::~CountUpEvent() {

}

string CountUpEvent::GetId() {
    return "count_up";
}

string CountUpEvent::GetType() const {
    return "count_up";
}

string CountUpEvent::GetName() const {
    return "count_up";
}

bool CountUpEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<CountUpEvent*>(e);
    if (!other) return false;

    return counter == other->counter;
}

void CountUpEvent::SetCounter(std::string counter) {
    this->counter = counter;
}

std::string CountUpEvent::GetCounter() {
    return counter;
}

UseSkillEvent::UseSkillEvent(string skill) : skill(skill) {

}

UseSkillEvent::~UseSkillEvent() {

}

string UseSkillEvent::GetId() {
    return "use_skill";
}

string UseSkillEvent::GetType() const {
    return "use_skill";
}

string UseSkillEvent::GetName() const {
    return "use_skill";
}

bool UseSkillEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<UseSkillEvent*>(e);
    if (!other) return false;

    return skill == other->skill;
}

void UseSkillEvent::SetSkill(string skill) {
    this->skill = skill;
}

string UseSkillEvent::GetSkill() {
    return skill;
}

SkillChangeEvent::SkillChangeEvent(string skill, int level)
    : skill(skill), level(level) {

}

SkillChangeEvent::~SkillChangeEvent() {

}

string SkillChangeEvent::GetId() {
    return "skill_change";
}

string SkillChangeEvent::GetType() const {
    return "skill_change";
}

string SkillChangeEvent::GetName() const {
    return "skill_change";
}

bool SkillChangeEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<SkillChangeEvent*>(e);
    if (!other) return false;

    return skill == other->skill && level == other->level;
}

void SkillChangeEvent::SetSkill(string skill) {
    this->skill = skill;
}

string SkillChangeEvent::GetSkill() {
    return skill;
}

void SkillChangeEvent::SetLevel(int level) {
    this->level = level;
}

int SkillChangeEvent::GetLevel() {
    return level;
}

BattleWinEvent::BattleWinEvent(string enemy) : enemy(enemy) {

}

BattleWinEvent::~BattleWinEvent() {

}

string BattleWinEvent::GetId() {
    return "battle_win";
}

string BattleWinEvent::GetType() const {
    return "battle_win";
}

string BattleWinEvent::GetName() const {
    return "battle_win";
}

bool BattleWinEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<BattleWinEvent*>(e);
    if (!other) return false;

    return enemy == other->enemy;
}

void BattleWinEvent::SetEnemy(string enemy) {
    this->enemy = enemy;
}

string BattleWinEvent::GetEnemy() {
    return enemy;
}

BattleLoseEvent::BattleLoseEvent(string enemy) : enemy(enemy) {

}

BattleLoseEvent::~BattleLoseEvent() {

}

string BattleLoseEvent::GetId() {
    return "battle_lose";
}

string BattleLoseEvent::GetType() const {
    return "battle_lose";
}

string BattleLoseEvent::GetName() const {
    return "battle_lose";
}

bool BattleLoseEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<BattleLoseEvent*>(e);
    if (!other) return false;

    return enemy == other->enemy;
}

void BattleLoseEvent::SetEnemy(string enemy) {
    this->enemy = enemy;
}

string BattleLoseEvent::GetEnemy() {
    return enemy;
}

EscapeSuccessEvent::EscapeSuccessEvent(string enemy) : enemy(enemy) {

}

EscapeSuccessEvent::~EscapeSuccessEvent() {

}

string EscapeSuccessEvent::GetId() {
    return "escape_success";
}

string EscapeSuccessEvent::GetType() const {
    return "escape_success";
}

string EscapeSuccessEvent::GetName() const {
    return "escape_success";
}

bool EscapeSuccessEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<EscapeSuccessEvent*>(e);
    if (!other) return false;

    return enemy == other->enemy;
}

void EscapeSuccessEvent::SetEnemy(string enemy) {
    this->enemy = enemy;
}

string EscapeSuccessEvent::GetEnemy() {
    return enemy;
}

EscapeFailEvent::EscapeFailEvent(string enemy) : enemy(enemy) {

}

EscapeFailEvent::~EscapeFailEvent() {

}

string EscapeFailEvent::GetId() {
    return "escape_fail";
}

string EscapeFailEvent::GetType() const {
    return "escape_fail";
}

string EscapeFailEvent::GetName() const {
    return "escape_fail";
}

bool EscapeFailEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<EscapeFailEvent*>(e);
    if (!other) return false;

    return enemy == other->enemy;
}

void EscapeFailEvent::SetEnemy(string enemy) {
    this->enemy = enemy;
}

string EscapeFailEvent::GetEnemy() {
    return enemy;
}

CultivationChangeEvent::CultivationChangeEvent(string method, int level)
    : method(method), level(level) {

}

CultivationChangeEvent::~CultivationChangeEvent() {

}

string CultivationChangeEvent::GetId() {
    return "cultivation_change";
}

string CultivationChangeEvent::GetType() const {
    return "cultivation_change";
}

string CultivationChangeEvent::GetName() const {
    return "cultivation_change";
}

bool CultivationChangeEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<CultivationChangeEvent*>(e);
    if (!other) return false;

    return method == other->method && level == other->level;
}

void CultivationChangeEvent::SetMethod(string method) {
    this->method = method;
}

string CultivationChangeEvent::GetMethod() {
    return method;
}

void CultivationChangeEvent::SetLevel(int level) {
    this->level = level;
}

int CultivationChangeEvent::GetLevel() {
    return level;
}

WantedChangeEvent::WantedChangeEvent(string reason, int level)
    : reason(reason), level(level) {

}

WantedChangeEvent::~WantedChangeEvent() {

}

string WantedChangeEvent::GetId() {
    return "wanted_change";
}

string WantedChangeEvent::GetType() const {
    return "wanted_change";
}

string WantedChangeEvent::GetName() const {
    return "wanted_change";
}

bool WantedChangeEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<WantedChangeEvent*>(e);
    if (!other) return false;

    return reason == other->reason && level == other->level;
}

void WantedChangeEvent::SetReason(string reason) {
    this->reason = reason;
}

string WantedChangeEvent::GetReason() {
    return reason;
}

void WantedChangeEvent::SetLevel(int level) {
    this->level = level;
}

int WantedChangeEvent::GetLevel() {
    return level;
}

PlayerArrestedEvent::PlayerArrestedEvent(string reason) : reason(reason) {

}

PlayerArrestedEvent::~PlayerArrestedEvent() {

}

string PlayerArrestedEvent::GetId() {
    return "player_arrested";
}

string PlayerArrestedEvent::GetType() const {
    return "player_arrested";
}

string PlayerArrestedEvent::GetName() const {
    return "player_arrested";
}

bool PlayerArrestedEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<PlayerArrestedEvent*>(e);
    if (!other) return false;

    return reason == other->reason;
}

void PlayerArrestedEvent::SetReason(string reason) {
    this->reason = reason;
}

string PlayerArrestedEvent::GetReason() {
    return reason;
}

PlayerReleasedEvent::PlayerReleasedEvent(string reason) : reason(reason) {

}

PlayerReleasedEvent::~PlayerReleasedEvent() {

}

string PlayerReleasedEvent::GetId() {
    return "player_released";
}

string PlayerReleasedEvent::GetType() const {
    return "player_released";
}

string PlayerReleasedEvent::GetName() const {
    return "player_released";
}

bool PlayerReleasedEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<PlayerReleasedEvent*>(e);
    if (!other) return false;

    return reason == other->reason;
}

void PlayerReleasedEvent::SetReason(string reason) {
    this->reason = reason;
}

string PlayerReleasedEvent::GetReason() {
    return reason;
}

WeatherChangeEvent::WeatherChangeEvent(string weather) : weather(weather) {

}

WeatherChangeEvent::~WeatherChangeEvent() {

}

string WeatherChangeEvent::GetId() {
    return "weather_change";
}

string WeatherChangeEvent::GetType() const {
    return "weather_change";
}

string WeatherChangeEvent::GetName() const {
    return "weather_change";
}

bool WeatherChangeEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<WeatherChangeEvent*>(e);
    if (!other) return false;

    return weather == other->weather;
}

void WeatherChangeEvent::SetWeather(string weather) {
    this->weather = weather;
}

string WeatherChangeEvent::GetWeather() {
    return weather;
}

PolicyChangeEvent::PolicyChangeEvent(string policy, bool status)
    : policy(policy), status(status) {

}

PolicyChangeEvent::~PolicyChangeEvent() {

}

string PolicyChangeEvent::GetId() {
    return "policy_change";
}

string PolicyChangeEvent::GetType() const {
    return "policy_change";
}

string PolicyChangeEvent::GetName() const {
    return "policy_change";
}

bool PolicyChangeEvent::operator==(Event* e) {
    if (!e) return false;
    if (GetType() != e->GetType()) return false;

    auto other = dynamic_cast<PolicyChangeEvent*>(e);
    if (!other) return false;

    return policy == other->policy && status == other->status;
}

void PolicyChangeEvent::SetPolicy(string policy) {
    this->policy = policy;
}

string PolicyChangeEvent::GetPolicy() {
    return policy;
}

void PolicyChangeEvent::SetStatus(bool status) {
    this->status = status;
}

bool PolicyChangeEvent::GetStatus() {
    return status;
}