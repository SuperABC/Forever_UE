#include "script.h"

#include "story/event.h"
#include "story/dialog.h"
#include "story/change.h"

#include <fstream>
#include <filesystem>
#include <regex>


using namespace std;

unordered_map<string, pair<vector<string>, unordered_map<string, Milestone*>>> Script::caches = {};

Script::Script(ScriptFactory* factory, const string& script) :
	mod(factory->CreateScript(script)),
	factory(factory),
	type(),
	name(),
	milestones(),
	actives() {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Script " + script + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Script::~Script() {
	factory->DestroyScript(mod);
}

string Script::GetType() const {
	return type;
}

string Script::GetName() const {
	return name;
}

pair<bool, ValueType> Script::GetValue(const string& name) const {
	auto it = variables.find(name);
	if (it != variables.end()) {
		return { true, it->second };
	}
	return { false, 0 };
}

map<string, ValueType> Script::GetValues(const string& reg) const {
	map<string, ValueType> values;

	if (reg.empty()) {
		return values;
	}

	try {
		regex pattern(reg);

		for (const auto& [key, val] : variables) {
			if (regex_match(key, pattern)) {
				values.insert({ key, val });
			}
		}
	}
	catch (const regex_error&) {
		THROW_EXCEPTION(InvalidArgumentException, "Invalid regular expression " + reg + ".\n");
	}

	return values;
}

void Script::SetValue(const string& name, ValueType value) {
	variables[name] = value;
}

void Script::RemoveValue(const string& name) {
	variables.erase(name);
}

string Script::GetTask() const {
	string task;
	for (auto active : actives) {
		auto goal = active->content->GetGoal();
		if (goal.size() > 0) {
			task += "目标：" + goal + "\n";

			auto description = active->content->GetDescription();
			if (description.size() > 0) {
				task += "详细内容：" + description + "\n";
			}
		}
	}
	return task;
}

vector<ScriptAction>& Script::WrapScript(Event* event, const vector<ScriptAction>& actions,
	const vector<function<pair<bool, ValueType>(const string&)>>& getValues,
	PostHandle* post) {
	mod->WrapScript(event, actions, getValues, post);
	return mod->actionStack.back();
}

void Script::AutoPop() {
	mod->AutoPop();
}

void Script::ReadScript(const string& path) {
	if (path.empty()) {
		return;
	}
	if (caches.find(path) != caches.end()) {
		return;
	}

	if (!filesystem::exists(path)) {
		debugf("Warning: Script path does not exist: %s.\n", path.data());
		return;
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

		auto item = make_pair(path, pair<vector<string>, unordered_map<string, Milestone*>>());
		if (root.GetType() == DATA_ARRAY) {
			for (auto milestone : root) {
				Milestone* content = new Milestone(
					milestone["milestone"].AsString(),
					BuildEvent(milestone["triggers"]),
					milestone["visible"].AsBool(),
					BuildCondition(milestone["drop"].AsString()),
					milestone["description"].AsString(),
					milestone["goal"].AsString(),
					BuildDialogs(milestone["dialogs"]),
					BuildChanges(milestone["changes"]),
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
					BuildEvent(milestone["triggers"]),
					milestone["visible"].AsBool(),
					BuildCondition(milestone["drop"].AsString()),
					milestone["description"].AsString(),
					milestone["goal"].AsString(),
					BuildDialogs(milestone["dialogs"]),
					BuildChanges(milestone["changes"]),
					BuildSubsequences(milestone["subsequences"])
				);
				item.second.second.insert(make_pair(content->GetName(), content));
			}
		}
		else {
			for (auto& [_, ms] : item.second.second) {
				delete ms;
			}
			fin.close();
			THROW_EXCEPTION(JsonFormatException, "Json syntax error: Root element must be array or object.\n");
		}
		caches.insert(item);
	}
	else {
		fin.close();
		THROW_EXCEPTION(JsonFormatException, "Json syntax error: " + reader.GetErrorMessages() + ".\n");
	}
	fin.close();
}

