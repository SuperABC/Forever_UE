#include "map/map.h"
#include "populace/populace.h"
#include "populace/person.h"
#include "society/society.h"
#include "story/story.h"
#include "story/script.h"
#include "common/implement.h"
#include "story/event.h"
#include "story/dialog.h"
#include "story/change.h"
#include "industry/industry.h"
#include "traffic/traffic.h"
#include "player/player.h"

#include "parser.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <io.h>
#include <fcntl.h>

#pragma comment(lib, "Core.lib")
#pragma comment(lib, "Dependence.lib")


using namespace std;

Map* map = new Map();
Populace *populace = new Populace();
Society* society = new Society();
Story* story = new Story();
Industry* industry = new Industry();
Traffic* traffic = new Traffic();
Player* player = new Player();
PostImplement* implement = new PostImplement(::map, populace, society, story, industry, traffic, player);

// ANSI字符串和UTF-8字符串转换
string AnsiToUtf8(const string& ansiStr) {
	// 第一步：ANSI -> UTF-16
	int lenWide = MultiByteToWideChar(CP_ACP, 0, ansiStr.data(), -1, nullptr, 0);
	if (lenWide == 0) return ""; // 转换失败
	vector<wchar_t> wideBuf(lenWide);
	MultiByteToWideChar(CP_ACP, 0, ansiStr.data(), -1, wideBuf.data(), lenWide);

	// 第二步：UTF-16 -> UTF-8
	int lenUtf8 = WideCharToMultiByte(CP_UTF8, 0, wideBuf.data(), -1, nullptr, 0, nullptr, nullptr);
	if (lenUtf8 == 0) return "";
	vector<char> utf8Buf(lenUtf8);
	WideCharToMultiByte(CP_UTF8, 0, wideBuf.data(), -1, utf8Buf.data(), lenUtf8, nullptr, nullptr);

	return string(utf8Buf.data());
}
string Utf8ToAnsi(const string& utf8Str) {
	// 第一步：UTF-8 -> UTF-16
	int lenWide = MultiByteToWideChar(CP_UTF8, 0, utf8Str.data(), -1, nullptr, 0);
	if (lenWide == 0) return "";
	vector<wchar_t> wideBuf(lenWide);
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.data(), -1, wideBuf.data(), lenWide);

	// 第二步：UTF-16 -> ANSI
	int lenAnsi = WideCharToMultiByte(CP_ACP, 0, wideBuf.data(), -1, nullptr, 0, nullptr, nullptr);
	if (lenAnsi == 0) return "";
	vector<char> ansiBuf(lenAnsi);
	WideCharToMultiByte(CP_ACP, 0, wideBuf.data(), -1, ansiBuf.data(), lenAnsi, nullptr, nullptr);

	return string(ansiBuf.data());
}

