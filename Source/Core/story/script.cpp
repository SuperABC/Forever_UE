#include "script.h"

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <unordered_map>


using namespace std;

std::unordered_map<std::string, std::pair<std::vector<std::string>, std::unordered_map<std::string, Milestone*>>> Script::libraries = {};

Script::Script() {

}

Script::~Script() {

}

void Script::ReadScript(string name, string path,
    EventFactory* eventFactory, ChangeFactory* changeFactory) {
    if(libraries.find(name) != libraries.end()) {
        return;
    }

    if (!filesystem::exists(path)) {
        THROW_EXCEPTION(IOException, "Path does not exist: " + path + ".\n");
    }

    JsonReader reader;
    JsonValue root;

    ifstream fin(path);
    if (!fin.is_open()) {
        THROW_EXCEPTION(IOException, "Failed to open file: " + path + ".\n");
    }

    if (reader.Parse(fin, root)) {
        unordered_map<string, int> hash;
        hash["game_finish"] = -1;
        hash["game_fail"] = -2;

        auto item = make_pair(name, pair<vector<string>, unordered_map<string, Milestone*>>());
        if (root.GetType() == DATA_ARRAY) {
            for (auto milestone : root) {
                Milestone *content = new Milestone(
                    milestone["milestone"].AsString(),
                    BuildEvent(milestone["triggers"], eventFactory),
                    milestone["visible"].AsBool(),
                    BuildCondition(milestone["drop"].AsString()),
                    milestone["description"].AsString(),
                    milestone["goal"].AsString(),
                    BuildDialogs(milestone["dialogs"], changeFactory),
                    BuildChanges(milestone["changes"], changeFactory),
					BuildSubsequences(milestone["subsequences"])
                );
				item.second.second.insert(make_pair(content->GetName(), content));
            }
        }
        else if (root.GetType() == DATA_OBJECT) {
            vector<string> names;
            for (auto name : root["names"]) {
                names.push_back(name.AsString());
            }
			item.second.first = names;
            for (auto milestone : root["milestones"]) {
                Milestone* content = new Milestone(
                    milestone["milestone"].AsString(),
                    BuildEvent(milestone["triggers"], eventFactory),
                    milestone["visible"].AsBool(),
                    BuildCondition(milestone["drop"].AsString()),
                    milestone["description"].AsString(),
                    milestone["goal"].AsString(),
                    BuildDialogs(milestone["dialogs"], changeFactory),
                    BuildChanges(milestone["changes"], changeFactory),
                    BuildSubsequences(milestone["subsequences"])
                );
                item.second.second.insert(make_pair(content->GetName(), content));
            }
        }
        else {
            THROW_EXCEPTION(JsonFormatException, "Json syntax error: Root element must be array or object.\n");
        }
		libraries.insert(item);
    }
    else {
        THROW_EXCEPTION(JsonFormatException, "Json syntax error: " + reader.GetErrorMessages() + ".\n");
    }
    fin.close();
}

vector<string> Script::ReadNames(string name, string path,
    EventFactory* eventFactory, ChangeFactory* changeFactory) {
	ReadScript(name, path, eventFactory, changeFactory);
    if (libraries.find(name) == libraries.end()) {
        THROW_EXCEPTION(RuntimeException, "Read script failed: " + path + "\n");
	}
	return libraries[name].first;
}

void Script::ReadMilestones(string name, string path,
    EventFactory* eventFactory, ChangeFactory* changeFactory) {
    ReadScript(name, path, eventFactory, changeFactory);
    if (libraries.find(name) == libraries.end()) {
        THROW_EXCEPTION(RuntimeException, "Read script failed: " + path + "\n");
    }

    for (auto milestone : libraries[name].second) {
        milestones[milestone.first] = MilestoneNode(milestone.second);
    }
    for (auto milestone : milestones) {
        for (auto subsequence : milestone.second.content->GetSubsequences()) {
            if (milestones.find(subsequence) == milestones.end())continue;
			milestones[milestone.first].subsequents.push_back(&milestones[subsequence]);
			milestones[subsequence].premise++;
        }
    }
    for (auto& milestone : milestones) {
        if (milestone.second.premise == 0) {
            actives.push_back(&milestone.second);
        }
    }
}

