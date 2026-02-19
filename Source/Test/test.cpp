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

int main() {
	std::unordered_map<string, HMODULE> modHandles;

	// 读取Map相关类及Mod
	::map->ReadConfigs(REPLACE_PATH("../Resources/configs/config_map.json"));
	::map->InitTerrains(modHandles);
	::map->InitRoadnets(modHandles);
	::map->InitZones(modHandles);
	::map->InitBuildings(modHandles);
	::map->InitComponents(modHandles);
	::map->InitRooms(modHandles);

	// 读取Populace相关类及Mod
	populace->ReadConfigs(REPLACE_PATH("../Resources/configs/config_populace.json"));

	// 读取Society相关类及Mod
	society->ReadConfigs(REPLACE_PATH("../Resources/configs/config_society.json"));

	// 读取Story相关类及Mod
	story->ReadConfigs(REPLACE_PATH("../Resources/configs/config_story.json"));

	// 读取Industry相关类及Mod
	industry->ReadConfigs(REPLACE_PATH("../Resources/configs/config_industry.json"));

	// 读取Traffic相关类及Mod
	traffic->ReadConfigs(REPLACE_PATH("../Resources/configs/config_traffic.json"));

	// 读取Player相关类及Mod
	traffic->ReadConfigs(REPLACE_PATH("../Resources/configs/config_player.json"));

	// 读取命令行
	string cmd;
	CMD_TYPE type = CMD_UNKOWN;
	do {
		// 读取一行命令
		string input;
		getline(cin, input);

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
				::map->Init(size, size);
				break;
			}
			case CMD_PASS: { // 时间流逝
				break;
			}
			case CMD_EVENT: { // 文本模拟事件
				break;
			}
			case CMD_LOOKUP: { // 查找信息
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