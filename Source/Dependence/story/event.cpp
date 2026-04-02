#include "event.h"


using namespace std;

Event::Event() {

}

Event::~Event() {

}

const Condition& Event::GetCondition() {
	return condition;
}

void Event::SetCondition(const Condition& condition) {
	this->condition = condition;
}

GameStartEvent::GameStartEvent() {

}

GameStartEvent::~GameStartEvent() {

}

string GameStartEvent::GetType() const {
	return "game_start";
}

bool GameStartEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;
	return true;
}

ScriptMessageEvent::ScriptMessageEvent(string message) :
	message(message) {

}

ScriptMessageEvent::~ScriptMessageEvent() {

}

string ScriptMessageEvent::GetType() const {
	return "script_message";
}

bool ScriptMessageEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<ScriptMessageEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (message.size() > 0 && other->message.size() > 0) {
		Condition condition;
		condition.ParseCondition(message);
		auto value = condition.EvaluateValue(getValues);
		if (auto messageValue = get_if<string>(&value)) {
			result = (result && *messageValue == other->message);
		}
		else {
			result = (result && message == other->message);
		}
	}

	return result;
}

void ScriptMessageEvent::SetMessage(string message) {
	this->message = message;
}

string ScriptMessageEvent::GetMessage() const {
	return message;
}

OptionDialogEvent::OptionDialogEvent(string name, string option) :
	idx(-1), name(name), option(option) {

}

OptionDialogEvent::OptionDialogEvent(int idx, string option) :
	idx(idx), name(""), option(option) {

}

OptionDialogEvent::~OptionDialogEvent() {

}

string OptionDialogEvent::GetType() const {
	return "option_dialog";
}

bool OptionDialogEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<OptionDialogEvent*>(e);
	if (!other) return false;

	if (idx == -1 && name == "" ||
		other->idx == -1 && other->name == "") {
		return option == other->option;
	}

	bool result = true;
	if (name.size() > 0 && other->name.size() > 0) {
		Condition condition;
		condition.ParseCondition(name);
		auto value = condition.EvaluateValue(getValues);
		if (auto nameValue = get_if<string>(&value)) {
			result = (result && *nameValue == other->name);
		}
		else {
			result = (result && name == other->name);
		}
	}
	if (option.size() > 0 && other->option.size() > 0) {
		Condition condition;
		condition.ParseCondition(option);
		auto value = condition.EvaluateValue(getValues);
		if (auto optionValue = get_if<string>(&value)) {
			result = (result && *optionValue == other->option);
		}
		else {
			result = (result && option == other->option);
		}
	}

	return result;
}

void OptionDialogEvent::SetIdx(int idx) {
	this->idx = idx;
}

int OptionDialogEvent::GetIdx() const {
	return idx;
}

void OptionDialogEvent::SetName(string name) {
	this->name = name;
}

string OptionDialogEvent::GetName() const {
	return name;
}

void OptionDialogEvent::SetOption(string option) {
	this->option = option;
}

string OptionDialogEvent::GetOption() const {
	return option;
}

NPCMeetEvent::NPCMeetEvent(string npc) :
	npc(npc) {

}

NPCMeetEvent::~NPCMeetEvent() {

}

string NPCMeetEvent::GetType() const {
	return "npc_meet";
}

bool NPCMeetEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<NPCMeetEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (npc.size() > 0 && other->npc.size() > 0) {
		Condition condition;
		condition.ParseCondition(npc);
		auto value = condition.EvaluateValue(getValues);
		if (auto npcValue = get_if<string>(&value)) {
			result = (result && *npcValue == other->npc);
		}
		else {
			result = (result && npc == other->npc);
		}
	}
	return result;
}

void NPCMeetEvent::SetNPC(string npc) {
	this->npc = npc;
}

string NPCMeetEvent::GetNPC() const {
	return npc;
}

CitizenBornEvent::CitizenBornEvent(string name)
	: name(name) {

}