vector<string> Script::ReadNames(const string& path) {
	if (path.empty()) {
		return {};
	}
	ReadScript(path);
	if (caches.find(path) == caches.end()) {
		debugf("Warning: Read script failed: %s.\n", path.data());
		return {};
	}

	return caches[path].first;
}

void Script::ReadMilestones(const string& path) {
	if (path.empty()) {
		return;
	}
	ReadScript(path);
	if (caches.find(path) == caches.end()) {
		debugf("Warning: Read script failed: %s.\n", path.data());
		return;
	}

	actives.clear();
	for (auto& [_, node] : milestones) {
		node.subsequents.clear();
		node.premise = 0;
	}

	for (auto& [msName, content] : caches[path].second) {
		milestones[msName] = MilestoneNode(content);
	}
	for (auto& [name, node] : milestones) {
		for (auto subsequence : node.content->GetSubsequences()) {
			if (milestones.find(subsequence) == milestones.end())continue;
			milestones[name].subsequents.push_back(&milestones[subsequence]);
			milestones[subsequence].premise++;
		}
	}
	for (auto& [_, node] : milestones) {
		if (node.premise == 0) {
			actives.push_back(&node);
		}
	}
}

vector<ScriptAction> Script::MatchEvent(Event* event,
	const vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	vector<ScriptAction> actions;

	vector<MilestoneNode*> tmps;
	for (auto it = actives.begin(); it != actives.end(); ) {
		bool match = false;
		for (auto trigger : (*it)->content->GetTriggers()) {
			if (!trigger->GetCondition().EvaluateBool(getValues)) {
				continue;
			}

			if ((*it)->content->MatchTrigger(event, getValues)) {
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

			auto changes = (*it)->content->GetChanges();
			actions.insert(actions.end(), changes.begin(), changes.end());
			auto dialogs = (*it)->content->GetDialogs();
			actions.insert(actions.end(), dialogs.begin(), dialogs.end());

			if ((*it)->content->DropSelf(getValues)) {
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

	return actions;
}

void Script::DeactivateMilestone(const string& name) {
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

void Script::ClearContext() {
	milestones.clear();
	actives.clear();
	variables.clear();
}

vector<Event*> Script::BuildEvent(JsonValue root) {
	vector<Event*> events;

	for (auto obj : root) {
		Event* event = nullptr;
		string type = obj["type"].AsString();

		if (type == "game_start") {
			event = new GameStartEvent();
		}
		else if (type == "global_message") {
			auto message = obj["message"];
			if (message.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing message for global_message event.\n");
			}
			event = new GlobalMessageEvent(message.AsString());
		}
		else if (type == "option_dialog") {
			auto id = obj["id"];
			auto name = obj["name"];
			auto option = obj["option"];
			if (id.IsNull()) {
				event = new OptionDialogEvent(name.AsString(), obj["option"].AsString());
			}
			else {
				event = new OptionDialogEvent(id.AsInt(), obj["option"].AsString());
			}
		}
		else if (type == "global_dialog") {
			auto name = obj["name"];
			auto option = obj["option"];
			if (name.IsNull() || option.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing name or option for global_dialog event.\n");
			}
			event = new GlobalDialogEvent(name.AsString(), option.AsString());
		}
		else if(type == "speaking_finish"){
			auto label = obj["label"];
			if (label.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing label for speaking_finish event.\n");
			}
			event = new SpeakingFinishEvent(label.AsString());
		}
		else if (type == "npc_meet") {
			auto npc = obj["npc"];
			if (npc.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing npc for npc_meet event.\n");
			}
			event = new NPCMeetEvent(npc.AsString());
		}
		else if (type == "citizen_born") {
			auto name = obj["name"];
			if (name.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing name for citizen_born event.\n");
			}
			event = new CitizenBornEvent(name.AsString());
		}
		else if (type == "citizen_decease") {
			auto name = obj["name"];
			auto reason = obj["reason"];
			if (name.IsNull() || reason.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing name or reason for citizen_decease event.\n");
			}
			event = new CitizenDeceaseEvent(name.AsString(), reason.AsString());
		}
		else if (type == "enter_zone") {
			auto zone = obj["zone"];
			event = new EnterZoneEvent(zone.AsString());
		}
		else if (type == "leave_zone") {
			auto zone = obj["zone"];
			event = new LeaveZoneEvent(zone.AsString());
		}
		else if (type == "enter_building") {
			auto zone = obj["zone"];
			auto building = obj["building"];
			event = new EnterBuildingEvent(zone.AsString(), building.AsString());
		}
		else if (type == "leave_building") {
			auto zone = obj["zone"];
			auto building = obj["building"];
			event = new LeaveBuildingEvent(zone.AsString(), building.AsString());
		}
		else if (type == "enter_room") {
			auto zone = obj["zone"];
			auto building = obj["building"];
			auto room = obj["room"];
			event = new EnterRoomEvent(zone.AsString(), building.AsString(), room.AsString());
		}
		else if (type == "leave_room") {
			auto zone = obj["zone"];
			auto building = obj["building"];
			auto room = obj["room"];
			event = new LeaveRoomEvent(zone.AsString(), building.AsString(), room.AsString());
		}
		else if (type == "player_injured") {
			auto wound = obj["wound"];
			if (wound.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing wound for player_injured event.\n");
			}
			event = new PlayerInjuredEvent(wound.AsString());
		}
		else if (type == "player_cured") {
			auto wound = obj["wound"];
			if (wound.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing wound for player_cured event.\n");
			}
			event = new PlayerCuredEvent(wound.AsString());
		}
		else if (type == "player_ill") {
			auto illness = obj["illness"];
			if (illness.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing illness for player_ill event.\n");
			}
			event = new PlayerIllEvent(illness.AsString());
		}
		else if (type == "player_recover") {
			auto illness = obj["illness"];
			if (illness.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing illness for player_recover event.\n");
			}
			event = new PlayerRecoverEvent(illness.AsString());
		}
		else if (type == "player_rest") {
			auto minute = obj["minute"];
			if (minute.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing minute for player_rest event.\n");
			}
			event = new PlayerRestEvent(minute.AsInt());
		}
		else if (type == "player_sleep") {
			auto hour = obj["hour"];
			if (hour.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing hour for player_sleep event.\n");
			}
			event = new PlayerSleepEvent(hour.AsInt());
		}
		else if (type == "time_up") {
			auto name = obj["name"];
			if (name.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing name for time_up event.\n");
			}
			event = new TimeUpEvent(name.AsString());
		}
		else if (type == "battle_win") {
			auto enemy = obj["enemy"];
			if (enemy.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing enemy for battle_win event.\n");
			}
			event = new BattleWinEvent(enemy.AsString());
		}
		else if (type == "battle_lose") {
			auto enemy = obj["enemy"];
			if (enemy.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing enemy for battle_lose event.\n");
			}
			event = new BattleLoseEvent(enemy.AsString());
		}
		else if (type == "puzzle_result") {
			auto result = obj["result"];
			event = new PuzzleResultEvent(result.IsNull() ? 0 : result.AsInt());
		}
		else if (type == "cultivation_change") {
			auto method = obj["method"];
			auto level = obj["level"];
			if (method.IsNull() || level.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing method or level for cultivation_change event.\n");
			}
			event = new CultivationChangeEvent(method.AsString(), level.AsInt());
		}
		else if (type == "wanted_change") {
			auto reason = obj["reason"];
			auto level = obj["level"];
			if (reason.IsNull() || level.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing reason or level for wanted_change event.\n");
			}
			event = new WantedChangeEvent(reason.AsString(), level.AsInt());
		}
		else if (type == "player_arrested") {
			auto reason = obj["reason"];
			if (reason.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing reason for player_arrested event.\n");
			}
			event = new PlayerArrestedEvent(reason.AsString());
		}
		else if (type == "player_released") {
			auto reason = obj["reason"];
			if (reason.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing reason for player_released event.\n");
			}
			event = new PlayerReleasedEvent(reason.AsString());
		}
		else if (type == "weather_change") {
			auto weather = obj["weather"];
			if (weather.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing weather for weather_change event.\n");
			}
			event = new WeatherChangeEvent(weather.AsString());
		}
		else if (type == "policy_change") {
			auto policy = obj["policy"];
			auto status = obj["status"];
			if (policy.IsNull() || status.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing policy or status for policy_change event.\n");
			}
			event = new PolicyChangeEvent(policy.AsString(), status.AsBool());
		}
		else if (type == "npc_arrive") {
			auto name = obj["name"];
			auto address = obj["address"];
			if (name.IsNull() || address.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing name or address for npc_arrive event.\n");
			}
			event = new NpcArriveEvent(name.AsString(), address.AsString());
		}
		else if (type == "transaction_result") {
			auto result = obj["result"];
			if (result.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing result for transaction_result event.\n");
			}
			auto name = obj["name"];
			event = new TransactionResultEvent(result.AsBool(), name.IsNull() ? "" : name.AsString());
		}
		else if (type == "object_result") {
			auto object = obj["object"];
			if (object.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing object for object_result event.\n");
			}
			auto res = obj["result"];
			auto action = obj["action"];
			auto num = obj["num"];
			event = new ObjectResultEvent(
				action.IsNull() ? "" : action.AsString(),
				object.AsString(),
				res.IsNull() ? false : res.AsBool(),
				num.IsNull() ? -1 : num.AsInt()
			);
		}
		else if (type == "use_asset") {
			auto asset = obj["asset"];
			if (asset.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing asset for use_asset event.\n");
			}
			event = new UseAssetEvent(asset.AsString());
		}

		if (!event) {
			THROW_EXCEPTION(RuntimeException, "Invalid event type: " + type + ".\n");
		}

		event->SetCondition(BuildCondition(obj["condition"]));
		events.push_back(event);
	}

	return events;
}

vector<Dialog*> Script::BuildDialogs(JsonValue root) {
	vector<Dialog*> dialogs;

	for (auto obj : root) {
		Dialog* dialog = new Dialog();

		dialog->SetCondition(BuildCondition(obj["condition"]));

		for (auto section : obj["list"]) {
			if (section.IsObject()) {
				dialog->AddDialog(section["speaker"].AsString(), section["content"].AsString(),
					section["label"].AsString(), section["voice"].AsString());
			}
			else if (section.IsArray()) {
				vector<Option> options;
				for (auto item : section) {
					options.emplace_back(BuildCondition(item["condition"]), item["option"].AsString(),
						BuildDialogs(item["dialogs"]), BuildChanges(item["changes"]));
				}
				dialog->AddDialog(options);
			}
		}

		dialogs.push_back(dialog);
	}

	return dialogs;
}

vector<Change*> Script::BuildChanges(JsonValue root) {
	vector<Change*> changes;

	for (auto obj : root) {
		string type = obj["type"].AsString();
		Change* change = nullptr;

		if (type == "for_range") {
			auto var = obj["var"];
			auto from = obj["from"];
			auto to = obj["to"];
			if (var.IsNull() || from.IsNull() || to.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing var, from or to for for_range change.\n");
			}
			string step = obj["step"].IsNull() ? "1" : obj["step"].AsString();
			auto doChanges = BuildChanges(obj["do"]);
			change = new ForRangeChange(var.AsString(), from.AsString(), to.AsString(), step, doChanges);
		}
		else if (type == "place_holder") {
			auto label = obj["label"];
			change = new PlaceHolderChange(label.IsNull() ? "" : label.AsString());
		}
		else if (type == "global_message") {
			auto message = obj["message"];
			if (message.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing message for global_message change.\n");
			}
			change = new GlobalMessageChange(message.AsString());
		}
		else if (type == "game_end") {
			change = new GameEndChange();
		}
		else if (type == "set_value") {
			auto variable = obj["variable"];
			auto value = obj["value"];
			if (variable.IsNull() || value.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing variable or value for set_value change.\n");
			}
			change = new SetValueChange(variable.AsString(), value.AsString());
		}
		else if (type == "global_setting") {
			auto setting = obj["setting"];
			auto value = obj["value"];
			if (setting.IsNull() || value.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing setting or value for global_setting change.\n");
			}
			change = new GlobalSettingChange(setting.AsString(), value.AsString());
		}
		else if (type == "remove_value") {
			auto variable = obj["variable"];
			if (variable.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing variable for remove_value change.\n");
			}
			change = new RemoveValueChange(variable.AsString());
		}
		else if (type == "deactivate_milestone") {
			auto milestone = obj["milestone"];
			if (milestone.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing milestone for deactivate_milestone change.\n");
			}
			change = new DeactivateMilestoneChange(milestone.AsString());
		}
		else if (type == "add_option") {
			auto name = obj["name"];
			auto option = obj["option"];
			if (name.IsNull() || option.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing name or option for add_option change.\n");
			}
			change = new AddOptionChange(name.AsString(), option.AsString());
		}
		else if (type == "remove_option") {
			auto name = obj["name"];
			auto option = obj["option"];
			if (name.IsNull() || option.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing name or option for remove_option change.\n");
			}
			change = new RemoveOptionChange(name.AsString(), option.AsString());
		}
		else if (type == "add_global") {
			auto option = obj["option"];
			if (option.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing option for add_global change.\n");
			}
			change = new AddGlobalChange(option.AsString());
		}
		else if (type == "remove_global") {
			auto option = obj["option"];
			if (option.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing option for remove_global change.\n");
			}
			change = new RemoveGlobalChange(option.AsString());
		}
		else if (type == "spawn_npc") {
			auto name = obj["name"];
			string avatar = obj["avatar"].IsNull() ? "" : obj["avatar"].AsString();
			if (name.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing name for spawn_npc change.\n");
			}
			auto gender = obj["gender"];
			if (gender.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing gender for spawn_npc change.\n");
			}
			auto birthday = obj["birthday"];
			if (birthday.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing birthday for spawn_npc change.\n");
			}
			float height = obj["height"].IsNull() ? 0.0f : obj["height"].AsFloat();
			float weight = obj["weight"].IsNull() ? 0.0f : obj["weight"].AsFloat();
			string nick = obj["nick"].IsNull() ? "" : obj["nick"].AsString();
			int deposit = obj["deposit"].IsNull() ? 0 : obj["deposit"].AsInt();
			int phone = obj["phone"].IsNull() ? 0 : obj["phone"].AsInt();
			string home = obj["home"].IsNull() ? "" : obj["home"].AsString();
			vector<string> jobs;
			for (auto job : obj["jobs"]) {
				if (!job.IsString()) {
					THROW_EXCEPTION(RuntimeException, "Invalid job for spawn_npc change.\n");
				}
				jobs.push_back(job.AsString());
			}
			string scheduler = obj["scheduler"].IsNull() ? "" : obj["scheduler"].AsString();
			change = new SpawnNpcChange(avatar, name.AsString(), gender.AsString(), birthday.AsString(),
				height, weight, nick, deposit, phone, home, jobs, scheduler);
		}
		else if (type == "remove_npc") {
			auto name = obj["name"];
			if (name.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing name for remove_npc change.\n");
			}
			change = new RemoveNpcChange(name.AsString());
		}
		else if (type == "teleport_citizen") {
			auto name = obj["name"];
			auto destination = obj["destination"];
			if (name.IsNull() || destination.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing name or destination for teleport_citizen change.\n");
			}
			change = new TeleportCitizenChange(name.AsString(), destination.AsString());
		}
		else if (type == "npc_navigate") {
			auto name = obj["name"];
			auto destination = obj["destination"];
			if (name.IsNull() || destination.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing name or destination for npc_navigate change.\n");
			}
			change = new NPCNavigateChange(name.AsString(), destination.AsString());
		}
		else if (type == "teleport_player") {
			auto destination = obj["destination"];
			if (destination.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing destination for teleport_player change.\n");
			}
			change = new TeleportPlayerChange(destination.AsString());
		}
		else if (type == "open_shop") {
			auto saler = obj["saler"];
			if (saler.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing saler for open_shop change.\n");
			}
			change = new OpenShopChange(saler.AsString());
		}
		else if (type == "start_puzzle") {
			auto puzzle = obj["puzzle"];
			if (puzzle.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing puzzle for start_puzzle change.\n");
			}
			change = new StartPuzzleChange(puzzle.AsString());
		}
		else if (type == "enter_vehicle") {
			auto vehicle = obj["vehicle"];
			if (vehicle.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing vehicle for enter_vehicle change.\n");
			}
			change = new EnterVehicleChange(vehicle.AsString());
		}
		else if (type == "leave_vehicle") {
			auto vehicle = obj["vehicle"];
			if (vehicle.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing vehicle for leave_vehicle change.\n");
			}
			change = new LeaveVehicleChange(vehicle.AsString());
		}
		else if (type == "create_timer") {
			auto name = obj["name"];
			auto time = obj["time"];
			auto category = obj["category"];
			if (name.IsNull() || time.IsNull() || category.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing name, time or category for create_timer change.\n");
			}
			string label = obj["label"].IsNull() ? "" : obj["label"].AsString();
			change = new CreateTimerChange(name.AsString(), time.AsString(), category.AsString(), label);
		}
		else if (type == "remove_timer") {
			auto name = obj["name"];
			if (name.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing name for remove_timer change.\n");
			}
			change = new RemoveTimerChange(name.AsString());
		}
		else if (type == "launch_elevator") {
			auto building = obj["building"];
			auto elevator = obj["elevator"];
			auto command = obj["command"];
			if (building.IsNull() || elevator.IsNull() || command.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing building, elevator or command for launch_elevator change.\n");
			}
			change = new LaunchElevatorChange(building.AsString(), elevator.AsString(), command.AsString());
		}
		else if (type == "bank_transaction") {
			auto amount = obj["amount"];
			if (amount.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing amount for bank_transaction change.\n");
			}
			auto nameVal = obj["name"];
			string name = nameVal.IsNull() ? "" : nameVal.AsString();
			change = new BankTransactionChange(name, amount.AsInt());
		}
		else if (type == "give_estate") {
			auto estate = obj["estate"];
			if (estate.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing estate for give_estate change.\n");
			}
			auto nameVal = obj["name"];
			auto forceVal = obj["force"];
			change = new GiveEstateChange(estate.AsString(),
				nameVal.IsNull() ? "" : nameVal.AsString(),
				forceVal.IsNull() ? true : forceVal.AsBool());
		}
		else if (type == "remove_estate") {
			auto estate = obj["estate"];
			if (estate.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing estate for remove_estate change.\n");
			}
			auto nameVal = obj["name"];
			change = new RemoveEstateChange(estate.AsString(),
				nameVal.IsNull() ? "" : nameVal.AsString());
		}
		else if (type == "give_vehicle") {
			auto vehicle = obj["vehicle"];
			if (vehicle.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing vehicle for give_vehicle change.\n");
			}
			auto nameVal = obj["name"];
			auto forceVal = obj["force"];
			change = new GiveVehicleChange(vehicle.AsString(),
				nameVal.IsNull() ? "" : nameVal.AsString(),
				forceVal.IsNull() ? true : forceVal.AsBool());
		}
		else if (type == "remove_vehicle") {
			auto vehicle = obj["vehicle"];
			if (vehicle.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing vehicle for remove_vehicle change.\n");
			}
			auto nameVal = obj["name"];
			change = new RemoveVehicleChange(vehicle.AsString(),
				nameVal.IsNull() ? "" : nameVal.AsString());
		}
		else if (type == "give_object") {
			auto object = obj["object"];
			if (object.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing object for give_object change.\n");
			}
			auto num = obj["num"];
			change = new GiveObjectChange(object.AsString(), num.IsNull() ? 1 : num.AsInt());
		}
		else if (type == "remove_object") {
			auto object = obj["object"];
			if (object.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing object for remove_object change.\n");
			}
			auto num = obj["num"];
			auto forceVal = obj["force"];
			change = new RemoveObjectChange(object.AsString(), num.IsNull() ? 1 : num.AsInt(),
				forceVal.IsNull() ? false : forceVal.AsBool());
		}
		else if (type == "enter_battle") {
			auto enemy = obj["enemy"];
			if (enemy.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing enemy for enter_battle change.\n");
			}
			change = new EnterBattleChange(enemy.AsString());
		}
		else if (type == "player_injured") {
			auto wound = obj["wound"];
			if (wound.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing wound for player_injured change.\n");
			}
			change = new PlayerInjuredChange(wound.AsString());
		}
		else if (type == "player_cured") {
			auto wound = obj["wound"];
			if (wound.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing wound for player_cured change.\n");
			}
			change = new PlayerCuredChange(wound.AsString());
		}
		else if (type == "player_ill") {
			auto illness = obj["illness"];
			if (illness.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing illness for player_ill change.\n");
			}
			change = new PlayerIllChange(illness.AsString());
		}
		else if (type == "player_recover") {
			auto illness = obj["illness"];
			if (illness.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing illness for player_recover change.\n");
			}
			change = new PlayerRecoverChange(illness.AsString());
		}
		else if (type == "player_sleep") {
			auto hour = obj["hour"];
			if (hour.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing hour for player_sleep change.\n");
			}
			change = new PlayerSleepChange(hour.AsInt());
		}
		else if (type == "change_time") {
			auto delta = obj["delta"];
			if (delta.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing delta for change_time change.\n");
			}
			change = new ChangeTimeChange(delta.AsString());
		}
		else if (type == "change_cultivation") {
			auto method = obj["method"];
			auto level = obj["level"];
			if (method.IsNull() || level.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing method or level for change_cultivation change.\n");
			}
			change = new ChangeCultivationChange(method.AsString(), level.AsInt());
		}
		else if (type == "change_wanted") {
			auto reason = obj["reason"];
			auto level = obj["level"];
			if (reason.IsNull() || level.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing reason or level for change_wanted change.\n");
			}
			change = new ChangeWantedChange(reason.AsString(), level.AsInt());
		}
		else if (type == "change_weather") {
			auto weather = obj["weather"];
			if (weather.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing weather for change_weather change.\n");
			}
			change = new ChangeWeatherChange(weather.AsString());
		}
		else if (type == "change_policy") {
			auto policy = obj["policy"];
			if (policy.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing policy for change_policy change.\n");
			}
			change = new ChangePolicyChange(policy.AsString());
		}
		else if (type == "play_video") {
			auto path = obj["path"];
			if (path.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing path for play_video change.\n");
			}
			change = new PlayVideoChange(path.AsString());
		}
		else if (type == "play_bgm") {
			auto bgm = obj["bgm"];
			if (bgm.IsNull()) {
				THROW_EXCEPTION(RuntimeException, "Missing bgm for play_bgm change.\n");
			}
			auto loopVal = obj["loop"];
			change = new PlayBgmChange(bgm.AsString(), loopVal.IsNull() ? true : loopVal.AsBool());
		}
		else if (type == "stop_bgm") {
			change = new StopBgmChange();
		}

		if (!change) {
			THROW_EXCEPTION(InvalidArgumentException, "Invalid change type: " + type + ".\n");
		}

		change->SetCondition(BuildCondition(obj["condition"]));
		changes.push_back(change);
	}

	return changes;
}

Condition Script::BuildCondition(JsonValue root) {
	Condition condition;

	condition.ParseCondition(root.AsString());

	return condition;
}

vector<string> Script::BuildSubsequences(JsonValue root) {
	vector<string> subsequences;
	for (auto obj : root) {
		subsequences.push_back(obj.AsString());
	}
	return subsequences;
}

int EmptyScript::count = 0;

EmptyScript::EmptyScript() : id(count++) {

}

EmptyScript::~EmptyScript() {

}

const char* EmptyScript::GetId() {
	return "empty";
}

const char* EmptyScript::GetType() const {
	return "empty";
}

const char* EmptyScript::GetName() {
	name = "空脚本" + to_string(id);
	return name.data();
}

bool EmptyScript::MainStory() {
	return true;
}

void EmptyScript::SetScript() {

}
