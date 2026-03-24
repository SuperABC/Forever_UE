#include "room_mod.h"


using namespace std;

RoomMod::RoomMod() {

}

RoomMod::~RoomMod() {

}

void RoomFactory::RegisterRoom(const string& id,
	function<RoomMod* ()> creator, function<void(RoomMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void RoomFactory::RemoveAll() {
	for (auto& config : configs) {
		config.second = false;
	}
}

RoomMod* RoomFactory::CreateRoom(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Room %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Room %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Room " + id + " creater is null.\n");
	}

	return nullptr;
}

bool RoomFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void RoomFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

void RoomFactory::DestroyRoom(RoomMod* terrain) const {
	if (!terrain) {
		debugf("Warning: Room is null when deleting.\n");
		return;
	}

	auto it = registries.find(terrain->GetType());
	if (it == registries.end()) {
		debugf("Warning: Room %s not registered when deleting.\n", terrain->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(terrain);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Room " + string(terrain->GetType()) + " deleter is null.\n");
	}
}