CitizenBornEvent::~CitizenBornEvent() {

}

string CitizenBornEvent::GetType() const {
	return "citizen_born";
}

bool CitizenBornEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<CitizenBornEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (name.size() > 0 && other->name.size() > 0) {
		Condition condition;
		condition.ParseCondition(name);
		auto value = condition.EvaluateValue(getValues);
		if (auto nameValue = get_if<string>(&value)) {
			result = (result && *nameValue == other->name);
		}
		else {
			result = (result && name == other->name);
		}
	}
	return result;
}

void CitizenBornEvent::SetName(string name) {
	this->name = name;
}

string CitizenBornEvent::GetName() const {
	return name;
}

CitizenDeceaseEvent::CitizenDeceaseEvent(string name, string reason)
	: name(name), reason(reason) {

}

CitizenDeceaseEvent::~CitizenDeceaseEvent() {

}

string CitizenDeceaseEvent::GetType() const {
	return "citizen_decease";
}

bool CitizenDeceaseEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<CitizenDeceaseEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (name.size() > 0 && other->name.size() > 0) {
		Condition condition;
		condition.ParseCondition(name);
		auto value = condition.EvaluateValue(getValues);
		if (auto nameValue = get_if<string>(&value)) {
			result = (result && *nameValue == other->name);
		}
		else {
			result = (result && name == other->name);
		}
	}
	if (reason.size() > 0 && other->reason.size() > 0) {
		Condition condition;
		condition.ParseCondition(reason);
		auto value = condition.EvaluateValue(getValues);
		if (auto reasonValue = get_if<string>(&value)) {
			result = (result && *reasonValue == other->reason);
		}
		else {
			result = (result && reason == other->reason);
		}
	}
	return result;
}

void CitizenDeceaseEvent::SetName(string name) {
	this->name = name;
}

string CitizenDeceaseEvent::GetName() const {
	return name;
}

void CitizenDeceaseEvent::SetReason(string reason) {
	this->reason = reason;
}

string CitizenDeceaseEvent::GetReason() const {
	return reason;
}

EnterZoneEvent::EnterZoneEvent(string zone) :
	zone(zone) {

}

EnterZoneEvent::~EnterZoneEvent() {

}

string EnterZoneEvent::GetType() const {
	return "enter_zone";
}

bool EnterZoneEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<EnterZoneEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (zone.size() > 0 && other->zone.size() > 0) {
		Condition condition;
		condition.ParseCondition(zone);
		auto value = condition.EvaluateValue(getValues);
		if (auto zoneValue = get_if<string>(&value)) {
			result = (result && *zoneValue == other->zone);
		}
		else {
			result = (result && zone == other->zone);
		}
	}

	return result;
}

void EnterZoneEvent::SetZone(string zone) {
	this->zone = zone;
}

string EnterZoneEvent::GetZone() const {
	return zone;
}

LeaveZoneEvent::LeaveZoneEvent(string zone) :
	zone(zone) {

}

LeaveZoneEvent::~LeaveZoneEvent() {

}

string LeaveZoneEvent::GetType() const {
	return "leave_zone";
}

bool LeaveZoneEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<LeaveZoneEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (zone.size() > 0 && other->zone.size() > 0) {
		Condition condition;
		condition.ParseCondition(zone);
		auto value = condition.EvaluateValue(getValues);
		if (auto zoneValue = get_if<string>(&value)) {
			result = (result && *zoneValue == other->zone);
		}
		else {
			result = (result && zone == other->zone);
		}
	}
	return result;
}

void LeaveZoneEvent::SetZone(string zone) {
	this->zone = zone;
}

string LeaveZoneEvent::GetZone() const {
	return zone;
}

EnterBuildingEvent::EnterBuildingEvent(string zone, string building)
	: zone(zone), building(building) {

}

EnterBuildingEvent::~EnterBuildingEvent() {

}