// 解析命令行输入事件
Event* ParseEvent(Parser& parser) {
	string type = parser.GetOption("--type");
	int id = -1;
	string message = "";
	string name = "";
	string npc = "";
	string option = "";
	int num = 0;
	int level = 0;
	bool status = false;

	if (type == "game_start") {
		return new GameStartEvent();
	}
	else if (type == "script_message") {
		if (parser.HasOption("--message")) message = parser.GetOption("--message");
		return new ScriptMessageEvent(message);
	}
	else if (type == "option_dialog") {
		if (parser.HasOption("--id")) id = stoi(parser.GetOption("--id"));
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		if (parser.HasOption("--option")) option = parser.GetOption("--option");
		if (name.size() == 0)
			return new OptionDialogEvent(id, option);
		else {
			return new OptionDialogEvent(name, option);
		}
	}
	else if (type == "npc_meet") {
		if (parser.HasOption("--npc")) npc = parser.GetOption("--npc");
		return new NPCMeetEvent(npc);
	}
	else if (type == "citizen_born") {
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		return new CitizenBornEvent(name);
	}
	else if (type == "citizen_decease") {
		string name = "";
		string reason = "";
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		if (parser.HasOption("--reason")) reason = parser.GetOption("--reason");
		return new CitizenDeceaseEvent(name, reason);
	}
	else if (type == "enter_zone") {
		string zone = "";
		if (parser.HasOption("--zone")) zone = parser.GetOption("--zone");
		return new EnterZoneEvent(zone);
	}
	else if (type == "leave_zone") {
		string zone = "";
		if (parser.HasOption("--zone")) zone = parser.GetOption("--zone");
		return new LeaveZoneEvent(zone);
	}
	else if (type == "enter_building") {
		string zone = "";
		string building = "";
		if (parser.HasOption("--zone")) zone = parser.GetOption("--zone");
		if (parser.HasOption("--building")) building = parser.GetOption("--building");
		return new EnterBuildingEvent(zone, building);
	}
	else if (type == "leave_building") {
		string zone = "";
		string building = "";
		if (parser.HasOption("--zone")) zone = parser.GetOption("--zone");
		if (parser.HasOption("--building")) building = parser.GetOption("--building");
		return new LeaveBuildingEvent(zone, building);
	}
	else if (type == "enter_room") {
		string zone = "";
		string building = "";
		string room = "";
		if (parser.HasOption("--zone")) zone = parser.GetOption("--zone");
		if (parser.HasOption("--building")) building = parser.GetOption("--building");
		if (parser.HasOption("--room")) room = parser.GetOption("--room");
		return new EnterRoomEvent(zone, building, room);
	}
	else if (type == "leave_room") {
		string zone = "";
		string building = "";
		string room = "";
		if (parser.HasOption("--zone")) zone = parser.GetOption("--zone");
		if (parser.HasOption("--building")) building = parser.GetOption("--building");
		if (parser.HasOption("--room")) room = parser.GetOption("--room");
		return new LeaveRoomEvent(zone, building, room);
	}
	else if (type == "speaking_finish") {
		string label = "";
		if (parser.HasOption("--label")) label = parser.GetOption("--label");
		return new SpeakingFinishEvent(label);
	}
	else if (type == "puzzle_result") {
		int result = 0;
		if (parser.HasOption("--result")) result = stoi(parser.GetOption("--result"));
		return new PuzzleResultEvent(result);
	}
	else if (type == "transaction_result") {
		bool result = false;
		string name = "";
		if (parser.HasOption("--result")) result = parser.GetOption("--result") == "true";
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		return new TransactionResultEvent(result, name);
	}
	else if (type == "npc_arrive") {
		string name = "", address = "";
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		if (parser.HasOption("--address")) address = parser.GetOption("--address");
		return new NpcArriveEvent(name, address);
	}
	else if (type == "object_result") {
		string action = "", object = "";
		bool res = false;
		int num = -1;
		if (parser.HasOption("--action")) action = parser.GetOption("--action");
		if (parser.HasOption("--object")) object = parser.GetOption("--object");
		if (parser.HasOption("--result")) res = parser.GetOption("--result") == "true";
		if (parser.HasOption("--num")) num = stoi(parser.GetOption("--num"));
		return new ObjectResultEvent(action, object, res, num);
	}
	else if (type == "player_injured") {
		string wound = "";
		if (parser.HasOption("--wound")) wound = parser.GetOption("--wound");
		return new PlayerInjuredEvent(wound);
	}
	else if (type == "player_cured") {
		string wound = "";
		if (parser.HasOption("--wound")) wound = parser.GetOption("--wound");
		return new PlayerCuredEvent(wound);
	}
	else if (type == "player_ill") {
		string illness = "";
		if (parser.HasOption("--illness")) illness = parser.GetOption("--illness");
		return new PlayerIllEvent(illness);
	}
	else if (type == "player_recover") {
		string illness = "";
		if (parser.HasOption("--illness")) illness = parser.GetOption("--illness");
		return new PlayerRecoverEvent(illness);
	}
	else if (type == "player_rest") {
		int minute = 0;
		if (parser.HasOption("--minute")) minute = stoi(parser.GetOption("--minute"));
		return new PlayerRestEvent(minute);
	}
	else if (type == "player_sleep") {
		int hour = 0;
		if (parser.HasOption("--hour")) hour = stoi(parser.GetOption("--hour"));
		return new PlayerSleepEvent(hour);
	}
	else if (type == "time_up") {
		string timer = "";
		if (parser.HasOption("--timer")) timer = parser.GetOption("--timer");
		return new TimeUpEvent(timer);
	}
	else if (type == "battle_win") {
		string enemy = "";
		if (parser.HasOption("--enemy")) enemy = parser.GetOption("--enemy");
		return new BattleWinEvent(enemy);
	}
	else if (type == "battle_lose") {
		string enemy = "";
		if (parser.HasOption("--enemy")) enemy = parser.GetOption("--enemy");
		return new BattleLoseEvent(enemy);
	}
	else if (type == "cultivation_change") {
		string method = "";
		int level = 0;
		if (parser.HasOption("--method")) method = parser.GetOption("--method");
		if (parser.HasOption("--level")) level = stoi(parser.GetOption("--level"));
		return new CultivationChangeEvent(method, level);
	}
	else if (type == "wanted_change") {
		string reason = "";
		int level = 0;
		if (parser.HasOption("--reason")) reason = parser.GetOption("--reason");
		if (parser.HasOption("--level")) level = stoi(parser.GetOption("--level"));
		return new WantedChangeEvent(reason, level);
	}
	else if (type == "player_arrested") {
		string reason = "";
		if (parser.HasOption("--reason")) reason = parser.GetOption("--reason");
		return new PlayerArrestedEvent(reason);
	}
	else if (type == "player_released") {
		string reason = "";
		if (parser.HasOption("--reason")) reason = parser.GetOption("--reason");
		return new PlayerReleasedEvent(reason);
	}
	else if (type == "weather_change") {
		string weather = "";
		if (parser.HasOption("--weather")) weather = parser.GetOption("--weather");
		return new WeatherChangeEvent(weather);
	}
	else if (type == "policy_change") {
		string policy = "";
		bool status = false;
		if (parser.HasOption("--policy")) policy = parser.GetOption("--policy");
		if (parser.HasOption("--status")) {
			string status_str = parser.GetOption("--status");
			status = (status_str == "true" || status_str == "1");
		}
		return new PolicyChangeEvent(policy, status);
	}

	return nullptr;
}

