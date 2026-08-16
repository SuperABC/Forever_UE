#include "building_mod.h"


using namespace std;

BuildingMod::BuildingMod() :
	maxAcreage(10000.f),
	minAcreage(0.f),
	layers(1),
	basements(0),
	height(0.4f),
	construction(),
	pivots(),
	wallTexture(),
	ceilingTexture(),
	floorTexture(),
	templates(),
	elevators(),
	singles(),
	rows(),
	decorations(),
	script({ "empty", {} }) {

}

BuildingMod::~BuildingMod() {

}

void BuildingMod::AllocateFloors() {
	templates.resize(basements + layers);
}

void BuildingMod::AddElevator(string name, int temp, int idx, int from, int to, string script, vector<string> paths) {
	elevators.push_back({ name, temp, idx, from, to, script, paths });
}

void BuildingMod::AssignFloor(int level, int face, string layout) {
	templates.resize(basements + layers);
	level += basements;
	if (level < 0 || level >= templates.size()) {
		THROW_EXCEPTION(OutOfRangeException, "Assigning floor our of basements or layers range.\n");
	}
	templates[level] = { layout, face };
}

void BuildingMod::AssignFloors(int face, string layout) {
	templates.resize(basements + layers);
	for (int i = 0; i < basements + layers; i++) {
		templates[i] = { layout, face };
	}
}

void BuildingMod::AssignFloors(int face, vector<string> layouts) {
	if (layouts.size() != basements + layers) {
		THROW_EXCEPTION(RuntimeException, "Building layout numbers mismatch the floor numbers.\n");
	}
	templates.resize(basements + layers);
	for (int i = 0; i < basements + layers; i++) {
		templates[i] = { layouts[i], face };
	}
}

void BuildingMod::AssignRoom(int level, int slot, string room, string component, int id) {
	level += basements;
	singles[{ component, id }].emplace_back(level, slot, room);
}

void BuildingMod::ArrangeRow(int level, int slot, string room, float acreage, string component, int id) {
	level += basements;
	if (rows.find({ component, id }) == rows.end()) {
		rows[{ component, id }].resize(basements + layers);
	}
	rows[{ component, id }][level].emplace_back(slot, room, acreage);
}

void BuildingMod::AddDecoration(string path, const Quad* quad,
	float px, float py, float pz, float sx, float sy, float sz, float rx, float ry, float rz,
	int direction) {
	if (direction < 0 || direction >= 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Facing direction out of range [0,3].\n");
	}

	float footprintSizeX = quad->GetSizeX() * construction.GetSizeX();
	float footprintSizeY = quad->GetSizeY() * construction.GetSizeY();
	float xRatio = footprintSizeX > 0.f ? px / footprintSizeX : 0.f;
	float yRatio = footprintSizeY > 0.f ? py / footprintSizeY : 0.f;
	float rotatedX = px, rotatedY = py, yaw = rz;
	switch (direction) {
	case 0:
		rotatedX = yRatio * footprintSizeX;
		rotatedY = (1.f - xRatio) * footprintSizeY;
		yaw = rz + 270.f;
		break;
	case 1:
		rotatedX = (1.f - yRatio) * footprintSizeX;
		rotatedY = xRatio * footprintSizeY;
		yaw = rz + 90.f;
		break;
	case 2:
		break;
	case 3:
		rotatedX = (1.f - xRatio) * footprintSizeX;
		rotatedY = (1.f - yRatio) * footprintSizeY;
		yaw = rz + 180.f;
		break;
	}

	decorations.push_back({ path, {rotatedX, rotatedY, pz, sx, sy, sz, rx, ry, yaw} });
}