string EnterBuildingEvent::GetType() const {
	return "enter_building";
}

bool EnterBuildingEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<EnterBuildingEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (zone.size() > 0 && other->zone.size() > 0) {
		Condition condition;
		condition.ParseCondition(zone);
		auto value = condition.EvaluateValue(getValues);
		if (auto zoneValue = get_if<string>(&value)) {
			result = (result && *zoneValue == other->zone);
		}
		else {
			result = (result && zone == other->zone);
		}
	}
	if (building.size() > 0 && other->building.size() > 0) {
		Condition condition;
		condition.ParseCondition(building);
		auto value = condition.EvaluateValue(getValues);
		if (auto buildingValue = get_if<string>(&value)) {
			result = (result && *buildingValue == other->building);
		}
		else {
			result = (result && building == other->building);
		}
	}
	return result;
}

void EnterBuildingEvent::SetZone(string zone) {
	this->zone = zone;
}

string EnterBuildingEvent::GetZone() const {
	return zone;
}

void EnterBuildingEvent::SetBuilding(string building) {
	this->building = building;
}

string EnterBuildingEvent::GetBuilding() const {
	return building;
}

LeaveBuildingEvent::LeaveBuildingEvent(string zone, string building)
	: zone(zone), building(building) {

}

LeaveBuildingEvent::~LeaveBuildingEvent() {

}

string LeaveBuildingEvent::GetType() const {
	return "leave_building";
}

bool LeaveBuildingEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<LeaveBuildingEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (zone.size() > 0 && other->zone.size() > 0) {
		Condition condition;
		condition.ParseCondition(zone);
		auto value = condition.EvaluateValue(getValues);
		if (auto zoneValue = get_if<string>(&value)) {
			result = (result && *zoneValue == other->zone);
		}
		else {
			result = (result && zone == other->zone);
		}
	}
	if (building.size() > 0 && other->building.size() > 0) {
		Condition condition;
		condition.ParseCondition(building);
		auto value = condition.EvaluateValue(getValues);
		if (auto buildingValue = get_if<string>(&value)) {
			result = (result && *buildingValue == other->building);
		}
		else {
			result = (result && building == other->building);
		}
	}
	return result;
}

void LeaveBuildingEvent::SetZone(string zone) {
	this->zone = zone;
}

string LeaveBuildingEvent::GetZone() const {
	return zone;
}

void LeaveBuildingEvent::SetBuilding(string building) {
	this->building = building;
}

string LeaveBuildingEvent::GetBuilding() const {
	return building;
}

EnterRoomEvent::EnterRoomEvent(string zone, string building, string room)
	: zone(zone), building(building), room(room) {

}

EnterRoomEvent::~EnterRoomEvent() {

}

string EnterRoomEvent::GetType() const {
	return "enter_room";
}

bool EnterRoomEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<EnterRoomEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (zone.size() > 0 && other->zone.size() > 0) {
		Condition condition;
		condition.ParseCondition(zone);
		auto value = condition.EvaluateValue(getValues);
		if (auto zoneValue = get_if<string>(&value)) {
			result = (result && *zoneValue == other->zone);
		}
		else {
			result = (result && zone == other->zone);
		}
	}
	if (building.size() > 0 && other->building.size() > 0) {
		Condition condition;
		condition.ParseCondition(building);
		auto value = condition.EvaluateValue(getValues);
		if (auto buildingValue = get_if<string>(&value)) {
			result = (result && *buildingValue == other->building);
		}
		else {
			result = (result && building == other->building);
		}
	}
	if (room.size() > 0 && other->room.size() > 0) {
		Condition condition;
		condition.ParseCondition(room);
		auto value = condition.EvaluateValue(getValues);
		if (auto roomValue = get_if<string>(&value)) {
			result = (result && *roomValue == other->room);
		}
		else {
			result = (result && room == other->room);
		}
	}
	return result;
}

