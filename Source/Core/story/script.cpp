#include "script.h"

#include <fstream>
#include <filesystem>


using namespace std;

unordered_map<string, pair<vector<string>, unordered_map<string, Milestone*>>> Script::caches = {};

Script::Script(ScriptFactory* factory, const string& script) :
	mod(factory->CreateScript(script)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	milestones(),
	actives() {

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

void Script::SetValue(const string& name, ValueType value) {
    variables[name] = value;
}

void Script::RemoveValue(const string& name) {
    variables.erase(name);
}

vector<Action> Script::PreTrigger(Event* event) {
    vector<Action> actions;

    mod->PreTrigger(event);

    actions.reserve(mod->actionQueue.size());
    while (!mod->actionQueue.empty()) {
        actions.push_back(mod->actionQueue.front());
        mod->actionQueue.pop();
    }

    return actions;
}

vector<Action> Script::PostTrigger(Event* event) {
    vector<Action> actions;

    mod->PostTrigger(event);

    actions.reserve(mod->actionQueue.size());
    while (!mod->actionQueue.empty()) {
        actions.push_back(mod->actionQueue.front());
        mod->actionQueue.pop();
    }

    return actions;
}

void Script::ReadScript(const string& path) {
    if (caches.find(path) != caches.end()) {
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
            THROW_EXCEPTION(JsonFormatException, "Json syntax error: Root element must be array or object.\n");
        }
        caches.insert(item);
    }
    else {
        THROW_EXCEPTION(JsonFormatException, "Json syntax error: " + reader.GetErrorMessages() + ".\n");
    }
    fin.close();
}

vector<string> Script::ReadNames(const string& path) {
    ReadScript(path);
    if (caches.find(path) == caches.end()) {
        THROW_EXCEPTION(RuntimeException, "Read script failed: " + path + "\n");
    }

    return caches[path].first;
}

void Script::ReadMilestones(const string& path) {
    ReadScript(path);
    if (caches.find(path) == caches.end()) {
        THROW_EXCEPTION(RuntimeException, "Read script failed: " + path + "\n");
    }

    actives.clear();
    for (auto milestone : milestones) {
        milestone.second.subsequents.clear();
        milestone.second.premise = 0;
    }

    for (auto milestone : caches[path].second) {
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

vector<Action> Script::MatchEvent(Event* event,
    vector<function<pair<bool, ValueType>(const string&)>> getValues) {
    vector<Action> actions;

    vector<MilestoneNode*> tmps;
    for (auto it = actives.begin(); it != actives.end(); ) {
        bool match = false;
        for (auto trigger : (*it)->content->GetTriggers()) {
            if (!trigger->GetCondition().EvaluateBool(getValues)) {
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
            actions.insert(actions.end(), dialogs.begin(), dialogs.end());
            auto changes = (*it)->content->GetChanges();
            actions.insert(actions.end(), changes.begin(), changes.end());

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

vector<Event*> Script::BuildEvent(JsonValue root) {
    vector<Event*> events;

    for (auto obj : root) {
        Event* event = nullptr;
        string type = obj["type"].AsString();

        if (type == "game_start") {
            event = new GameStartEvent();
        }
        else if (type == "script_message") {
            auto message = obj["message"];
            if (message.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing message for script_message event.\n");
            }
            event = new ScriptMessageEvent(message.AsString());
        }
        else if (type == "option_dialog") {
            auto id = obj["id"];
            auto name = obj["name"];
            auto option = obj["option"];
            if (option.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing option for option_dialog event.\n");
            }
            if (id.AsInt() == -1) {
                event = new OptionDialogEvent(obj["name"].AsString(), obj["option"].AsString());
            }
            else {
                event = new OptionDialogEvent(id.AsInt(), obj["option"].AsString());
            }
        }
        else if (type == "npc_meet") {
            auto npc = obj["npc"];
            if (npc.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing npc for npc_meet event.\n");
            }
            event = new NPCMeetEvent(obj["npc"].AsString());
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
        else if (type == "deposit_change") {
            auto result = obj["result"];
            auto delta = obj["delta"];
            if (result.IsNull() || delta.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing result or delta for deposit_change event.\n");
            }
            event = new DepositChangeEvent(result.AsInt(), delta.AsInt());
        }
        else if (type == "cash_change") {
            auto result = obj["result"];
            auto delta = obj["delta"];
            if (result.IsNull() || delta.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing result or delta for cash_change event.\n");
            }
            event = new CashChangeEvent(result.AsInt(), delta.AsInt());
        }
        else if (type == "get_item") {
            auto item = obj["item"];
            auto num = obj["num"];
            if (item.IsNull() || num.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing item or num for get_item event.\n");
            }
            event = new GetItemEvent(item.AsString(), num.AsInt());
        }
        else if (type == "lose_item") {
            auto item = obj["item"];
            auto num = obj["num"];
            if (item.IsNull() || num.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing item or num for lose_item event.\n");
            }
            event = new LoseItemEvent(item.AsString(), num.AsInt());
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
            auto timer = obj["timer"];
            if (timer.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing timer for time_up event.\n");
            }
            event = new TimeUpEvent(timer.AsString());
        }
        else if (type == "count_up") {
            auto counter = obj["counter"];
            if (counter.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing counter for count_up event.\n");
            }
            event = new CountUpEvent(counter.AsString());
        }
        else if (type == "use_skill") {
            auto skill = obj["skill"];
            if (skill.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing skill for use_skill event.\n");
            }
            event = new UseSkillEvent(skill.AsString());
        }
        else if (type == "skill_change") {
            auto skill = obj["skill"];
            auto level = obj["level"];
            if (skill.IsNull() || level.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing skill or level for skill_change event.\n");
            }
            event = new SkillChangeEvent(skill.AsString(), level.AsInt());
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
        else if (type == "escape_success") {
            auto enemy = obj["enemy"];
            if (enemy.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing enemy for escape_success event.\n");
            }
            event = new EscapeSuccessEvent(enemy.AsString());
        }
        else if (type == "escape_fail") {
            auto enemy = obj["enemy"];
            if (enemy.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing enemy for escape_fail event.\n");
            }
            event = new EscapeFailEvent(enemy.AsString());
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
                dialog->AddDialog(section["speaker"].AsString(), section["content"].AsString());
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

        if (type == "global_message") {
            auto message = obj["message"];
            if (message.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing message for global_message change.\n");
            }
            change = new GlobalMessageChange(message.AsString());
        }
        else if (type == "set_value") {
            auto variable = obj["variable"];
            auto value = obj["value"];
            if (variable.IsNull() || value.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing variable or value for set_value change.\n");
            }
            change = new SetValueChange(variable.AsString(), value.AsString());
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
        else if (type == "spawn_npc") {
            auto name = obj["name"];
            if (name.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing name for spawn_npc change.\n");
            }
            string avatar = obj["avatar"].IsNull() ? "" : obj["avatar"].AsString();
            change = new SpawnNpcChange(name.AsString(), avatar);
        }
        else if (type == "remove_npc") {
            auto name = obj["name"];
            if (name.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing name for remove_npc change.\n");
            }
            change = new RemoveNpcChange(name.AsString());
        }
        else if (type == "npc_navigate") {
            auto name = obj["name"];
            auto destination = obj["destination"];
            if (name.IsNull() || destination.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing name or destination for npc_navigate change.\n");
            }
            change = new NPCNavigateChange(name.AsString(), destination.AsString());
        }
        else if (type == "teleport_citizen") {
            auto name = obj["name"];
            auto destination = obj["destination"];
            if (name.IsNull() || destination.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing name or destination for teleport_citizen change.\n");
            }
            change = new TeleportCitizenChange(name.AsString(), destination.AsString());
        }
        else if (type == "teleport_player") {
            auto destination = obj["destination"];
            if (destination.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing destination for teleport_player change.\n");
            }
            change = new TeleportPlayerChange(destination.AsString());
        }
        else if (type == "bank_transaction") {
            auto amount = obj["amount"];
            if (amount.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing amount for bank_transaction change.\n");
            }
            change = new BankTransactionChange(amount.AsInt());
        }
        else if (type == "cash_transaction") {
            auto amount = obj["amount"];
            if (amount.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing amount for cash_transaction change.\n");
            }
            change = new CashTransactionChange(amount.AsInt());
        }
        else if (type == "give_item") {
            auto item = obj["item"];
            auto num = obj["num"];
            if (item.IsNull() || num.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing item or num for give_item change.\n");
            }
            change = new GiveItemChange(item.AsString(), num.AsInt());
        }
        else if (type == "remove_item") {
            auto item = obj["item"];
            auto num = obj["num"];
            if (item.IsNull() || num.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing item or num for remove_item change.\n");
            }
            change = new RemoveItemChange(item.AsString(), num.AsInt());
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
        else if (type == "create_timer") {
            auto timer = obj["timer"];
            auto duration = obj["duration"];
            if (timer.IsNull() || duration.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing timer or duration for create_timer change.\n");
            }
            int loop = obj["loop"].IsNull() ? 1 : obj["loop"].AsInt();
            Time durationTime = Time(duration.AsString());
            change = new CreateTimerChange(timer.AsString(), durationTime, loop);
        }
        else if (type == "pause_timer") {
            auto timer = obj["timer"];
            if (timer.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing timer for pause_timer change.\n");
            }
            change = new PauseTimerChange(timer.AsString());
        }
        else if (type == "resume_timer") {
            auto timer = obj["timer"];
            if (timer.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing timer for resume_timer change.\n");
            }
            change = new ResumeTimerChange(timer.AsString());
        }
        else if (type == "remove_timer") {
            auto timer = obj["timer"];
            if (timer.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing timer for remove_timer change.\n");
            }
            change = new RemoveTimerChange(timer.AsString());
        }
        else if (type == "reset_timer") {
            auto timer = obj["timer"];
            if (timer.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing timer for reset_timer change.\n");
            }
            change = new ResetTimerChange(timer.AsString());
        }
        else if (type == "create_counter") {
            auto counter = obj["counter"];
            auto count = obj["count"];
            if (counter.IsNull() || count.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing counter or count for create_counter change.\n");
            }
            change = new CreateCounterChange(counter.AsString(), count.AsInt());
        }
        else if (type == "call_counter") {
            auto counter = obj["counter"];
            auto delta = obj["delta"];
            if (counter.IsNull() || delta.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing counter or delta for call_counter change.\n");
            }
            change = new CallCounterChange(counter.AsString(), delta.AsInt());
        }
        else if (type == "remove_counter") {
            auto counter = obj["counter"];
            if (counter.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing counter for remove_counter change.\n");
            }
            change = new RemoveCounterChange(counter.AsString());
        }
        else if (type == "enter_battle") {
            auto enemy = obj["enemy"];
            if (enemy.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing enemy for enter_battle change.\n");
            }
            change = new EnterBattleChange(enemy.AsString());
        }
        else if (type == "change_time") {
            auto delta = obj["delta"];
            if (delta.IsNull()) {
                THROW_EXCEPTION(RuntimeException, "Missing delta for change_time change.\n");
            }
            Time deltaTime = Time(delta.AsString());
            change = new ChangeTimeChange(deltaTime);
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

void EmptyScript::PreTrigger(Event* event) {

}

void EmptyScript::PostTrigger(Event* event) {

}