pair<vector<Dialog>, vector<Change*>> Script::MatchEvent(
    Event* event, Story *story) {
	pair<vector<Dialog>, vector<Change*>> results;
	results.first.clear();
	results.second.clear();

	vector<MilestoneNode*> tmps;
	for (auto it = actives.begin(); it != actives.end(); ) {
		bool match = false;
		for (auto trigger : (*it)->content->GetTriggers()) {
			if (!story->JudgeCondition(trigger->GetCondition())) {
				continue;
			}

			if ((*it)->content->MatchTrigger(event)) {
				match = true;
				break;
			}
		}

		if (match) {
			auto subsequents = (*it)->subsequents;
			for (auto subsequent : subsequents) {
				subsequent->premise--;
				if (subsequent->premise <= 0) {
					tmps.push_back(subsequent);
				}
			}

			auto dialogs = (*it)->content->GetDialogs();
			results.first.insert(results.first.end(), dialogs.begin(), dialogs.end());
			auto changes = (*it)->content->GetChanges();
			results.second.insert(results.second.end(), changes.begin(), changes.end());

			if ((*it)->content->DropSelf([&story](string name) -> pair<bool, ValueType> {
				return story->GetValue(name);
				})) {
				it = actives.erase(it);
			}
			else {
				it++;
			}
		}
		else {
			it++;
		}
	}
	actives.insert(actives.end(), tmps.begin(), tmps.end());

	return results;
}

pair<vector<Dialog>, vector<Change*>> Script::MatchEvent(
    Event* event, Story* story, Person* person) {
	pair<vector<Dialog>, vector<Change*>> results;
	results.first.clear();
	results.second.clear();

	vector<MilestoneNode*> tmps;
	for (auto it = actives.begin(); it != actives.end(); ) {
		bool match = false;
		for (auto trigger : (*it)->content->GetTriggers()) {
			if (!story->JudgeCondition(trigger->GetCondition(), person)) {
				continue;
			}

			if ((*it)->content->MatchTrigger(event)) {
				match = true;
				break;
			}
		}

		if (match) {
			auto subsequents = (*it)->subsequents;
			for (auto subsequent : subsequents) {
				subsequent->premise--;
				if (subsequent->premise <= 0) {
					tmps.push_back(subsequent);
				}
			}

			auto dialogs = (*it)->content->GetDialogs();
			results.first.insert(results.first.end(), dialogs.begin(), dialogs.end());
			auto changes = (*it)->content->GetChanges();
			results.second.insert(results.second.end(), changes.begin(), changes.end());

			if ((*it)->content->DropSelf([&story](string name) -> pair<bool, ValueType> {
				return story->GetValue(name);
				})) {
				it = actives.erase(it);
			}
			else {
				it++;
			}
		}
		else {
			it++;
		}
	}
	actives.insert(actives.end(), tmps.begin(), tmps.end());

	return results;
}

void Script::DeactivateMilestone(const std::string& name) {
	for (auto it = actives.begin(); it != actives.end(); ) {
		if ((*it)->content->GetName() == name) {
			it = actives.erase(it);
			return;
		}
		else {
			it++;
		}
	}
}