void EnterRoomEvent::SetZone(string zone) {
	this->zone = zone;
}

string EnterRoomEvent::GetZone() const {
	return zone;
}

void EnterRoomEvent::SetBuilding(string building) {
	this->building = building;
}

string EnterRoomEvent::GetBuilding() const {
	return building;
}

void EnterRoomEvent::SetRoom(string room) {
	this->room = room;
}

string EnterRoomEvent::GetRoom() const {
	return room;
}

LeaveRoomEvent::LeaveRoomEvent(string zone, string building, string room)
	: zone(zone), building(building), room(room) {

}

LeaveRoomEvent::~LeaveRoomEvent() {

}

string LeaveRoomEvent::GetType() const {
	return "leave_room";
}

bool LeaveRoomEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<LeaveRoomEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (zone.size() > 0 && other->zone.size() > 0) {
		Condition condition;
		condition.ParseCondition(zone);
		auto value = condition.EvaluateValue(getValues);
		if (auto zoneValue = get_if<string>(&value)) {
			result = (result && *zoneValue == other->zone);
		}
		else {
			result = (result && zone == other->zone);
		}
	}
	if (building.size() > 0 && other->building.size() > 0) {
		Condition condition;
		condition.ParseCondition(building);
		auto value = condition.EvaluateValue(getValues);
		if (auto buildingValue = get_if<string>(&value)) {
			result = (result && *buildingValue == other->building);
		}
		else {
			result = (result && building == other->building);
		}
	}
	if (room.size() > 0 && other->room.size() > 0) {
		Condition condition;
		condition.ParseCondition(room);
		auto value = condition.EvaluateValue(getValues);
		if (auto roomValue = get_if<string>(&value)) {
			result = (result && *roomValue == other->room);
		}
		else {
			result = (result && room == other->room);
		}
	}
	return result;
}

void LeaveRoomEvent::SetZone(string zone) {
	this->zone = zone;
}

string LeaveRoomEvent::GetZone() const {
	return zone;
}

void LeaveRoomEvent::SetBuilding(string building) {
	this->building = building;
}

string LeaveRoomEvent::GetBuilding() const {
	return building;
}

void LeaveRoomEvent::SetRoom(string room) {
	this->room = room;
}

string LeaveRoomEvent::GetRoom() const {
	return room;
}

DepositChangeEvent::DepositChangeEvent(int result, int delta)
	: result(result), delta(delta) {

}

DepositChangeEvent::~DepositChangeEvent() {

}

string DepositChangeEvent::GetType() const {
	return "deposit_change";
}

bool DepositChangeEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
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

string CashChangeEvent::GetType() const {
	return "cash_change";
}

bool CashChangeEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
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

string GetItemEvent::GetType() const {
	return "get_item";
}

bool GetItemEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<GetItemEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (item.size() > 0 && other->item.size() > 0) {
		Condition condition;
		condition.ParseCondition(item);
		auto value = condition.EvaluateValue(getValues);
		if (auto itemValue = get_if<string>(&value)) {
			result = (result && *itemValue == other->item);
		}
		else {
			result = (result && item == other->item);
		}
	}
	if (num != 0) {
		result = result && (num == other->num);
	}
	return result;
}

void GetItemEvent::SetItem(string item) {
	this->item = item;
}

string GetItemEvent::GetItem() const {
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

string LoseItemEvent::GetType() const {
	return "lose_item";
}

bool LoseItemEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<LoseItemEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (item.size() > 0 && other->item.size() > 0) {
		Condition condition;
		condition.ParseCondition(item);
		auto value = condition.EvaluateValue(getValues);
		if (auto itemValue = get_if<string>(&value)) {
			result = (result && *itemValue == other->item);
		}
		else {
			result = (result && item == other->item);
		}
	}
	if (num != 0) {
		result = result && (num == other->num);
	}
	return result;
}

void LoseItemEvent::SetItem(string item) {
	this->item = item;
}

