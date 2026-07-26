#include "implement.h"

#include "map/map.h"
#include "map/room.h"
#include "populace/populace.h"
#include "populace/person.h"
#include "society/society.h"
#include "society/job.h"
#include "story/story.h"
#include "industry/industry.h"
#include "traffic/traffic.h"
#include "player/player.h"


using namespace std;

PostImplement::PostImplement(Map* map, Populace* populace, Society* society,
	Story* story, Industry* industry, Traffic* traffic, Player* player) :
	result(DATA_OBJECT),
	map(map),
	populace(populace),
	society(society),
	story(story),
	industry(industry),
	traffic(traffic),
	player(player) {
}

PostImplement::~PostImplement() {

}

void PostImplement::Post(const JsonValue& request) {
	result = JsonValue(DATA_OBJECT);

	if (request.IsObject() && !request["post"].IsNull()) {
		if (request["post"].AsString() == "random room") {
			auto type = request["type"].AsString();
			auto room = RandomRoom(type);
			if (!room) {
				result["result"] = "fail";
				result["msg"] = "no room with type " + type + " found.";
				return;
			}
			else {
				result["result"] = "success";
				result["name"] = room->GetName();
				result["address"] = room->GetAddress();
				return;
			}
		}
		else if (request["post"].AsString() == "lookup citizen") {
			auto name = request["name"].AsString();
			auto citizen = populace->GetCitizen(name);
			if (!citizen) {
				result["result"] = "fail";
				result["msg"] = "no citizen named " + name + " found.";
				return;
			}
			else {
				result["result"] = "success";
				auto& citizenInfo = result["citizen"];
				citizenInfo["name"] = citizen->GetName();
				citizenInfo["avatar"] = citizen->GetAvatar();
				citizenInfo["gender"] = citizen->GetGender() == GENDER_MALE ? "male" : "female";
				citizenInfo["birthday"] = citizen->GetBirthday().ToString(true, false);
				citizenInfo["age"] = citizen->GetAge(player->GetTime());
				citizenInfo["height"] = citizen->GetHeight();
				citizenInfo["weight"] = citizen->GetWeight();
				citizenInfo["nick"] = citizen->GetNick();
				citizenInfo["deposit"] = citizen->GetDeposit();
				citizenInfo["phone"] = citizen->GetPhone();
				citizenInfo["home"] = citizen->GetHome() ? citizen->GetHome()->GetName() : "";
				citizenInfo["room"] = citizen->GetCurrentRoom() ? citizen->GetCurrentRoom()->GetName() : "";
				auto jobs = JsonValue(DATA_ARRAY);
				for (auto job : citizen->GetJobs()) {
					jobs.append(job->GetName());
				}
				citizenInfo["jobs"] = jobs;
				return;
			}
		}
	}

	result["result"] = "fail";
	result["msg"] = "post not found.";
}

const JsonValue& PostImplement::GetResult() const {
	return result;
}

Room* PostImplement::RandomRoom(string type) {
	auto rooms = map->GetRooms();
	vector<int> ids;
	for (int i = 0; i < rooms.size(); i++) {
		if (rooms[i]->GetType() == type) {
			ids.push_back(i);
		}
	}
	if (ids.empty())return nullptr;
	else return rooms[ids[GetRandom(static_cast<int>(ids.size()))]];
}