vector<Event*> Script::BuildEvent(JsonValue root, EventFactory* factory) const {
    vector<Event*> events;

    for (auto obj : root) {
        Event* event = nullptr;
        string type = obj["type"].AsString();

        if (type == "game_start") {
            event = new GameStartEvent();
        }
        else if (type == "game_success") {
            auto result = obj["result"];
            if (result.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing result for game_success event.\n");
            }
            event = new GameSuccessEvent(obj["result"].AsString());
        }
        else if (type == "game_fail") {
            auto result = obj["result"];
            if (result.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing result for game_fail event.\n");
            }
            event = new GameFailEvent(obj["result"].AsString());
        }
        else if (type == "option_dialog") {
            auto target = obj["target"];
            auto option = obj["option"];
            if (option.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing option for option_dialog event.\n");
            }
            event = new OptionDialogEvent(obj["target"].AsString(), obj["option"].AsString());
        }
        else if (type == "npc_meet") {
            auto npc = obj["npc"];
            if (npc.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing npc for npc_meet event.\n");
            }
            event = new NPCMeetEvent(obj["npc"].AsString());
        }
        else if (type == "citizen_born") {
            auto name = obj["name"];
            auto gender = obj["gender"];
            if (name.IsNull() || gender.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing name or gender for citizen_born event.\n");
            }
            // 需要根据实际情况转换GENDER_TYPE
            event = new CitizenBornEvent(obj["name"].AsString(),
                static_cast<GENDER_TYPE>(obj["gender"].AsInt()));
        }
        else if (type == "citizen_decease") {
            auto name = obj["name"];
            auto reason = obj["reason"];
            if (name.IsNull() || reason.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing name or reason for citizen_decease event.\n");
            }
            event = new CitizenDeceaseEvent(obj["name"].AsString(), obj["reason"].AsString());
        }
        else if (type == "enter_zone") {
            auto zone = obj["zone"];
            if (zone.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing zone for enter_zone event.\n");
            }
            event = new EnterZoneEvent(obj["zone"].AsString());
        }
        else if (type == "leave_zone") {
            auto zone = obj["zone"];
            if (zone.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing zone for leave_zone event.\n");
            }
            event = new LeaveZoneEvent(obj["zone"].AsString());
        }
        else if (type == "enter_building") {
            auto zone = obj["zone"];
            auto building = obj["building"];
            if (zone.IsNull() || building.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing zone or building for enter_building event.\n");
            }
            event = new EnterBuildingEvent(obj["zone"].AsString(), obj["building"].AsString());
        }
        else if (type == "leave_building") {
            auto zone = obj["zone"];
            auto building = obj["building"];
            if (zone.IsNull() || building.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing zone or building for leave_building event.\n");
            }
            event = new LeaveBuildingEvent(obj["zone"].AsString(), obj["building"].AsString());
        }
        else if (type == "enter_room") {
            auto zone = obj["zone"];
            auto building = obj["building"];
            auto room = obj["room"];
            if (zone.IsNull() || building.IsNull() || room.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing zone, building or room for enter_room event.\n");
            }
            event = new EnterRoomEvent(obj["zone"].AsString(), obj["building"].AsString(), obj["room"].AsString());
        }
        else if (type == "leave_room") {
            auto zone = obj["zone"];
            auto building = obj["building"];
            auto room = obj["room"];
            if (zone.IsNull() || building.IsNull() || room.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing zone, building or room for leave_room event.\n");
            }
            event = new LeaveRoomEvent(obj["zone"].AsString(), obj["building"].AsString(), obj["room"].AsString());
        }
        else if (type == "deposit_change") {
            auto result = obj["result"];
            auto delta = obj["delta"];
            if (result.IsNull() || delta.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing result or delta for deposit_change event.\n");
            }
            event = new DepositChangeEvent(obj["result"].AsInt(), obj["delta"].AsInt());
        }
        else if (type == "cash_change") {
            auto result = obj["result"];
            auto delta = obj["delta"];
            if (result.IsNull() || delta.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing result or delta for cash_change event.\n");
            }
            event = new CashChangeEvent(obj["result"].AsInt(), obj["delta"].AsInt());
        }
        else if (type == "get_item") {
            auto item = obj["item"];
            auto num = obj["num"];
            if (item.IsNull() || num.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing item or num for get_item event.\n");
            }
            event = new GetItemEvent(obj["item"].AsString(), obj["num"].AsInt());
        }
        else if (type == "lose_item") {
            auto item = obj["item"];
            auto num = obj["num"];
            if (item.IsNull() || num.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing item or num for lose_item event.\n");
            }
            event = new LoseItemEvent(obj["item"].AsString(), obj["num"].AsInt());
        }
        else if (type == "player_injured") {
            auto wound = obj["wound"];
            if (wound.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing wound for player_injured event.\n");
            }
            event = new PlayerInjuredEvent(obj["wound"].AsString());
        }
        else if (type == "player_cured") {
            auto wound = obj["wound"];
            if (wound.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing wound for player_cured event.\n");
            }
            event = new PlayerCuredEvent(obj["wound"].AsString());
        }
        else if (type == "player_ill") {
            auto illness = obj["illness"];
            if (illness.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing illness for player_ill event.\n");
            }
            event = new PlayerIllEvent(obj["illness"].AsString());
        }
        else if (type == "player_recover") {
            auto illness = obj["illness"];
            if (illness.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing illness for player_recover event.\n");
            }
            event = new PlayerRecoverEvent(obj["illness"].AsString());
        }
        else if (type == "player_rest") {
            auto minute = obj["minute"];
            if (minute.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing minute for player_rest event.\n");
            }
            event = new PlayerRestEvent(obj["minute"].AsInt());
        }
        else if (type == "player_sleep") {
            auto hour = obj["hour"];
            if (hour.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing hour for player_sleep event.\n");
            }
            event = new PlayerSleepEvent(obj["hour"].AsInt());
        }
        else if (type == "time_up") {
            auto timer = obj["timer"];
            if (timer.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing timer for time_up event.\n");
            }
            event = new TimeUpEvent(obj["timer"].AsString());
        }
        else if (type == "count_up") {
            auto counter = obj["counter"];
            if (counter.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing counter for count_up event.\n");
            }
            event = new CountUpEvent(obj["counter"].AsString());
        }
        else if (type == "use_skill") {
            auto skill = obj["skill"];
            if (skill.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing skill for use_skill event.\n");
            }
            event = new UseSkillEvent(obj["skill"].AsString());
        }
        else if (type == "skill_change") {
            auto skill = obj["skill"];
            auto level = obj["level"];
            if (skill.IsNull() || level.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing skill or level for skill_change event.\n");
            }
            event = new SkillChangeEvent(obj["skill"].AsString(), obj["level"].AsInt());
        }
        else if (type == "battle_win") {
            auto enemy = obj["enemy"];
            if (enemy.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing enemy for battle_win event.\n");
            }
            event = new BattleWinEvent(obj["enemy"].AsString());
        }
        else if (type == "battle_lose") {
            auto enemy = obj["enemy"];
            if (enemy.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing enemy for battle_lose event.\n");
            }
            event = new BattleLoseEvent(obj["enemy"].AsString());
        }
        else if (type == "escape_success") {
            auto enemy = obj["enemy"];
            if (enemy.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing enemy for escape_success event.\n");
            }
            event = new EscapeSuccessEvent(obj["enemy"].AsString());
        }
        else if (type == "escape_fail") {
            auto enemy = obj["enemy"];
            if (enemy.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing enemy for escape_fail event.\n");
            }
            event = new EscapeFailEvent(obj["enemy"].AsString());
        }
        else if (type == "cultivation_change") {
            auto method = obj["method"];
            auto level = obj["level"];
            if (method.IsNull() || level.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing method or level for cultivation_change event.\n");
            }
            event = new CultivationChangeEvent(obj["method"].AsString(), obj["level"].AsInt());
        }
        else if (type == "wanted_change") {
            auto reason = obj["reason"];
            auto level = obj["level"];
            if (reason.IsNull() || level.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing reason or level for wanted_change event.\n");
            }
            event = new WantedChangeEvent(obj["reason"].AsString(), obj["level"].AsInt());
        }
        else if (type == "player_arrested") {
            auto reason = obj["reason"];
            if (reason.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing reason for player_arrested event.\n");
            }
            event = new PlayerArrestedEvent(obj["reason"].AsString());
        }
        else if (type == "player_released") {
            auto reason = obj["reason"];
            if (reason.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing reason for player_released event.\n");
            }
            event = new PlayerReleasedEvent(obj["reason"].AsString());
        }
        else if (type == "weather_change") {
            auto weather = obj["weather"];
            if (weather.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing weather for weather_change event.\n");
            }
            event = new WeatherChangeEvent(obj["weather"].AsString());
        }
        else if (type == "policy_change") {
            auto policy = obj["policy"];
            auto status = obj["status"];
            if (policy.IsNull() || status.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing policy or status for policy_change event.\n");
            }
            event = new PolicyChangeEvent(obj["policy"].AsString(), obj["status"].AsBool());
        }
        else if (factory->CheckRegistered(type)) {
            event = factory->CreateEvent(type);
        }

        if (!event) {
            THROW_EXCEPTION(InvalidArgumentException, "Invalid event type: " + type + ".\n");
        }

        // 设置条件
        auto condition = obj["condition"];
        if (!condition.IsNull()) {
            event->SetCondition(BuildCondition(condition));
        }

        events.push_back(event);
    }

    return events;
}