// 解析命令行输入变化
Change* ParseChange(Parser& parser) {
	string type = parser.GetOption("--type");

	if (type == "global_message") {
		string message = "";
		if (parser.HasOption("--message")) message = parser.GetOption("--message");
		return new GlobalMessageChange(message);
	}
	else if (type == "game_end") {
		return new GameEndChange();
	}
	else if (type == "set_value") {
		string variable = "", value = "";
		if (parser.HasOption("--variable")) variable = parser.GetOption("--variable");
		if (parser.HasOption("--value")) value = parser.GetOption("--value");
		return new SetValueChange(variable, value);
	}
	else if (type == "remove_value") {
		string variable = "";
		if (parser.HasOption("--variable")) variable = parser.GetOption("--variable");
		return new RemoveValueChange(variable);
	}
	else if (type == "deactivate_milestone") {
		string milestone = "";
		if (parser.HasOption("--milestone")) milestone = parser.GetOption("--milestone");
		return new DeactivateMilestoneChange(milestone);
	}
	else if (type == "add_option") {
		string name = "", option = "";
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		if (parser.HasOption("--option")) option = parser.GetOption("--option");
		return new AddOptionChange(name, option);
	}
	else if (type == "remove_option") {
		string name = "", option = "";
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		if (parser.HasOption("--option")) option = parser.GetOption("--option");
		return new RemoveOptionChange(name, option);
	}
	else if (type == "spawn_npc") {
		string avatar = "", name = "", gender = "", birthday = "", nick = "", home = "", scheduler = "";
		float height = 0, weight = 0;
		int deposit = 0, phone = 0;
		vector<string> jobs;
		if (parser.HasOption("--avatar")) avatar = parser.GetOption("--avatar");
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		if (parser.HasOption("--gender")) gender = parser.GetOption("--gender");
		if (parser.HasOption("--birthday")) birthday = parser.GetOption("--birthday");
		if (parser.HasOption("--height")) height = stof(parser.GetOption("--height"));
		if (parser.HasOption("--weight")) weight = stof(parser.GetOption("--weight"));
		if (parser.HasOption("--nick")) nick = parser.GetOption("--nick");
		if (parser.HasOption("--deposit")) deposit = stoi(parser.GetOption("--deposit"));
		if (parser.HasOption("--phone")) phone = stoi(parser.GetOption("--phone"));
		if (parser.HasOption("--home")) home = parser.GetOption("--home");
		if (parser.HasOption("--jobs")) {
			stringstream ss(parser.GetOption("--jobs"));
			string job;
			while (getline(ss, job, ',')) jobs.push_back(job);
		}
		if (parser.HasOption("--scheduler")) scheduler = parser.GetOption("--scheduler");
		return new SpawnNpcChange(avatar, name, gender, birthday, height, weight, nick, deposit, phone, home, jobs, scheduler);
	}
	else if (type == "remove_npc") {
		string name = "";
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		return new RemoveNpcChange(name);
	}
	else if (type == "teleport_citizen") {
		string name = "", destination = "";
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		if (parser.HasOption("--destination")) destination = parser.GetOption("--destination");
		return new TeleportCitizenChange(name, destination);
	}
	else if (type == "npc_navigate") {
		string name = "", destination = "";
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		if (parser.HasOption("--destination")) destination = parser.GetOption("--destination");
		return new NPCNavigateChange(name, destination);
	}
	else if (type == "teleport_player") {
		string destination = "";
		if (parser.HasOption("--destination")) destination = parser.GetOption("--destination");
		return new TeleportPlayerChange(destination);
	}
	else if (type == "open_shop") {
		string saler = "";
		if (parser.HasOption("--saler")) saler = parser.GetOption("--saler");
		return new OpenShopChange(saler);
	}
	else if (type == "start_puzzle") {
		string puzzle = "";
		if (parser.HasOption("--puzzle")) puzzle = parser.GetOption("--puzzle");
		return new StartPuzzleChange(puzzle);
	}
	else if (type == "enter_vehicle") {
		string vehicle = "";
		if (parser.HasOption("--vehicle")) vehicle = parser.GetOption("--vehicle");
		return new EnterVehicleChange(vehicle);
	}
	else if (type == "leave_vehicle") {
		string vehicle = "";
		if (parser.HasOption("--vehicle")) vehicle = parser.GetOption("--vehicle");
		return new LeaveVehicleChange(vehicle);
	}
	else if (type == "create_timer") {
		string name = "", time = "", category = "", label = "";
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		if (parser.HasOption("--time")) time = parser.GetOption("--time");
		if (parser.HasOption("--category")) category = parser.GetOption("--category");
		if (parser.HasOption("--label")) label = parser.GetOption("--label");
		return new CreateTimerChange(name, time, category, label);
	}
	else if (type == "enter_battle") {
		string enemy = "";
		if (parser.HasOption("--enemy")) enemy = parser.GetOption("--enemy");
		return new EnterBattleChange(enemy);
	}
	else if (type == "launch_elevator") {
		string building = "", elevator = "", command = "";
		if (parser.HasOption("--building")) building = parser.GetOption("--building");
		if (parser.HasOption("--elevator")) elevator = parser.GetOption("--elevator");
		if (parser.HasOption("--command")) command = parser.GetOption("--command");
		return new LaunchElevatorChange(building, elevator, command);
	}
	else if (type == "play_video") {
		string path = "";
		if (parser.HasOption("--path")) path = parser.GetOption("--path");
		return new PlayVideoChange(path);
	}
	else if (type == "bank_transaction") {
		string name = "";
		int amount = 0;
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		if (parser.HasOption("--amount")) amount = stoi(parser.GetOption("--amount"));
		return new BankTransactionChange(name, amount);
	}
	else if (type == "give_estate") {
		string estate = "", name = "";
		bool force = true;
		if (parser.HasOption("--estate")) estate = parser.GetOption("--estate");
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		if (parser.HasOption("--force")) force = parser.GetOption("--force") == "true";
		return new GiveEstateChange(estate, name, force);
	}
	else if (type == "remove_estate") {
		string estate = "", name = "";
		if (parser.HasOption("--estate")) estate = parser.GetOption("--estate");
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		return new RemoveEstateChange(estate, name);
	}
	else if (type == "give_vehicle") {
		string vehicle = "", name = "";
		bool force = true;
		if (parser.HasOption("--vehicle")) vehicle = parser.GetOption("--vehicle");
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		if (parser.HasOption("--force")) force = parser.GetOption("--force") == "true";
		return new GiveVehicleChange(vehicle, name, force);
	}
	else if (type == "remove_vehicle") {
		string vehicle = "", name = "";
		if (parser.HasOption("--vehicle")) vehicle = parser.GetOption("--vehicle");
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		return new RemoveVehicleChange(vehicle, name);
	}
	else if (type == "give_object") {
		string object = "";
		int num = 1;
		if (parser.HasOption("--object")) object = parser.GetOption("--object");
		if (parser.HasOption("--num")) num = stoi(parser.GetOption("--num"));
		return new GiveObjectChange(object, num);
	}
	else if (type == "remove_object") {
		string object = "";
		int num = 1;
		bool force = false;
		if (parser.HasOption("--object")) object = parser.GetOption("--object");
		if (parser.HasOption("--num")) num = stoi(parser.GetOption("--num"));
		if (parser.HasOption("--force")) force = parser.GetOption("--force") == "true";
		return new RemoveObjectChange(object, num, force);
	}
	else if (type == "player_injured") {
		string wound = "";
		if (parser.HasOption("--wound")) wound = parser.GetOption("--wound");
		return new PlayerInjuredChange(wound);
	}
	else if (type == "player_cured") {
		string wound = "";
		if (parser.HasOption("--wound")) wound = parser.GetOption("--wound");
		return new PlayerCuredChange(wound);
	}
	else if (type == "player_ill") {
		string illness = "";
		if (parser.HasOption("--illness")) illness = parser.GetOption("--illness");
		return new PlayerIllChange(illness);
	}
	else if (type == "player_recover") {
		string illness = "";
		if (parser.HasOption("--illness")) illness = parser.GetOption("--illness");
		return new PlayerRecoverChange(illness);
	}
	else if (type == "player_sleep") {
		int hour = 0;
		if (parser.HasOption("--hour")) hour = stoi(parser.GetOption("--hour"));
		return new PlayerSleepChange(hour);
	}
	else if (type == "change_cultivation") {
		string method = "";
		int level = 0;
		if (parser.HasOption("--method")) method = parser.GetOption("--method");
		if (parser.HasOption("--level")) level = stoi(parser.GetOption("--level"));
		return new ChangeCultivationChange(method, level);
	}
	else if (type == "change_wanted") {
		string reason = "";
		int level = 0;
		if (parser.HasOption("--reason")) reason = parser.GetOption("--reason");
		if (parser.HasOption("--level")) level = stoi(parser.GetOption("--level"));
		return new ChangeWantedChange(reason, level);
	}
	else if (type == "change_weather") {
		string weather = "";
		if (parser.HasOption("--weather")) weather = parser.GetOption("--weather");
		return new ChangeWeatherChange(weather);
	}
	else if (type == "change_policy") {
		string policy = "";
		if (parser.HasOption("--policy")) policy = parser.GetOption("--policy");
		return new ChangePolicyChange(policy);
	}
	else if (type == "change_time") {
		string delta = "";
		if (parser.HasOption("--delta")) delta = parser.GetOption("--delta");
		return new ChangeTimeChange(Time(delta));
	}
	else if (type == "for_range") {
		string var = "", from = "", to = "", step = "1";
		if (parser.HasOption("--var")) var = parser.GetOption("--var");
		if (parser.HasOption("--from")) from = parser.GetOption("--from");
		if (parser.HasOption("--to")) to = parser.GetOption("--to");
		if (parser.HasOption("--step")) step = parser.GetOption("--step");
		return new ForRangeChange(var, from, to, step, {});
	}

	return nullptr;
}