string LoseItemEvent::GetItem() const {
	return item;
}

void LoseItemEvent::SetNum(int num) {
	this->num = num;
}

int LoseItemEvent::GetNum() {
	return num;
}

PlayerInjuredEvent::PlayerInjuredEvent(string wound) :
	wound(wound) {

}

PlayerInjuredEvent::~PlayerInjuredEvent() {

}

string PlayerInjuredEvent::GetType() const {
	return "player_injured";
}

bool PlayerInjuredEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<PlayerInjuredEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (wound.size() > 0 && other->wound.size() > 0) {
		Condition condition;
		condition.ParseCondition(wound);
		auto value = condition.EvaluateValue(getValues);
		if (auto woundValue = get_if<string>(&value)) {
			result = (result && *woundValue == other->wound);
		}
		else {
			result = (result && wound == other->wound);
		}
	}
	return result;
}

void PlayerInjuredEvent::SetWound(string wound) {
	this->wound = wound;
}

string PlayerInjuredEvent::GetWound() const {
	return wound;
}

PlayerCuredEvent::PlayerCuredEvent(string wound) :
	wound(wound) {

}

PlayerCuredEvent::~PlayerCuredEvent() {

}

string PlayerCuredEvent::GetType() const {
	return "player_cured";
}

bool PlayerCuredEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<PlayerCuredEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (wound.size() > 0 && other->wound.size() > 0) {
		Condition condition;
		condition.ParseCondition(wound);
		auto value = condition.EvaluateValue(getValues);
		if (auto woundValue = get_if<string>(&value)) {
			result = (result && *woundValue == other->wound);
		}
		else {
			result = (result && wound == other->wound);
		}
	}
	return result;
}

void PlayerCuredEvent::SetWound(string wound) {
	this->wound = wound;
}

string PlayerCuredEvent::GetWound() const {
	return wound;
}

PlayerIllEvent::PlayerIllEvent(string illness) :
	illness(illness) {

}

PlayerIllEvent::~PlayerIllEvent() {

}

string PlayerIllEvent::GetType() const {
	return "player_ill";
}

bool PlayerIllEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<PlayerIllEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (illness.size() > 0 && other->illness.size() > 0) {
		Condition condition;
		condition.ParseCondition(illness);
		auto value = condition.EvaluateValue(getValues);
		if (auto illnessValue = get_if<string>(&value)) {
			result = (result && *illnessValue == other->illness);
		}
		else {
			result = (result && illness == other->illness);
		}
	}
	return result;
}

void PlayerIllEvent::SetIllness(string illness) {
	this->illness = illness;
}

string PlayerIllEvent::GetIllness() const {
	return illness;
}

PlayerRecoverEvent::PlayerRecoverEvent(string illness) :
	illness(illness) {

}

PlayerRecoverEvent::~PlayerRecoverEvent() {

}

string PlayerRecoverEvent::GetType() const {
	return "player_recover";
}

bool PlayerRecoverEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<PlayerRecoverEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (illness.size() > 0 && other->illness.size() > 0) {
		Condition condition;
		condition.ParseCondition(illness);
		auto value = condition.EvaluateValue(getValues);
		if (auto illnessValue = get_if<string>(&value)) {
			result = (result && *illnessValue == other->illness);
		}
		else {
			result = (result && illness == other->illness);
		}
	}
	return result;
}

void PlayerRecoverEvent::SetIllness(string illness) {
	this->illness = illness;
}

string PlayerRecoverEvent::GetIllness() const {
	return illness;
}

PlayerRestEvent::PlayerRestEvent(int minute) :
   minute(minute) {

}

PlayerRestEvent::~PlayerRestEvent() {

}

string PlayerRestEvent::GetType() const {
	return "player_rest";
}

bool PlayerRestEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
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

PlayerSleepEvent::PlayerSleepEvent(int hour) :
	hour(hour) {

}