vector<Dialog> Script::BuildDialogs(JsonValue root, ChangeFactory* factory) const {
	vector<Dialog> dialogs;

	for (auto obj : root) {
		Dialog dialog;

		dialog.SetCondition(BuildCondition(obj["condition"]));

		for (auto section : obj["list"]) {
			if (section.IsObject()) {
				dialog.AddDialog(section["speaker"].AsString(), section["content"].AsString());
			}
			else if (section.IsArray()) {
				vector<Option> options;
				for (auto item : section) {
					options.emplace_back(BuildCondition(item["condition"]), item["option"].AsString(),
						BuildDialogs(item["dialogs"], factory), BuildChanges(item["changes"], factory));
				}
				dialog.AddDialog(options);
			}
		}

		dialogs.push_back(dialog);
	}

	return dialogs;
}

vector<Change*> Script::BuildChanges(JsonValue root, ChangeFactory* factory) const {
    vector<Change*> changes;

    for (auto obj : root) {
        string type = obj["type"].AsString();
        Change* change = nullptr;

        if (type == "set_value") {
            auto variable = obj["variable"];
            auto value = obj["value"];
            if (variable.IsNull() || value.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing variable or value for set_value change.\n");
            }
            change = new SetValueChange(obj["variable"].AsString(), obj["value"].AsString());
        }
        else if (type == "remove_value") {
            auto variable = obj["variable"];
            if (variable.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing variable for remove_value change.\n");
            }
            change = new RemoveValueChange(obj["variable"].AsString());
        }
        else if (type == "deactivate_milestone") {
            auto milestone = obj["milestone"];
            if (milestone.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing milestone for deactivate_milestone change.\n");
            }
            change = new DeactivateMilestoneChange(obj["milestone"].AsString());
        }
        else if (type == "add_option") {
            auto target = obj["target"];
            auto option = obj["option"];
            if (target.IsNull() || option.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing target or option for add_option change.\n");
            }
            change = new AddOptionChange(obj["target"].AsString(), obj["option"].AsString());
        }
        else if (type == "remove_option") {
            auto target = obj["target"];
            auto option = obj["option"];
            if (target.IsNull() || option.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing target or option for remove_option change.\n");
            }
            change = new RemoveOptionChange(obj["target"].AsString(), obj["option"].AsString());
        }
        else if (type == "spawn_npc") {
            auto target = obj["target"];
            if (target.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing target for spawn_npc change.\n");
            }
            string gender = obj["gender"].IsNull() ? "" : obj["gender"].AsString();
            string birthday = obj["birthday"].IsNull() ? "" : obj["birthday"].AsString();
            string avatar = obj["avatar"].IsNull() ? "" : obj["avatar"].AsString();
            change = new SpawnNpcChange(obj["target"].AsString(), gender, birthday, avatar);
        }
        else if (type == "remove_npc") {
            auto target = obj["target"];
            if (target.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing target for remove_npc change.\n");
            }
            change = new RemoveNpcChange(obj["target"].AsString());
        }
        else if (type == "npc_navigate") {
            auto target = obj["target"];
            auto destination = obj["destination"];
            if (target.IsNull() || destination.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing target or destination for npc_navigate change.\n");
            }
            change = new NPCNavigateChange(obj["target"].AsString(), obj["destination"].AsString());
        }
        else if (type == "teleport_citizen") {
            auto target = obj["target"];
            auto destination = obj["destination"];
            if (target.IsNull() || destination.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing target or destination for teleport_citizen change.\n");
            }
            change = new TeleportCitizenChange(obj["target"].AsString(), obj["destination"].AsString());
        }
        else if (type == "teleport_player") {
            auto destination = obj["destination"];
            if (destination.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing destination for teleport_player change.\n");
            }
            change = new TeleportPlayerChange(obj["destination"].AsString());
        }
        else if (type == "bank_transaction") {
            auto amount = obj["amount"];
            if (amount.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing amount for bank_transaction change.\n");
            }
            change = new BankTransactionChange(obj["amount"].AsInt());
        }
        else if (type == "cash_transaction") {
            auto amount = obj["amount"];
            if (amount.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing amount for cash_transaction change.\n");
            }
            change = new CashTransactionChange(obj["amount"].AsInt());
        }
        else if (type == "give_item") {
            auto item = obj["item"];
            auto num = obj["num"];
            if (item.IsNull() || num.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing item or num for give_item change.\n");
            }
            change = new GiveItemChange(obj["item"].AsString(), obj["num"].AsInt());
        }
        else if (type == "remove_item") {
            auto item = obj["item"];
            auto num = obj["num"];
            if (item.IsNull() || num.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing item or num for remove_item change.\n");
            }
            change = new RemoveItemChange(obj["item"].AsString(), obj["num"].AsInt());
        }
        else if (type == "player_injured") {
            auto wound = obj["wound"];
            if (wound.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing wound for player_injured change.\n");
            }
            change = new PlayerInjuredChange(obj["wound"].AsString());
        }
        else if (type == "player_cured") {
            auto wound = obj["wound"];
            if (wound.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing wound for player_cured change.\n");
            }
            change = new PlayerCuredChange(obj["wound"].AsString());
        }
        else if (type == "player_ill") {
            auto illness = obj["illness"];
            if (illness.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing illness for player_ill change.\n");
            }
            change = new PlayerIllChange(obj["illness"].AsString());
        }
        else if (type == "player_recover") {
            auto illness = obj["illness"];
            if (illness.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing illness for player_recover change.\n");
            }
            change = new PlayerRecoverChange(obj["illness"].AsString());
        }
        else if (type == "player_sleep") {
            auto hour = obj["hour"];
            if (hour.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing hour for player_sleep change.\n");
            }
            change = new PlayerSleepChange(obj["hour"].AsInt());
        }
        else if (type == "create_timer") {
            auto timer = obj["timer"];
            auto duration = obj["duration"];
            if (timer.IsNull() || duration.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing timer or duration for create_timer change.\n");
            }
            int loop = obj["loop"].IsNull() ? 1 : obj["loop"].AsInt();
            Time durationTime = Time(obj["duration"].AsString());
            change = new CreateTimerChange(obj["timer"].AsString(), durationTime, loop);
        }
        else if (type == "pause_timer") {
            auto timer = obj["timer"];
            if (timer.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing timer for pause_timer change.\n");
            }
            change = new PauseTimerChange(obj["timer"].AsString());
        }
        else if (type == "resume_timer") {
            auto timer = obj["timer"];
            if (timer.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing timer for resume_timer change.\n");
            }
            change = new ResumeTimerChange(obj["timer"].AsString());
        }
        else if (type == "remove_timer") {
            auto timer = obj["timer"];
            if (timer.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing timer for remove_timer change.\n");
            }
            change = new RemoveTimerChange(obj["timer"].AsString());
        }
        else if (type == "reset_timer") {
            auto timer = obj["timer"];
            if (timer.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing timer for reset_timer change.\n");
            }
            change = new ResetTimerChange(obj["timer"].AsString());
        }
        else if (type == "create_counter") {
            auto counter = obj["counter"];
            auto count = obj["count"];
            if (counter.IsNull() || count.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing counter or count for create_counter change.\n");
            }
            change = new CreateCounterChange(obj["counter"].AsString(), obj["count"].AsInt());
        }
        else if (type == "call_counter") {
            auto counter = obj["counter"];
            auto delta = obj["delta"];
            if (counter.IsNull() || delta.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing counter or delta for call_counter change.\n");
            }
            change = new CallCounterChange(obj["counter"].AsString(), obj["delta"].AsInt());
        }
        else if (type == "remove_counter") {
            auto counter = obj["counter"];
            if (counter.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing counter for remove_counter change.\n");
            }
            change = new RemoveCounterChange(obj["counter"].AsString());
        }
        else if (type == "enter_battle") {
            auto enemy = obj["enemy"];
            if (enemy.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing enemy for enter_battle change.\n");
            }
            change = new EnterBattleChange(obj["enemy"].AsString());
        }
        else if (type == "change_time") {
            auto delta = obj["delta"];
            if (delta.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing delta for change_time change.\n");
            }
            Time deltaTime = Time(obj["delta"].AsString());
            change = new ChangeTimeChange(deltaTime);
        }
        else if (type == "change_cultivation") {
            auto method = obj["method"];
            auto level = obj["level"];
            if (method.IsNull() || level.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing method or level for change_cultivation change.\n");
            }
            change = new ChangeCultivationChange(obj["method"].AsString(), obj["level"].AsInt());
        }
        else if (type == "change_wanted") {
            auto reason = obj["reason"];
            auto level = obj["level"];
            if (reason.IsNull() || level.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing reason or level for change_wanted change.\n");
            }
            change = new ChangeWantedChange(obj["reason"].AsString(), obj["level"].AsInt());
        }
        else if (type == "change_weather") {
            auto weather = obj["weather"];
            if (weather.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing weather for change_weather change.\n");
            }
            change = new ChangeWeatherChange(obj["weather"].AsString());
        }
        else if (type == "change_policy") {
            auto policy = obj["policy"];
            if (policy.IsNull()) {
                THROW_EXCEPTION(InvalidInputException, "Missing policy for change_policy change.\n");
            }
            change = new ChangePolicyChange(obj["policy"].AsString());
        }
        else if (factory->CheckRegistered(type)) {
            change = factory->CreateChange(type);
        }

        if (!change) {
            THROW_EXCEPTION(InvalidArgumentException, "Invalid change type: " + type + ".\n");
        }

        change->SetCondition(BuildCondition(obj["condition"]));
        changes.push_back(change);
    }

    return changes;
}

Condition Script::BuildCondition(JsonValue root) const {
	Condition condition;

	condition.ParseCondition(root.AsString());

	return condition;
}

vector<string> Script::BuildSubsequences(JsonValue root) const {
    vector<string> subsequences;
    for (auto obj : root) {
        subsequences.push_back(obj.AsString());
    }
    return subsequences;
}