// 递归输出对话与选项
bool PrintDialog(Dialog* dialog, vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	auto sections = dialog->GetDialogs();
	if (sections.size() == 0)return false;

	for (auto section : sections) {
		if (section.IsBranch()) { // 如果当前段是选项
			auto options = section.GetOptions();
			vector<int> indices;
			int i = 0, j = 0;
			for (auto option : options) {
				if (option.GetCondition().EvaluateBool(getValues)) {
					indices.push_back(i++);
				}
			}
			if (indices.size() == 0) continue;

			cout << "Options: " << endl;
			for (auto index : indices) {
				cout << j << ": " << Utf8ToAnsi(options[indices[j++]].GetOption()) << endl;
			}
			cin >> j;
			while (j >= indices.size()) {
				cout << "Out of range." << endl;
				cin >> j;
			}
			auto selected = options[indices[j]];
			for (auto dialog : selected.GetDialogs()) {
				if (dialog->GetCondition().EvaluateBool(getValues)) {
					PrintDialog(dialog, getValues);
				}
			}
			for (auto change : selected.GetChanges()) {
				if (!change->GetCondition().EvaluateBool(getValues))continue;
				::map->ApplyChange(change, getValues);
				populace->ApplyChange(::map, player, traffic, change, getValues);
				society->ApplyChange(change, getValues);
				story->ApplyChange(change, getValues);
				industry->ApplyChange(change, getValues);
				traffic->ApplyChange(change, getValues);
				player->ApplyChange(change, getValues);
			}
		}
		else { // 如果当前段是对话
			Condition conditionContent;
			conditionContent.ParseCondition(get<1>(section.GetSpeaking()));
			if (get<0>(section.GetSpeaking()).size() == 0) {
				cout << Utf8ToAnsi(ToString(conditionContent.EvaluateValue(getValues))) << endl;
			}
			else {
				Condition conditionSpeaker;
				conditionSpeaker.ParseCondition(get<0>(section.GetSpeaking()));
				cout << Utf8ToAnsi(ToString(conditionSpeaker.EvaluateValue(getValues))) << ": " <<
					Utf8ToAnsi(ToString(conditionContent.EvaluateValue(getValues))) << endl;
			}
		}
	}
	return true;
}

