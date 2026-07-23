#include "implement.h"


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
