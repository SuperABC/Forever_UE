#include "room_mod.h"

#include "building_mod.h"


using namespace std;

RoomMod::RoomMod() :
	isResidential(false),
	isWorkspace(false),
	isStorage(false),
	isManufacture(false),
	isParking(false),
	residentialCapacity(0),
	workspaceCapacity(0),
	storageConfig(),
	manufactureTypes(),
	parkingSpaces(),
	pivots() {

}

RoomMod::~RoomMod() {

}

void RoomMod::AddPivot(vector<float> point, int face) {
	if (face < 0 || face >= FACE_DIRECTION_COUNT) {
		THROW_EXCEPTION(InvalidArgumentException, "Facing direction out of range [0,3].\n");
	}
	if (point.size() != POINT_PARAM_COUNT) {
		THROW_EXCEPTION(InvalidArgumentException, "Pivot must have 4 elements.\n");
	}

	auto rotated = point;
	switch (face) {
	case 0:
		rotated[0] = 1.f - point[2];
		rotated[1] = -point[3];
		rotated[2] = point[0];
		rotated[3] = point[1];
		break;
	case 1:
		rotated[0] = point[2];
		rotated[1] = point[3];
		rotated[2] = 1.f - point[0];
		rotated[3] = -point[1];
		break;
	case 2:
		break;
	case 3:
		rotated[0] = 1.f - point[0];
		rotated[1] = -point[1];
		rotated[2] = 1.f - point[2];
		rotated[3] = -point[3];
		break;
	default:
		break;
	}

	pivots.push_back(rotated);
}

void RoomFactory::RegisterRoom(const string& id,
	function<RoomMod* ()> creator, function<void(RoomMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void RoomFactory::RemoveAll() {
	for (auto& [_, enabled] : configs) {
		enabled = false;
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

void RoomFactory::DestroyRoom(RoomMod* roomMod) const {
	if (!roomMod) {
		debugf("Warning: Room is null when deleting.\n");
		return;
	}

	auto it = registries.find(roomMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: Room %s not registered when deleting.\n", roomMod->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(roomMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Room " + string(roomMod->GetType()) + " deleter is null.\n");
	}
}