PlayerSleepEvent::~PlayerSleepEvent() {

}

string PlayerSleepEvent::GetType() const {
	return "player_sleep";
}

bool PlayerSleepEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
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

TimeUpEvent::TimeUpEvent(string timer) :
	timer(timer) {

}

TimeUpEvent::~TimeUpEvent() {

}

string TimeUpEvent::GetType() const {
	return "time_up";
}

bool TimeUpEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<TimeUpEvent*>(e);
	if (!other) return false;

	return timer == other->timer;
}

void TimeUpEvent::SetTimer(string timer) {
	this->timer = timer;
}

string TimeUpEvent::GetTimer() const {
	return timer;
}

CountUpEvent::CountUpEvent(string counter) :
	counter(counter) {

}

CountUpEvent::~CountUpEvent() {

}

string CountUpEvent::GetType() const {
	return "count_up";
}

bool CountUpEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<CountUpEvent*>(e);
	if (!other) return false;

	return counter == other->counter;
}

void CountUpEvent::SetCounter(string counter) {
	this->counter = counter;
}

string CountUpEvent::GetCounter() const {
	return counter;
}

UseSkillEvent::UseSkillEvent(string skill) :
	skill(skill) {

}

UseSkillEvent::~UseSkillEvent() {

}

string UseSkillEvent::GetType() const {
	return "use_skill";
}

bool UseSkillEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<UseSkillEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (skill.size() > 0 && other->skill.size() > 0) {
		Condition condition;
		condition.ParseCondition(skill);
		auto value = condition.EvaluateValue(getValues);
		if (auto skillValue = get_if<string>(&value)) {
			result = (result && *skillValue == other->skill);
		}
		else {
			result = (result && skill == other->skill);
		}
	}
	return result;
}

void UseSkillEvent::SetSkill(string skill) {
	this->skill = skill;
}

string UseSkillEvent::GetSkill() const {
	return skill;
}

SkillChangeEvent::SkillChangeEvent(string skill, int level)
	: skill(skill), level(level) {

}

SkillChangeEvent::~SkillChangeEvent() {

}

string SkillChangeEvent::GetType() const {
	return "skill_change";
}

bool SkillChangeEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<SkillChangeEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (skill.size() > 0 && other->skill.size() > 0) {
		Condition condition;
		condition.ParseCondition(skill);
		auto value = condition.EvaluateValue(getValues);
		if (auto skillValue = get_if<string>(&value)) {
			result = (result && *skillValue == other->skill);
		}
		else {
			result = (result && skill == other->skill);
		}
	}
	result = result && (level == other->level);
	return result;
}

void SkillChangeEvent::SetSkill(string skill) {
	this->skill = skill;
}

string SkillChangeEvent::GetSkill() const {
	return skill;
}

void SkillChangeEvent::SetLevel(int level) {
	this->level = level;
}

int SkillChangeEvent::GetLevel() {
	return level;
}

BattleWinEvent::BattleWinEvent(string enemy) :
	enemy(enemy) {

}

BattleWinEvent::~BattleWinEvent() {

}

string BattleWinEvent::GetType() const {
	return "battle_win";
}

bool BattleWinEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<BattleWinEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (enemy.size() > 0 && other->enemy.size() > 0) {
		Condition condition;
		condition.ParseCondition(enemy);
		auto value = condition.EvaluateValue(getValues);
		if (auto enemyValue = get_if<string>(&value)) {
			result = (result && *enemyValue == other->enemy);
		}
		else {
			result = (result && enemy == other->enemy);
		}
	}
	return result;
}

void BattleWinEvent::SetEnemy(string enemy) {
	this->enemy = enemy;
}

string BattleWinEvent::GetEnemy() const {
	return enemy;
}

BattleLoseEvent::BattleLoseEvent(string enemy) :
	enemy(enemy) {

}

BattleLoseEvent::~BattleLoseEvent() {

}