int main() {
	Config::ReadConfig("../Resources/config.json");

	auto mods = Config::GetMods();
	unordered_map<string, HMODULE> modHandles;

	// 读取Map相关类及Mod
	::map->LoadConfigs();
	::map->InitTerrains(modHandles, mods);
	::map->InitRoadnets(modHandles, mods);
	::map->InitZones(modHandles, mods);
	::map->InitBuildings(modHandles, mods);
	::map->InitComponents(modHandles, mods);
	::map->InitRooms(modHandles, mods);

	// 读取Populace相关类及Mod
	populace->LoadConfigs();
	populace->InitNames(modHandles, mods);
	populace->InitSchedulers(modHandles, mods);

	// 读取Society相关类及Mod
	society->LoadConfigs();
	society->InitCalendars(modHandles, mods);
	society->InitJobs(modHandles, mods);
	society->InitOrganizations(modHandles, mods);

	// 读取Story相关类及Mod
	story->LoadConfigs();
	story->InitScripts(modHandles, mods);

	// 读取Industry相关类及Mod
	industry->LoadConfigs();
	industry->InitProducts(modHandles, mods);
	industry->InitStorages(modHandles, mods);
	industry->InitManufactures(modHandles, mods);

	// 读取Traffic相关类及Mod
	traffic->LoadConfigs();
	traffic->InitRoutes(modHandles, mods);
	traffic->InitStations(modHandles, mods);
	traffic->InitVehicles(modHandles, mods);

	// 读取Player相关类及Mod
	player->LoadConfigs();
	player->InitAssets(modHandles, mods);
	player->InitPuzzles(modHandles, mods);
	player->InitApps(modHandles, mods);

	// 读取命令行
	string cmd;
	CMD_TYPE type = CMD_UNKOWN;
	do {
		// 读取一行命令
		string input;
		getline(cin, input);
		input = AnsiToUtf8(input);

		// 添加帮助选项
		Parser parser;
		parser.AddOption("--help", 'h', "Show help.");

		// 空格分解命令
		auto cmd = parser.ExtractCmd(input);
		if (cmd.size() <= 0)continue;
		auto type = parser.GetAction(cmd[0]);

		try {
			switch (type) {
			case CMD_INIT: { // 初始化世界、人口、剧本
				parser.AddOption("--block", 0, "Block num both horizontally and vertically.", true, "4");
				parser.ParseCmd(cmd);

				int size = atoi(parser.GetOption("--block").data());

				player->Init();
				::map->InitBlocks(size, size);
				traffic->InitBuildings(::map);
				int accomodation = ::map->InitContents();
				populace->Init(accomodation, player, implement);
				::map->Checkin(populace, player);
				traffic->InitTraffic(::map, populace);
				society->Init(::map, populace, player);
				story->Init(::map, populace, player);
				industry->Init(::map);

				break;
			}
			case CMD_PASS: { // 时间流逝
				break;
			}
			case CMD_EVENT: { // 文本模拟事件
				parser.AddOption("--type", 0, "Event type.", true, "nothing_happen");
				parser.AddOption("--message", 0, "Event message.", true, "");
				parser.AddOption("--id", 0, "Event target id.", true, "");
				parser.AddOption("--name", 0, "Event target name.", true, "");
				parser.AddOption("--option", 0, "Event option.", true, "");
				parser.AddOption("--label", 0, "Event label.", true, "");
				parser.AddOption("--npc", 0, "NPC name.", true, "");
				parser.AddOption("--zone", 0, "Zone name.", true, "");
				parser.AddOption("--building", 0, "Building name.", true, "");
				parser.AddOption("--room", 0, "Room name.", true, "");
				parser.AddOption("--address", 0, "Address.", true, "");
				parser.AddOption("--result", 0, "Result value.", true, "");
				parser.AddOption("--action", 0, "Action type.", true, "");
				parser.AddOption("--object", 0, "Object type.", true, "");
				parser.AddOption("--num", 0, "Num.", true, "");
				parser.AddOption("--reason", 0, "Reason.", true, "");
				parser.AddOption("--wound", 0, "Wound.", true, "");
				parser.AddOption("--illness", 0, "Illness.", true, "");
				parser.AddOption("--minute", 0, "Minutes.", true, "");
				parser.AddOption("--hour", 0, "Hours.", true, "");
				parser.AddOption("--timer", 0, "Timer name.", true, "");
				parser.AddOption("--enemy", 0, "Enemy name.", true, "");
				parser.AddOption("--method", 0, "Method.", true, "");
				parser.AddOption("--level", 0, "Level.", true, "");
				parser.AddOption("--weather", 0, "Weather.", true, "");
				parser.AddOption("--policy", 0, "Policy.", true, "");
				parser.AddOption("--status", 0, "Status.", true, "");

				parser.ParseCmd(cmd);
				auto event = ParseEvent(parser);
				if (!event) {
					THROW_EXCEPTION(CommandException, "Wrong input event format.");
				}
				
				vector<function<pair<bool, ValueType>(const string&)>> getValues = {
					[&](string name) -> pair<bool, ValueType> {
						return story->GetScript()->GetValue(name);
					}
				};
				
				auto actions = story->GetScript()->MatchEvent(event, getValues);
				actions = story->GetScript()->WrapScript(event, actions, getValues, implement);
				for (auto action : actions) {
					visit([&](auto* ptr) {
						if constexpr (is_same_v<decltype(ptr), Dialog*>) {
							auto* dialog = dynamic_cast<Dialog*>(ptr);
							if (dialog->GetCondition().EvaluateBool(getValues)) {
								PrintDialog(dialog, getValues);
							}
						}
						else if constexpr (is_same_v<decltype(ptr), Change*>) {
							auto* change = dynamic_cast<Change*>(ptr);
							if (change->GetCondition().EvaluateBool(getValues)) {
								::map->ApplyChange(change, getValues);
								populace->ApplyChange(::map, player, traffic, change, getValues);
								society->ApplyChange(change, getValues);
								story->ApplyChange(change, getValues);
								industry->ApplyChange(change, getValues);
								traffic->ApplyChange(change, getValues);
								player->ApplyChange(change, getValues);
							}
						}
					}, action);
				}
				
				delete event;
				break;
			}
			case CMD_CHANGE: { // 直接触发变化
				parser.AddOption("--type", 0, "Change type.", true, "");
				parser.AddOption("--message", 0, "Message.", true, "");
				parser.AddOption("--variable", 0, "Variable name.", true, "");
				parser.AddOption("--value", 0, "Variable value.", true, "");
				parser.AddOption("--milestone", 0, "Milestone name.", true, "");
				parser.AddOption("--name", 0, "Name.", true, "");
				parser.AddOption("--option", 0, "Option text.", true, "");
				parser.AddOption("--avatar", 0, "Avatar.", true, "");
				parser.AddOption("--gender", 0, "Gender.", true, "");
				parser.AddOption("--birthday", 0, "Birthday.", true, "");
				parser.AddOption("--height", 0, "Height.", true, "");
				parser.AddOption("--weight", 0, "Weight.", true, "");
				parser.AddOption("--nick", 0, "Nick.", true, "");
				parser.AddOption("--deposit", 0, "Deposit.", true, "");
				parser.AddOption("--phone", 0, "Phone.", true, "");
				parser.AddOption("--home", 0, "Home.", true, "");
				parser.AddOption("--jobs", 0, "Jobs (comma-separated).", true, "");
				parser.AddOption("--scheduler", 0, "Scheduler.", true, "");
				parser.AddOption("--destination", 0, "Destination.", true, "");
				parser.AddOption("--saler", 0, "Saler name.", true, "");
				parser.AddOption("--puzzle", 0, "Puzzle type.", true, "");
				parser.AddOption("--vehicle", 0, "Vehicle name.", true, "");
				parser.AddOption("--time", 0, "Time string.", true, "");
				parser.AddOption("--category", 0, "Script category.", true, "");
				parser.AddOption("--label", 0, "Script label.", true, "");
				parser.AddOption("--enemy", 0, "Enemy name.", true, "");
				parser.AddOption("--building", 0, "Building name.", true, "");
				parser.AddOption("--elevator", 0, "Elevator name.", true, "");
				parser.AddOption("--command", 0, "Command string.", true, "");
				parser.AddOption("--path", 0, "File path.", true, "");
				parser.AddOption("--amount", 0, "Amount.", true, "");
				parser.AddOption("--estate", 0, "Estate name.", true, "");
				parser.AddOption("--force", 0, "Force flag (true/false).", true, "");
				parser.AddOption("--object", 0, "Object type.", true, "");
				parser.AddOption("--num", 0, "Num.", true, "");
				parser.AddOption("--wound", 0, "Wound.", true, "");
				parser.AddOption("--illness", 0, "Illness.", true, "");
				parser.AddOption("--hour", 0, "Hours.", true, "");
				parser.AddOption("--method", 0, "Method.", true, "");
				parser.AddOption("--level", 0, "Level.", true, "");
				parser.AddOption("--reason", 0, "Reason.", true, "");
				parser.AddOption("--weather", 0, "Weather.", true, "");
				parser.AddOption("--policy", 0, "Policy.", true, "");
				parser.AddOption("--delta", 0, "Time delta string.", true, "");
				parser.AddOption("--var", 0, "Loop variable name.", true, "");
				parser.AddOption("--from", 0, "Loop from expression.", true, "");
				parser.AddOption("--to", 0, "Loop to expression.", true, "");
				parser.AddOption("--step", 0, "Loop step expression.", true, "1");

				parser.ParseCmd(cmd);
				auto change = ParseChange(parser);
				if (!change) {
					THROW_EXCEPTION(CommandException, "Wrong input change format.");
				}

				vector<function<pair<bool, ValueType>(const string&)>> getValues = {
					[&](string name) -> pair<bool, ValueType> {
						return story->GetScript()->GetValue(name);
					}
				};

				::map->ApplyChange(change, getValues);
				populace->ApplyChange(::map, player, traffic, change, getValues);
				society->ApplyChange(change, getValues);
				story->ApplyChange(change, getValues);
				industry->ApplyChange(change, getValues);
				traffic->ApplyChange(change, getValues);
				player->ApplyChange(change, getValues);
				delete change;
				break;
			}
			case CMD_LOOKUP: { // 查找信息
				parser.AddOption("--id", 0, "Lookup citizen by id.", true, "");
				parser.AddOption("--name", 0, "Lookup citizen by name.", true, "");
				parser.ParseCmd(cmd);

				if (parser.HasOption("--id")) { // 按ID查找
					int id = atoi(parser.GetOption("--id").data());
					if (id < 0 || id >= populace->GetCitizens().size()) {
						THROW_EXCEPTION(CommandException, "Wrong input citizen ID.");
					}
					auto citizen = populace->GetCitizens()[id];
				
					cout << "Citizen ID: " << citizen->GetId() << endl;
					cout << "Gender: " << (citizen->GetGender() == GENDER_FEMALE ? "female" : "male") << endl;
					cout << "Name: " << Utf8ToAnsi(citizen->GetName()) << endl;
					cout << "Age: " << citizen->GetAge(player->GetTime()) << endl;
					cout << "Options: " << endl;
					for (auto option : citizen->GetOptions()) {
						cout << "--" << Utf8ToAnsi(option) << endl;
					}
				}
				else if (parser.HasOption("--name")) { // 按名字查找
					string name = parser.GetOption("--name");
					if (name.size() == 0) {
						THROW_EXCEPTION(CommandException, "Wrong input citizen name.");
					}
					for (auto citizen : populace->GetCitizens()) {
						if (citizen->GetName() == name) {
							cout << "Citizen ID: " << citizen->GetId() << endl;
							cout << "Name: " << Utf8ToAnsi(citizen->GetName()) << endl;
							cout << "Age: " << player->GetTime()->GetYear() - citizen->GetBirthday().GetYear() << endl;
							cout << "Options: " << endl;
							for (auto option : citizen->GetOptions()) {
								cout << "--" << Utf8ToAnsi(option) << endl;
							}
							break;
						}
					}
				}
				break;
			}
			case CMD_PRINT: { // 输出当前状态
				break;
			}
			default:
				THROW_EXCEPTION(CommandException, "Wrong input format.");
				break;
			}

			// 输出帮助
			if (parser.HasOption("--help")) {
				parser.PrintHelp(type);
			}
		}
		catch (ExceptionBase& e) {
			cout << e.GetDetailedInfo() << endl;

			switch (type) {
			case CMD_INIT:
				//::map->Destroy();
				break;
			default:
				break;
			}
		}

	} while (type != CMD_EXIT);

	for (auto modHandle : modHandles) {
		FreeLibrary(modHandle.second);
	}

	return 0;
}