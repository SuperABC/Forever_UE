#include "parser.h"
#include "utility.h"
#include "error.h"

#include "map/map.h"
#include "populace/populace.h"
#include "society/society.h"
#include "story/story.h"
#include "industry/industry.h"
#include "traffic/traffic.h"
#include "player/player.h"

#include <iostream>
#include <memory>
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

// ANSI字符串和UTF-8字符串转换
string AnsiToUtf8(const string& ansiStr) {
	// 第一步：ANSI -> UTF-16
	int lenWide = MultiByteToWideChar(CP_ACP, 0, ansiStr.c_str(), -1, nullptr, 0);
	if (lenWide == 0) return "";   // 转换失败
	vector<wchar_t> wideBuf(lenWide);
	MultiByteToWideChar(CP_ACP, 0, ansiStr.c_str(), -1, wideBuf.data(), lenWide);

	// 第二步：UTF-16 -> UTF-8
	int lenUtf8 = WideCharToMultiByte(CP_UTF8, 0, wideBuf.data(), -1, nullptr, 0, nullptr, nullptr);
	if (lenUtf8 == 0) return "";
	vector<char> utf8Buf(lenUtf8);
	WideCharToMultiByte(CP_UTF8, 0, wideBuf.data(), -1, utf8Buf.data(), lenUtf8, nullptr, nullptr);

	return string(utf8Buf.data());
}
string Utf8ToAnsi(const string& utf8Str) {
	// 第一步：UTF-8 -> UTF-16
	int lenWide = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
	if (lenWide == 0) return "";
	vector<wchar_t> wideBuf(lenWide);
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, wideBuf.data(), lenWide);

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
	string target = "";
	string npc = "";
	string option = "";
	int num = 0;
	int level = 0;
	bool status = false;

	if (type == "game_start") {
		return new GameStartEvent();
	}
	else if (type == "game_success") {
		string result = "";
		if (parser.HasOption("--result")) result = parser.GetOption("--result");
		return new GameSuccessEvent(result);
	}
	else if (type == "game_fail") {
		string result = "";
		if (parser.HasOption("--result")) result = parser.GetOption("--result");
		return new GameFailEvent(result);
	}
	else if (type == "option_dialog") {
		if (parser.HasOption("--id")) id = stoi(parser.GetOption("--id"));
		if (parser.HasOption("--target")) target = parser.GetOption("--target");
		if (parser.HasOption("--option")) option = parser.GetOption("--option");
		if (target.size() == 0)
			return new OptionDialogEvent(id, option);
		else {
			return new OptionDialogEvent(target, option);
		}
	}
	else if (type == "npc_meet") {
		if (parser.HasOption("--npc")) npc = parser.GetOption("--npc");
		return new NPCMeetEvent(npc);
	}
	else if (type == "citizen_born") {
		string name = "";
		GENDER_TYPE gender = GENDER_MALE;
		if (parser.HasOption("--name")) name = parser.GetOption("--name");
		if (parser.HasOption("--gender")) {
			string gender_str = parser.GetOption("--gender");
			if (gender_str == "male") gender = GENDER_MALE;
			else if (gender_str == "female") gender = GENDER_FEMALE;
		}
		return new CitizenBornEvent(name, gender);
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
	else if (type == "deposit_change") {
		int result = 0;
		int delta = 0;
		if (parser.HasOption("--result")) result = stoi(parser.GetOption("--result"));
		if (parser.HasOption("--delta")) delta = stoi(parser.GetOption("--delta"));
		return new DepositChangeEvent(result, delta);
	}
	else if (type == "cash_change") {
		int result = 0;
		int delta = 0;
		if (parser.HasOption("--result")) result = stoi(parser.GetOption("--result"));
		if (parser.HasOption("--delta")) delta = stoi(parser.GetOption("--delta"));
		return new CashChangeEvent(result, delta);
	}
	else if (type == "get_item") {
		string item = "";
		int num = 0;
		if (parser.HasOption("--item")) item = parser.GetOption("--item");
		if (parser.HasOption("--num")) num = stoi(parser.GetOption("--num"));
		return new GetItemEvent(item, num);
	}
	else if (type == "lose_item") {
		string item = "";
		int num = 0;
		if (parser.HasOption("--item")) item = parser.GetOption("--item");
		if (parser.HasOption("--num")) num = stoi(parser.GetOption("--num"));
		return new LoseItemEvent(item, num);
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
	else if (type == "count_up") {
		string counter = "";
		if (parser.HasOption("--counter")) counter = parser.GetOption("--counter");
		return new CountUpEvent(counter);
	}
	else if (type == "use_skill") {
		string skill = "";
		if (parser.HasOption("--skill")) skill = parser.GetOption("--skill");
		return new UseSkillEvent(skill);
	}
	else if (type == "skill_change") {
		string skill = "";
		int level = 0;
		if (parser.HasOption("--skill")) skill = parser.GetOption("--skill");
		if (parser.HasOption("--level")) level = stoi(parser.GetOption("--level"));
		return new SkillChangeEvent(skill, level);
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
	else if (type == "escape_success") {
		string enemy = "";
		if (parser.HasOption("--enemy")) enemy = parser.GetOption("--enemy");
		return new EscapeSuccessEvent(enemy);
	}
	else if (type == "escape_fail") {
		string enemy = "";
		if (parser.HasOption("--enemy")) enemy = parser.GetOption("--enemy");
		return new EscapeFailEvent(enemy);
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

// 递归输出对话与选项
bool PrintDialog(Dialog& dialog, vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	auto sections = dialog.GetDialogs();
	if (sections.size() == 0)return false;

	for (auto section : sections) {
		if (section.IsBranch()) { // 如果当前段是选项
			auto options = section.GetOptions();
			vector<int> indices;
			int i = 0, j = 0;
			for (auto option : options) {
				if (story->JudgeCondition(option.GetCondition(), getValues)) {
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
				if (story->JudgeCondition(dialog.GetCondition())) {
					PrintDialog(dialog, getValues);
				}
			}
			for (auto change : selected.GetChanges()) {
				if (!story->JudgeCondition(change->GetCondition()))continue;
				::map->ApplyChange(change, story, getValues);
				populace->ApplyChange(change, story, getValues);
				society->ApplyChange(change, story, getValues);
				story->ApplyChange(change, story, getValues);
				industry->ApplyChange(change, story, getValues);
				traffic->ApplyChange(change, story, getValues);
				player->ApplyChange(change, story, getValues);
			}
		}
		else { // 如果当前段是对话
			Condition conditionContent;
			conditionContent.ParseCondition(section.GetSpeaking().second);
			if (section.GetSpeaking().first.size() == 0) {
				cout << Utf8ToAnsi(ToString(conditionContent.EvaluateValue(getValues))) << endl;
			}
			else {
				Condition conditionSpeaker;
				conditionSpeaker.ParseCondition(section.GetSpeaking().first);
				cout << Utf8ToAnsi(ToString(conditionSpeaker.EvaluateValue(getValues))) << ": " <<
					Utf8ToAnsi(ToString(conditionContent.EvaluateValue(getValues))) << endl;
			}
		}
	}
	return true;
}

int main() {
	std::unordered_map<string, HMODULE> modHandles;

	// 读取Map相关类及Mod
	::map->SetResourcePath(REPLACE_PATH("../Resources/"));
	::map->ReadConfigs("configs/config_map.json");
	::map->InitTerrains(modHandles);
	::map->InitRoadnets(modHandles);
	::map->InitZones(modHandles);
	::map->InitBuildings(modHandles);
	::map->InitComponents(modHandles);
	::map->InitRooms(modHandles);

	// 读取Populace相关类及Mod
	populace->SetResourcePath(REPLACE_PATH("../Resources/"));
	populace->ReadConfigs("configs/config_populace.json");
	populace->InitAssets(modHandles);
	populace->InitJobs(modHandles);
	populace->InitNames(modHandles);
	populace->InitSchedulers(modHandles);

	// 读取Society相关类及Mod
	society->SetResourcePath(REPLACE_PATH("../Resources/"));
	society->ReadConfigs("configs/config_society.json");
	society->InitCalendars(modHandles);
	society->InitOrganizations(modHandles);

	// 读取Story相关类及Mod
	story->SetResourcePath(REPLACE_PATH("../Resources/"));
	story->ReadConfigs("configs/config_story.json");
	story->InitEvents(modHandles);
	story->InitChanges(modHandles);

	// 读取Industry相关类及Mod
	industry->SetResourcePath(REPLACE_PATH("../Resources/"));
	industry->ReadConfigs("configs/config_industry.json");

	// 读取Traffic相关类及Mod
	traffic->SetResourcePath(REPLACE_PATH("../Resources/"));
	traffic->ReadConfigs("configs/config_traffic.json");

	// 读取Player相关类及Mod
	traffic->SetResourcePath(REPLACE_PATH("../Resources/"));
	traffic->ReadConfigs("configs/config_player.json");

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
				parser.AddOption("--story", 0, "Story file.", true, REPLACE_PATH("../Resources/scripts/ys.json"));
				parser.ParseCmd(cmd);

				int size = atoi(parser.GetOption("--block").data());
				string path = parser.GetOption("--story");

				story->Init();
				int accomodation = ::map->Init(size, size);
				populace->Init(accomodation, story->ReadNames(path), player->GetTime());
				::map->Checkin(populace->GetCitizens(), player->GetTime());
				society->Init(::map, populace, player->GetTime());
				story->InitVariables(player->GetTime());
				story->ReadStory(path);

				break;
			}
			case CMD_PASS: { // 时间流逝
				break;
			}
			case CMD_EVENT: { // 文本模拟事件
				parser.AddOption("--type", 0, "Event type.", true, "nothing_happen");

				parser.AddOption("--id", 0, "Event target id.", true, "");
				parser.AddOption("--target", 0, "Event target name.", true, "");
				parser.AddOption("--option", 0, "Event option.", true, "");

				parser.ParseCmd(cmd);
				auto event = ParseEvent(parser);
				if (!event) {
					THROW_EXCEPTION(CommandException, "Wrong input event format.");
				}

				if (true) { // 全局事件
					vector<function<pair<bool, ValueType>(const string&)>> getValues = {
						[&](string name) -> pair<bool, ValueType> {
							return story->GetValue(name);
						}
					};

					auto actions = story->MatchEvent(event);
					auto dialogs = actions.first;
					auto changes = actions.second;
					for (auto dialog : dialogs) {
						if (story->JudgeCondition(dialog.GetCondition())) {
							if (PrintDialog(dialog, getValues)) {
								break;
							}
						}
					}
					for (auto change : changes) {
						if (!story->JudgeCondition(change->GetCondition()))continue;
						::map->ApplyChange(change, story, getValues);
						populace->ApplyChange(change, story, getValues);
						society->ApplyChange(change, story, getValues);
						story->ApplyChange(change, story, getValues);
						industry->ApplyChange(change, story, getValues);
						traffic->ApplyChange(change, story, getValues);
						player->ApplyChange(change, story, getValues);
					}
				}
				/*
				if (event->GetType() == "game_start") { // 市民独立开始事件
					for (int i = 0; i < populace->GetCitizens().size(); i++) {
						auto actions = populace->TriggerEvent(i, event, story);
						auto dialogs = actions.first;
						auto changes = actions.second;
						vector<function<pair<bool, ValueType>(const string&)>> getValues = {
							[&](string name) -> pair<bool, ValueType> {
								return story->GetValue(name);
							},
							[&](string name) -> pair<bool, ValueType> {
								return populace->GetCitizens()[i]->GetValue(name);
							}
						};
						for (auto dialog : dialogs) {
							if (story->JudgeCondition(dialog.GetCondition())) {
								if (PrintDialog(dialog, getValues)) {
									break;
								}
							}
						}
						for (auto change : changes) {
							if (!story->JudgeCondition(change->GetCondition(), populace->GetCitizens()[i]))continue;
							::map->ApplyChange(change, story);
							populace->ApplyChange(change, ::map, story, populace->GetCitizens()[i]);
							story->ApplyChange(change);
						}
					}
				}
				if (event->GetType() == "option_dialog") { // 市民独立对话事件
					auto target = dynamic_pointer_cast<OptionDialogEvent>(event)->GetTarget();
					auto idx = dynamic_pointer_cast<OptionDialogEvent>(event)->GetIdx();
					pair<vector<Dialog>, vector<shared_ptr<Change>>> actions;
					shared_ptr<Person> person = nullptr;
					if (target.size() == 0) {
						if (idx >= populace->GetCitizens().size() || idx < 0) {
							THROW_EXCEPTION(CommandException, "Wrong input citizen ID.");
						}
						actions = populace->TriggerEvent(idx, event, story);
						person = populace->GetCitizens()[idx];
					}
					else {
						actions = populace->TriggerEvent(target, event, story);
						person = populace->GetCitizen(target);
					}

					if (person == nullptr) {
						THROW_EXCEPTION(CommandException, "Citizen not found.");
					}

					vector<function<pair<bool, ValueType>(const string&)>> getValues = {
						[&](string name) -> pair<bool, ValueType> {
							return story->GetValue(name);
						},
						[&](string name) -> pair<bool, ValueType> {
							return person->GetValue(name);
						}
					};

					auto dialogs = actions.first;
					auto changes = actions.second;
					for (auto dialog : dialogs) {
						if (story->JudgeCondition(dialog.GetCondition(), person)) {
							if (PrintDialog(dialog, getValues)) {
								break;
							}
						}
					}
					for (auto change : changes) {
						if (!story->JudgeCondition(change->GetCondition()))continue;
						::map->ApplyChange(change, story);
						populace->ApplyChange(change, ::map, story);
						story->ApplyChange(change);
					}
				}
				*/

				delete event;
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
				::map->Destroy();
				break;
			default:
				break;
			}
		}

	} while (type != CMD_EXIT);

	return 0;
}