string BattleLoseEvent::GetType() const {
	return "battle_lose";
}

bool BattleLoseEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<BattleLoseEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (enemy.size() > 0 && other->enemy.size() > 0) {
		Condition condition;
		condition.ParseCondition(enemy);
		auto value = condition.EvaluateValue(getValues);
		if (auto enemyValue = get_if<string>(&value)) {
			result = (result && *enemyValue == other->enemy);
		}
		else {
			result = (result && enemy == other->enemy);
		}
	}
	return result;
}

void BattleLoseEvent::SetEnemy(string enemy) {
	this->enemy = enemy;
}

string BattleLoseEvent::GetEnemy() const {
	return enemy;
}

EscapeSuccessEvent::EscapeSuccessEvent(string enemy) :
	enemy(enemy) {

}

EscapeSuccessEvent::~EscapeSuccessEvent() {

}

string EscapeSuccessEvent::GetType() const {
	return "escape_success";
}

bool EscapeSuccessEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<EscapeSuccessEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (enemy.size() > 0 && other->enemy.size() > 0) {
		Condition condition;
		condition.ParseCondition(enemy);
		auto value = condition.EvaluateValue(getValues);
		if (auto enemyValue = get_if<string>(&value)) {
			result = (result && *enemyValue == other->enemy);
		}
		else {
			result = (result && enemy == other->enemy);
		}
	}
	return result;
}

void EscapeSuccessEvent::SetEnemy(string enemy) {
	this->enemy = enemy;
}

string EscapeSuccessEvent::GetEnemy() const {
	return enemy;
}

EscapeFailEvent::EscapeFailEvent(string enemy) :
	enemy(enemy) {

}

EscapeFailEvent::~EscapeFailEvent() {

}

string EscapeFailEvent::GetType() const {
	return "escape_fail";
}

bool EscapeFailEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<EscapeFailEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (enemy.size() > 0 && other->enemy.size() > 0) {
		Condition condition;
		condition.ParseCondition(enemy);
		auto value = condition.EvaluateValue(getValues);
		if (auto enemyValue = get_if<string>(&value)) {
			result = (result && *enemyValue == other->enemy);
		}
		else {
			result = (result && enemy == other->enemy);
		}
	}
	return result;
}

void EscapeFailEvent::SetEnemy(string enemy) {
	this->enemy = enemy;
}

string EscapeFailEvent::GetEnemy() const {
	return enemy;
}

CultivationChangeEvent::CultivationChangeEvent(string method, int level)
	: method(method), level(level) {

}

CultivationChangeEvent::~CultivationChangeEvent() {

}

string CultivationChangeEvent::GetType() const {
	return "cultivation_change";
}

bool CultivationChangeEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<CultivationChangeEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (method.size() > 0 && other->method.size() > 0) {
		Condition condition;
		condition.ParseCondition(method);
		auto value = condition.EvaluateValue(getValues);
		if (auto methodValue = get_if<string>(&value)) {
			result = (result && *methodValue == other->method);
		}
		else {
			result = (result && method == other->method);
		}
	}
	result = result && (level == other->level);
	return result;
}

void CultivationChangeEvent::SetMethod(string method) {
	this->method = method;
}

string CultivationChangeEvent::GetMethod() const {
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

string WantedChangeEvent::GetType() const {
	return "wanted_change";
}

bool WantedChangeEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<WantedChangeEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (reason.size() > 0 && other->reason.size() > 0) {
		Condition condition;
		condition.ParseCondition(reason);
		auto value = condition.EvaluateValue(getValues);
		if (auto reasonValue = get_if<string>(&value)) {
			result = (result && *reasonValue == other->reason);
		}
		else {
			result = (result && reason == other->reason);
		}
	}
	result = result && (level == other->level);
	return result;
}

void WantedChangeEvent::SetReason(string reason) {
	this->reason = reason;
}

string WantedChangeEvent::GetReason() const {
	return reason;
}