void BuildingMod::AddPivot(vector<float> point, int face) {
	if (face < 0 || face >= 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Facing direction out of range [0,3].\n");
	}
	if (point.size() != POINT_PARAM_COUNT) {
		THROW_EXCEPTION(InvalidArgumentException, "Pivot must have 4 elements.\n");
	}

	auto rotated = point;
	switch (face) {
	case 0:
		rotated[0] = point[2];
		rotated[1] = point[3];
		rotated[2] = 1.f - point[0];
		rotated[3] = -point[1];
		break;
	case 1:
		rotated[0] = 1.f - point[2];
		rotated[1] = -point[3];
		rotated[2] = point[0];
		rotated[3] = point[1];
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

void BuildingMod::AssignConstruction(int direction, float x, float y, float sizeX, float sizeY) {
	if (direction < 0 || direction >= 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Facing direction out of range [0,3].\n");
	}

	float posX = x, posY = y, sx = sizeX, sy = sizeY;
	switch (direction) {
	case FACE_WEST:
		posX = y;
		posY = 1.f - x;
		sx = sizeY;
		sy = sizeX;
		break;
	case FACE_EAST:
		posX = 1.f - y;
		posY = x;
		sx = sizeY;
		sy = sizeX;
		break;
	case FACE_NORTH:
		break;
	case FACE_SOUTH:
		posX = 1.f - x;
		posY = 1.f - y;
		break;
	default:
		break;
	}

	construction = Quad(posX, posY, sx, sy);
}

BuildingFactory::BuildingFactory()
	: registries(),
	configs(),
	powers(),
	assigners(),
	temp() {
	temp.registries.reserve(TEMP_RESERVE_CAPACITY);
	temp.powers.reserve(TEMP_RESERVE_CAPACITY);
	temp.assigners.reserve(TEMP_RESERVE_CAPACITY);
}

void BuildingFactory::RegisterBuilding(const string& id,
	const vector<float>& power, function<int(const Lot*, int, int)> assigner,
	function<BuildingMod* ()> creator, function<void(BuildingMod*)> deleter) {
	temp.registries[id] = { creator, deleter };
	temp.powers[id] = power;
	temp.assigners[id] = assigner;
}

void BuildingFactory::MergeTemp() {
	for (auto& [id, registry] : temp.registries) {
		registries[id] = registry;
	}
	for (auto& [id, power] : temp.powers) {
		powers[id] = power;
	}
	for (auto& [id, assigner] : temp.assigners) {
		assigners[id] = assigner;
	}
}

void BuildingFactory::CleanTemp() {
	temp.registries.clear();
	temp.powers.clear();
	temp.assigners.clear();
}

void BuildingFactory::RemoveAll() {
	for (auto& [_, enabled] : configs) {
		enabled = false;
	}
}

BuildingMod* BuildingFactory::CreateBuilding(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Building %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Building %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Building " + id + " creater is null.\n");
	}

	return nullptr;
}

bool BuildingFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void BuildingFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

unordered_map<string, vector<float>> BuildingFactory::GetPowers() const {
	auto result = powers;
	for (auto& [name, _] : powers) {
		if (configs.find(name) == configs.end() || !configs.at(name)) {
			result.erase(name);
		}
	}
	return result;
}

vector<string> BuildingFactory::CreateBuildings(const string& type, const Lot* lot, int idx, int total) const {
	vector<string> buildings;
	auto config = configs.find(type);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Building %s not enabled when creating.\n", type.data());
		return buildings;
	}

	auto assigner = assigners.find(type);
	if (assigner == assigners.end()) {
		debugf("Warning: Generator for building %s not found when creating.\n", type.data());
		return buildings;
	}

	int buildingCount = assigner->second(lot, idx, total);
	for (int i = 0; i < buildingCount; i++) {
		buildings.push_back(type);
	}
	return buildings;
}

void BuildingFactory::DestroyBuilding(BuildingMod* buildingMod) const {
	if (!buildingMod) {
		debugf("Warning: Building is null when deleting.\n");
		return;
	}

	auto it = registries.find(buildingMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: Building %s not registered when deleting.\n", buildingMod->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(buildingMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Building " + string(buildingMod->GetType()) + " deleter is null.\n");
	}
}