void WantedChangeEvent::SetLevel(int level) {
	this->level = level;
}

int WantedChangeEvent::GetLevel() {
	return level;
}

PlayerArrestedEvent::PlayerArrestedEvent(string reason) :
	reason(reason) {

}

PlayerArrestedEvent::~PlayerArrestedEvent() {

}

string PlayerArrestedEvent::GetType() const {
	return "player_arrested";
}

bool PlayerArrestedEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<PlayerArrestedEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (reason.size() > 0 && other->reason.size() > 0) {
		Condition condition;
		condition.ParseCondition(reason);
		auto value = condition.EvaluateValue(getValues);
		if (auto reasonValue = get_if<string>(&value)) {
			result = (result && *reasonValue == other->reason);
		}
		else {
			result = (result && reason == other->reason);
		}
	}
	return result;
}

void PlayerArrestedEvent::SetReason(string reason) {
	this->reason = reason;
}

string PlayerArrestedEvent::GetReason() const {
	return reason;
}

PlayerReleasedEvent::PlayerReleasedEvent(string reason) :
	reason(reason) {

}

PlayerReleasedEvent::~PlayerReleasedEvent() {

}

string PlayerReleasedEvent::GetType() const {
	return "player_released";
}

bool PlayerReleasedEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<PlayerReleasedEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (reason.size() > 0 && other->reason.size() > 0) {
		Condition condition;
		condition.ParseCondition(reason);
		auto value = condition.EvaluateValue(getValues);
		if (auto reasonValue = get_if<string>(&value)) {
			result = (result && *reasonValue == other->reason);
		}
		else {
			result = (result && reason == other->reason);
		}
	}
	return result;
}

void PlayerReleasedEvent::SetReason(string reason) {
	this->reason = reason;
}

string PlayerReleasedEvent::GetReason() const {
	return reason;
}

WeatherChangeEvent::WeatherChangeEvent(string weather) :
	weather(weather) {

}

WeatherChangeEvent::~WeatherChangeEvent() {

}

string WeatherChangeEvent::GetType() const {
	return "weather_change";
}

bool WeatherChangeEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<WeatherChangeEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (weather.size() > 0 && other->weather.size() > 0) {
		Condition condition;
		condition.ParseCondition(weather);
		auto value = condition.EvaluateValue(getValues);
		if (auto weatherValue = get_if<string>(&value)) {
			result = (result && *weatherValue == other->weather);
		}
		else {
			result = (result && weather == other->weather);
		}
	}
	return result;
}

void WeatherChangeEvent::SetWeather(string weather) {
	this->weather = weather;
}

string WeatherChangeEvent::GetWeather() const {
	return weather;
}

PolicyChangeEvent::PolicyChangeEvent(string policy, bool status)
	: policy(policy), status(status) {

}

PolicyChangeEvent::~PolicyChangeEvent() {

}

string PolicyChangeEvent::GetType() const {
	return "policy_change";
}

bool PolicyChangeEvent::Match(Event* e,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	if (!e) return false;
	if (GetType() != e->GetType()) return false;

	auto other = dynamic_cast<PolicyChangeEvent*>(e);
	if (!other) return false;

	bool result = true;
	if (policy.size() > 0 && other->policy.size() > 0) {
		Condition condition;
		condition.ParseCondition(policy);
		auto value = condition.EvaluateValue(getValues);
		if (auto policyValue = get_if<string>(&value)) {
			result = (result && *policyValue == other->policy);
		}
		else {
			result = (result && policy == other->policy);
		}
	}
	result = result && (status == other->status);
	return result;
}

void PolicyChangeEvent::SetPolicy(string policy) {
	this->policy = policy;
}

string PolicyChangeEvent::GetPolicy() const {
	return policy;
}

void PolicyChangeEvent::SetStatus(bool status) {
	this->status = status;
}

bool PolicyChangeEvent::GetStatus() {
	return status;
}

