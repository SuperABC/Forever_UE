#include "building.h"

#include "json.h"

#include "map/map.h"
#include "map/block.h"
#include "map/zone.h"
#include "map/component.h"
#include "map/room.h"
#include "populace/person.h"
#include "story/story.h"
#include "story/script.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <tuple>


using namespace std;

Stair::Stair(vector<float> params) :
	direction(FACE_WEST),
	walls(4, false),
	params(params) {

}

Stair::~Stair() {

}

FACE_DIRECTION Stair::GetDirection() const {
	return direction;
}

void Stair::SetDirection(FACE_DIRECTION direction) {
	this->direction = direction;
}

bool Stair::GetWall(int direction) const {
	if (direction < 0 || direction >= 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Stair wall direction out of range [0, 3].\n");
	}
	return walls[direction];
}

void Stair::AddWall(int direction) {
	if (direction < 0 || direction >= 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Stair wall direction out of range [0, 3].\n");
	}
	walls[direction] = true;
}

void Stair::InstanciateQuad(float width, float height) {
	if (params.size() != RECT_PARAM_COUNT) {
		THROW_EXCEPTION(InvalidArgumentException, "Stair rect must have 8 elements.\n");
	}

	float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
	float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
	float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
	float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
	Quad::SetPosition(x, y, w, h);
}

Elevator::Elevator(vector<float> params) :
	direction(FACE_WEST),
	walls(4, false),
	params(params) {

}

Elevator::~Elevator() {

}

FACE_DIRECTION Elevator::GetDirection() const {
	return direction;
}

void Elevator::SetDirection(FACE_DIRECTION direction) {
	this->direction = direction;
}

bool Elevator::GetWall(int direction) const {
	if (direction < 0 || direction >= 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Elevator wall direction out of range [0, 3].\n");
	}
	return walls[direction];
}

void Elevator::AddWall(int direction) {
	if (direction < 0 || direction >= 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Elevator wall direction out of range [0, 3].\n");
	}
	walls[direction] = true;
}

void Elevator::InstanciateQuad(float width, float height) {
	if (params.size() != RECT_PARAM_COUNT) {
		THROW_EXCEPTION(InvalidArgumentException, "Elevator rect must have 8 elements.\n");
	}

	float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
	float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
	float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
	float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
	Quad::SetPosition(x, y, w, h);
}

Ramp::Ramp(vector<float> params) :
	direction(FACE_WEST),
	walls(4, false),
	params(params) {

}

Ramp::~Ramp() {

}

FACE_DIRECTION Ramp::GetDirection() const {
	return direction;
}

void Ramp::SetDirection(FACE_DIRECTION direction) {
	this->direction = direction;
}

bool Ramp::GetWall(int direction) const {
	if (direction < 0 || direction >= 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Ramp wall direction out of range [0, 3].\n");
	}
	return walls[direction];
}

void Ramp::AddWall(int direction) {
	if (direction < 0 || direction >= 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Ramp wall direction out of range [0, 3].\n");
	}
	walls[direction] = true;
}

void Ramp::InstanciateQuad(float width, float height) {
	if (params.size() != RECT_PARAM_COUNT) {
		THROW_EXCEPTION(InvalidArgumentException, "Ramp rect must have 8 elements.\n");
	}

	float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
	float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
	float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
	float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
	Quad::SetPosition(x, y, w, h);
}

Ceiling::Ceiling(vector<float> params) :
	params(params) {

}

Ceiling::~Ceiling() {

}

void Ceiling::InstanciateQuad(float width, float height) {
	if (params.size() != RECT_PARAM_COUNT) {
		THROW_EXCEPTION(InvalidArgumentException, "Ceiling rect must have 8 elements.\n");
	}

	float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
	float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
	float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
	float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
	Quad::SetPosition(x, y, w, h);
}

Ground::Ground(vector<float> params) :
	params(params) {

}

Ground::~Ground() {

}

void Ground::InstanciateQuad(float width, float height) {
	if (params.size() != RECT_PARAM_COUNT) {
		THROW_EXCEPTION(InvalidArgumentException, "Ground rect must have 8 elements.\n");
	}

	float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
	float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
	float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
	float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
	Quad::SetPosition(x, y, w, h);
}

Corridor::Corridor(vector<float> params) :
	walls(4, false),
	doors(),
	windows(),
	params(params) {

}

Corridor::~Corridor() {

}

bool Corridor::GetWall(int direction) const {
	if (direction < 0 || direction >= 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Corridor wall direction out of range [0, 3].\n");
	}

	return walls[direction];
}

void Corridor::AddWall(int direction) {
	if (direction < 0 || direction >= 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Corridor wall direction out of range [0, 3].\n");
	}

	walls[direction] = true;
}

unordered_map<FACE_DIRECTION, vector<pair<vector<float>, Quad>>> Corridor::GetDoors() const {
	return doors;
}

void Corridor::AddDoor(FACE_DIRECTION direction, vector<vector<float>> positions) {
	for (auto& p : positions) {
		doors[direction].push_back({ p, Quad() });
	}
}

unordered_map<FACE_DIRECTION, vector<pair<vector<float>, Quad>>> Corridor::GetWindows() const {
	return windows;
}

void Corridor::AddWindow(FACE_DIRECTION direction, vector<vector<float>> positions) {
	for (auto& p : positions) {
		windows[direction].push_back({ p, Quad() });
	}
}

void Corridor::InstanciateQuad(float width, float height) {
	if (params.size() != RECT_PARAM_COUNT) {
		THROW_EXCEPTION(InvalidArgumentException, "Corridor rect must have 8 elements.\n");
	}

	float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
	float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
	float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
	float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
	Quad::SetPosition(x, y, w, h);
}

Hatch::Hatch(vector<float> params) :
	params(params) {

}

Hatch::~Hatch() {

}

void Hatch::InstanciateQuad(float width, float height) {
	if (params.size() != RECT_PARAM_COUNT) {
		THROW_EXCEPTION(InvalidArgumentException, "Hatch rect must have 8 elements.\n");
	}

	float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
	float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
	float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
	float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
	Quad::SetPosition(x, y, w, h);
}

Single::Single(vector<float> params) :
	direction(FACE_WEST),
	doors(),
	windows(),
	params(params) {

}

Single::~Single() {

}

FACE_DIRECTION Single::GetDirection() const {
	return direction;
}

void Single::SetDirection(FACE_DIRECTION direction) {
	this->direction = direction;
}

unordered_map<FACE_DIRECTION, vector<pair<vector<float>, Quad>>> Single::GetDoors() const {
	return doors;
}

void Single::AddDoor(FACE_DIRECTION direction, vector<vector<float>> positions) {
	for (auto& p : positions) {
		doors[direction].push_back({ p, Quad() });
	}
}

unordered_map<FACE_DIRECTION, vector<pair<vector<float>, Quad>>> Single::GetWindows() const {
	return windows;
}

void Single::AddWindow(FACE_DIRECTION direction, vector<vector<float>> positions) {
	for (auto& p : positions) {
		windows[direction].push_back({ p, Quad() });
	}
}

void Single::InstanciateQuad(float width, float height) {
	if (params.size() != RECT_PARAM_COUNT) {
		THROW_EXCEPTION(InvalidArgumentException, "Single rect must have 8 elements.\n");
	}

	float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
	float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
	float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
	float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
	Quad::SetPosition(x, y, w, h);
}

Row::Row(vector<float> params) :
	direction(FACE_WEST),
	doors(),
	windows(),
	params(params) {

}

Row::~Row() {

}

FACE_DIRECTION Row::GetDirection() const {
	return direction;
}

void Row::SetDirection(FACE_DIRECTION direction) {
	this->direction = direction;
}

unordered_map<FACE_DIRECTION, vector<pair<vector<float>, Quad>>> Row::GetDoors() const {
	return doors;
}

void Row::AddDoor(FACE_DIRECTION direction, vector<vector<float>> positions) {
	for (auto& p : positions) {
		doors[direction].push_back({ p, Quad() });
	}
}

unordered_map<FACE_DIRECTION, vector<pair<vector<float>, Quad>>> Row::GetWindows() const {
	return windows;
}

void Row::AddWindow(FACE_DIRECTION direction, vector<vector<float>> positions) {
	for (auto& p : positions) {
		windows[direction].push_back({ p, Quad() });
	}
}

void Row::InstanciateQuad(float width, float height) {
	if (params.size() != RECT_PARAM_COUNT) {
		THROW_EXCEPTION(InvalidArgumentException, "Row rect must have 8 elements.\n");
	}

	float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
	float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
	float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
	float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
	Quad::SetPosition(x, y, w, h);
}

Floor::Floor(int level, float width, float height) :
	level(level),
	number(0) {
	SetVertices(0, 0, width, height);
}

Floor::~Floor() {

}

int Floor::GetLevel() const {
	return level;
}

vector<Stair>& Floor::GetStairs() {
	return stairs;
}

vector<Elevator>& Floor::GetElevators() {
	return elevators;
}

vector<Ramp>& Floor::GetRamps() {
	return ramps;
}

vector<Ceiling>& Floor::GetCeilings() {
	return ceilings;
}

vector<Ground>& Floor::GetGrounds() {
	return grounds;
}

vector<Corridor>& Floor::GetCorridors() {
	return corridors;
}

vector<Single>& Floor::GetSingles() {
	return singles;
}

vector<Row>& Floor::GetRows() {
	return rows;
}

void Floor::AddStair(Stair stair) {
	stairs.push_back(stair);
}

void Floor::AddElevator(Elevator elevator) {
	elevators.push_back(elevator);
}

void Floor::AddRamp(Ramp ramp) {
	ramps.push_back(ramp);
}

void Floor::AddCeiling(Ceiling ceiling) {
	ceilings.push_back(ceiling);
}

void Floor::AddGround(Ground ground) {
	grounds.push_back(ground);
}

void Floor::AddCorridor(Corridor corridor) {
	corridors.push_back(corridor);
}

void Floor::AddSingle(Single single) {
	singles.push_back(single);
}

void Floor::AddRow(Row row) {
	rows.push_back(row);
}

vector<Hatch>& Floor::GetHatches() {
	return hatches;
}

void Floor::AddHatch(Hatch hatch) {
	hatches.push_back(hatch);
}

int Floor::AssignNumber() {
	return number++;
}

Cabin::Cabin(string name, int temp, int idx, int minFloor, int maxFloor, Script* script) :
	name(name),
	temp(temp),
	idx(idx),
	minFloor(minFloor),
	maxFloor(maxFloor),
	currentFloor(minFloor),
	script(script),
	options() {

}

Cabin::~Cabin() {
	delete script;
}

string Cabin::GetName() const {
	return name;
}

int Cabin::GetTemp() const {
	return temp;
}

int Cabin::GetIdx() const {
	return idx;
}

int Cabin::GetMinFloor() const {
	return minFloor;
}

int Cabin::GetMaxFloor() const {
	return maxFloor;
}

int Cabin::GetCurrentFloor() const {
	return currentFloor;
}

Script* Cabin::GetScript() const {
	return script;
}

void Cabin::AddOption(const string& option) {
	options.push_back(option);
}

void Cabin::RemoveOption(const string& option) {
	for (int i = 0; i < static_cast<int>(options.size()); i++) {
		if (options[i] == option) {
			options[i] = options.back();
			options.pop_back();
			return;
		}
	}
}

vector<string> Cabin::GetOptions() const {
	return options;
}

Building::Building(BuildingFactory* factory, const string& building) :
	mod(factory->CreateBuilding(building)),
	factory(factory),
	type(),
	name(),
	parentZone(nullptr),
	parentBlock(nullptr),
	layers(1),
	basements(0),
	height(0.4f),
	construction(),
	wallTexture(),
	ceilingTexture(),
	groundTexture(),
	decorations(),
	address(),
	pivots(),
	boundary(),
	nodes(),
	singleRoomBySlot(),
	rowRoomBySlot(),
	stated(),
	owner(),
	script(nullptr),
	cabins(),
	floors(),
	components(),
	rooms() {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Building " + building + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Building::~Building() {
	factory->DestroyBuilding(mod);

	for (auto& pivot : pivots) {
		delete pivot;
	}
	pivots.clear();

	for (auto node : nodes) {
		delete node;
	}
	nodes.clear();

	for (auto &cabin : cabins) {
		delete cabin;
	}
	cabins.clear();
	for (auto &floor : floors) {
		delete floor;
	}
	floors.clear();
	for (auto &component : components) {
		delete component;
	}
	components.clear();
	for (auto &room : rooms) {
		delete room;
	}
	rooms.clear();

	if (script) {
		delete script;
	}
	script = nullptr;
}

string Building::GetType() const {
	return type;
}

string Building::GetName() const {
	return name;
}

Block* Building::GetParentBlock() const {
	return parentBlock;
}

void Building::SetParent(Block* block) {
	parentBlock = block;
}

Zone* Building::GetParentZone() const {
	return parentZone;
}

void Building::SetParent(Zone* zone) {
	parentZone = zone;
}

int Building::GetLayers() const {
	return layers;
}

void Building::SetLayers(int layers) {
	this->layers = layers;
}

int Building::GetBasements() const {
	return basements;
}

void Building::SetBasements(int basements) {
	this->basements = basements;
}

float Building::GetHeight() const {
	return height;
}

void Building::SetHeight(float height) {
	this->height = height;
}

const Quad Building::GetConstruction() const {
	return construction;
}

void Building::PlaceConstruction() {
	mod->PlaceConstruction();
	construction = Quad(mod->construction.GetPosX() * GetSizeX(), mod->construction.GetPosY() * GetSizeY(),
		mod->construction.GetSizeX() * GetSizeX(), mod->construction.GetSizeY() * GetSizeY());
}

string Building::GetWallTexture() const {
	return wallTexture;
}

string Building::GetCeilingTexture() const {
	return ceilingTexture;
}

string Building::GetGroundTexture() const {
	return groundTexture;
}

vector<pair<string, vector<float>>>& Building::GetDecorations() {
	return decorations;
}

string Building::GetAddress() {
	if (address != "") {
		return address.data();
	}

	auto zone = GetParentZone();
	if (zone) {
		auto zoneAddress = GetParentZone()->GetAddress();
		address = zoneAddress + " " + GetName();
		return address;
	}
	else {
		auto block = GetParentBlock();
		if (!block) {
			THROW_EXCEPTION(NullPointerException, "Building parent block is null.\n");
		}
		auto blockAddress = block->GetAddress();
		address = blockAddress + " " + GetName();
		return address;
	}
}

const vector<Node*>& Building::GetPivots() {
	return pivots;
}

const QuadBoundary& Building::GetBoundary() const {
	return boundary;
}

void Building::SetBoundary(const QuadBoundary& boundary) {
	this->boundary = boundary;
}

const vector<Node*>& Building::GetNodes() const {
	return nodes;
}

vector<Component*>& Building::GetComponents() {
	return components;
}

vector<Room*>& Building::GetRooms() {
	return rooms;
}

Room* Building::GetRoom(const string& number) const {
	for (auto room : rooms) {
		if (room->GetNumber() == number) return room;
	}
	return nullptr;
}

Floor* Building::GetFloor(int level) const {
	int idx = basements + level;
	if (idx >= 0 && idx < floors.size())
		return floors[idx];
	return nullptr;
}

vector<Cabin*>& Building::GetCabins() {
	return cabins;
}

bool Building::GetStated() const {
	return stated;
}

void Building::SetStated(bool stated) {
	this->stated = stated;
}

Person* Building::GetOwner() const {
	return owner;
}

void Building::SetOwner(Person* owner) {
	this->owner = owner;
}

Script* Building::GetScript() const {
	return script;
}

float Building::RandomAcreage() {
	return mod->RandomAcreage();
}

float Building::GetAcreageMin() {
	return mod->minAcreage;
}

float Building::GetAcreageMax() {
	return mod->maxAcreage;
}

pair<float, float> Building::GetPosition() const {
	auto block = GetParentBlock();
	if (!block) {
		THROW_EXCEPTION(NullPointerException, "Building parent block is null.\n");
	}

	auto zone = GetParentZone();
	if (zone) {
		return block->GetPosition(zone->GetPosX() - zone->GetSizeX() / 2.f + GetPosX(),
			zone->GetPosY() - zone->GetSizeY() / 2.f + GetPosY());
	}
	return block->GetPosition(GetPosX(), GetPosY());
}

pair<float, float> Building::GetPosition(float x, float y) const {
	auto block = GetParentBlock();
	if (!block) {
		THROW_EXCEPTION(NullPointerException, "Building parent block is null.\n");
	}

	auto zone = GetParentZone();
	float offsetX = zone ? zone->GetLeft() : 0.f;
	float offsetY = zone ? zone->GetBottom() : 0.f;
	return block->GetPosition(offsetX + GetLeft() + construction.GetLeft() + x,
		offsetY + GetBottom() + construction.GetBottom() + y);
}

void Building::LayoutBuilding(Layout* layout, Map* map) {
	mod->LayoutBuilding(this);
	basements = mod->basements;
	layers = mod->layers;
	height = mod->height;
	wallTexture = mod->wallTexture;
	ceilingTexture = mod->ceilingTexture;
	groundTexture = mod->groundTexture;
	decorations = mod->decorations;

	for(auto cabin : mod->elevators) {
		auto script = new Script(Story::scriptFactory, get<5>(cabin));
		for (auto s : get<6>(cabin)) {
			script->ReadMilestones(Config::GetScript(s));
		}
		script->SetValue("self.name", get<0>(cabin));
		script->SetValue("self.min", get<3>(cabin));
		script->SetValue("self.max", get<4>(cabin));
		script->SetValue("self.parent.name", name);
		AddCabin(new Cabin(get<0>(cabin), get<1>(cabin), get<2>(cabin), get<3>(cabin), get<4>(cabin), script));
	}

	unordered_map<pair<string, int>, Component*, PairHash> componentMap;
	for (auto single : mod->singles) {
		if (componentMap.find(single.first) == componentMap.end()) {
			auto component = new Component(Map::componentFactory, single.first.first);
			component->InitComponent();
			componentMap[single.first] = component;
			components.push_back(component);
		}
	}
	for (auto row : mod->rows) {
		if (componentMap.find(row.first) == componentMap.end()) {
			auto component = new Component(Map::componentFactory, row.first.first);
			component->InitComponent();
			componentMap[row.first] = component;
			components.push_back(component);
		}
	}

	floors.resize(basements + layers);
	for (int i = 0; i < basements + layers; i++) {
		ReadFloor(i - basements, mod->templates[i].second, mod->templates[i].first, layout);
	}
	
	for (auto [component, singles] : mod->singles) {
		for (auto single : singles) {
			AssignRoom(get<0>(single) - basements, get<1>(single), get<2>(single), componentMap[component], Map::roomFactory);
		}
	}

	for (auto [component, rows] : mod->rows) {
		for (int i = 0; i < basements + layers; i++) {
			for (auto room : rows[i]) {
				ArrangeRow(i - basements, get<0>(room), get<1>(room), get<2>(room), componentMap[component], Map::roomFactory);
			}
		}
	}

	for (auto room : rooms) {
		room->ConfigRoom();
		room->PlacePivots(room);

		auto [wx, wy] = GetPosition(room->GetPosX(), room->GetPosY());
		room->SetNavigationNode(new Node("room", wx, wy, height * room->GetLayer()));
	}

	BuildNavigation(layout, map);

	script = new Script(Story::scriptFactory, mod->script.first);
	for (auto s : mod->script.second) {
		script->ReadMilestones(Config::GetScript(s));
	}
	script->SetValue("self.name", name);
}

void Building::PlacePivots(Quad* building) {
	mod->PlacePivots(building);

	for (auto& pivot : mod->pivots) {
		float x = pivot[0] * building->GetSizeX() + pivot[1];
		float y = pivot[2] * building->GetSizeY() + pivot[3];
		auto [wx, wy] = GetPosition(x, y);
		pivots.push_back(new Node("building", wx, wy));
	}
}

Layout* Building::ReadTemplates(const vector<string>& paths) {
	auto layout = new Layout();

	for (auto path : paths) {
		if (!filesystem::exists(path)) {
			delete layout;
			THROW_EXCEPTION(IOException, "Path does not exist: " + path + ".\n");
		}

		filesystem::path p(path);
		string filename = p.filename().string();
		string basename = filename.substr(0, filename.length() - 7);
		string extension = filename.substr(filename.length() - 6);
		if (extension != "layout") continue;

		JsonReader reader;
		JsonValue root;

		ifstream fin(path);
		if (!fin.is_open()) {
			delete layout;
			THROW_EXCEPTION(IOException, "Failed to open file: " + path + "\n");
		}

		if (reader.Parse(fin, root)) {
			layout->templateStairs[basename] = vector<vector<Stair>>(4);
			layout->templateElevators[basename] = vector<vector<Elevator>>(4);
			layout->templateRamps[basename] = vector<vector<Ramp>>(4);
			layout->templateCeilings[basename] = vector<vector<Ceiling>>(4);
			layout->templateGrounds[basename] = vector<vector<Ground>>(4);
			layout->templateCorridors[basename] = vector<vector<Corridor>>(4);
			layout->templateSingles[basename] = vector<vector<Single>>(4);
			layout->templateRows[basename] = vector<vector<Row>>(4);
			layout->templateNavigation[basename] = vector<NavigationTemplate>(4);
			layout->templateHatches[basename] = vector<vector<Hatch>>(4);

			for (auto s : root["stairs"]) {
				vector<float> rect(RECT_PARAM_COUNT);
				if (s["rect"].size() != RECT_PARAM_COUNT) {
					fin.close();
					delete layout;
					THROW_EXCEPTION(JsonFormatException, "Layout rect must have 8 floats.\n");
				}
				for (int i = 0; i < 8; i++) {
					rect[i] = s["rect"][i].AsFloat();
				}
				for (int i = 0; i < 4; i++) {
					Stair stair(InverseParams(rect, i));
					stair.SetDirection(static_cast<FACE_DIRECTION>(InverseDirection(s["direction"].AsInt(), i)));
					for (auto wall : s["walls"]) {
						stair.AddWall(InverseDirection(wall.AsInt(), i));
					}
					layout->templateStairs[basename][i].push_back(stair);
				}
			}

			for (auto e : root["elevators"]) {
				vector<float> rect(RECT_PARAM_COUNT);
				if (e["rect"].size() != RECT_PARAM_COUNT) {
					fin.close();
					delete layout;
					THROW_EXCEPTION(JsonFormatException, "Layout rect must have 8 floats.\n");
				}
				for (int i = 0; i < 8; i++) {
					rect[i] = e["rect"][i].AsFloat();
				}
				for (int i = 0; i < 4; i++) {
					Elevator elevator(InverseParams(rect, i));
					elevator.SetDirection(static_cast<FACE_DIRECTION>(InverseDirection(e["direction"].AsInt(), i)));
					for (auto wall : e["walls"]) {
						elevator.AddWall(InverseDirection(wall.AsInt(), i));
					}
					layout->templateElevators[basename][i].push_back(elevator);
				}
			}

			for (auto r : root["ramps"]) {
				vector<float> rect(RECT_PARAM_COUNT);
				if (r["rect"].size() != RECT_PARAM_COUNT) {
					fin.close();
					delete layout;
					THROW_EXCEPTION(JsonFormatException, "Layout rect must have 8 floats.\n");
				}
				for (int i = 0; i < 8; i++) {
					rect[i] = r["rect"][i].AsFloat();
				}
				for (int i = 0; i < 4; i++) {
					Ramp ramp(InverseParams(rect, i));
					ramp.SetDirection(static_cast<FACE_DIRECTION>(InverseDirection(r["direction"].AsInt(), i)));
					for (auto wall : r["walls"]) {
						ramp.AddWall(InverseDirection(wall.AsInt(), i));
					}
					layout->templateRamps[basename][i].push_back(ramp);
				}
			}

			for (auto c : root["ceilings"]) {
				vector<float> rect(RECT_PARAM_COUNT);
				if (c.size() != RECT_PARAM_COUNT) {
					fin.close();
					delete layout;
					THROW_EXCEPTION(JsonFormatException, "Layout rect must have 8 floats.\n");
				}
				for (int i = 0; i < 8; i++) {
					rect[i] = c[i].AsFloat();
				}
				for (int i = 0; i < 4; i++) {
					Ceiling ceiling(InverseParams(rect, i));
					layout->templateCeilings[basename][i].push_back(ceiling);
				}
			}

			for (auto g : root["grounds"]) {
				vector<float> rect(RECT_PARAM_COUNT);
				if (g.size() != RECT_PARAM_COUNT) {
					fin.close();
					delete layout;
					THROW_EXCEPTION(JsonFormatException, "Layout rect must have 8 floats.\n");
				}
				for (int i = 0; i < 8; i++) {
					rect[i] = g[i].AsFloat();
				}
				for (int i = 0; i < 4; i++) {
					Ground ground(InverseParams(rect, i));
					layout->templateGrounds[basename][i].push_back(ground);
				}
			}

			for (auto c : root["corridors"]) {
				vector<float> rect(RECT_PARAM_COUNT);
				if (c["rect"].size() != RECT_PARAM_COUNT) {
					fin.close();
					delete layout;
					THROW_EXCEPTION(JsonFormatException, "Layout rect must have 8 floats.\n");
				}
				for (int i = 0; i < 8; i++) {
					rect[i] = c["rect"][i].AsFloat();
				}
				for (int i = 0; i < 4; i++) {
					Corridor corridor(InverseParams(rect, i));
					for (auto wall : c["walls"]) {
						corridor.AddWall(InverseDirection(wall.AsInt(), i));
					}
					for (auto door : c["doors"]) {
						int origDir = door["direction"].AsInt();
						vector<vector<float>> positions;
						for (auto p : door["positions"]) {
							vector<float> pos;
							for (auto f : p) pos.push_back(f.AsFloat());
							positions.push_back(InverseWall(pos, origDir, i));
						}
						corridor.AddDoor(static_cast<FACE_DIRECTION>(InverseDirection(origDir, i)), positions);
					}
					for (auto window : c["windows"]) {
						int origDir = window["direction"].AsInt();
						vector<vector<float>> positions;
						for (auto p : window["positions"]) {
							vector<float> pos;
							for (auto f : p) pos.push_back(f.AsFloat());
							positions.push_back(InverseWall(pos, origDir, i));
						}
						corridor.AddWindow(static_cast<FACE_DIRECTION>(InverseDirection(origDir, i)), positions);
					}
					layout->templateCorridors[basename][i].push_back(corridor);
				}
			}

			for (auto s : root["singles"]) {
				vector<float> rect(RECT_PARAM_COUNT);
				if (s["rect"].size() != RECT_PARAM_COUNT) {
					fin.close();
					delete layout;
					THROW_EXCEPTION(JsonFormatException, "Layout rect must have 8 floats.\n");
				}
				for (int i = 0; i < 8; i++) {
					rect[i] = s["rect"][i].AsFloat();
				}
				for (int i = 0; i < 4; i++) {
					Single single(InverseParams(rect, i));
					single.SetDirection(static_cast<FACE_DIRECTION>(InverseDirection(s["direction"].AsInt(), i)));
					for (auto door : s["doors"]) {
						int origDir = door["direction"].AsInt();
						vector<vector<float>> positions;
						for (auto p : door["positions"]) {
							vector<float> pos;
							for (auto f : p) pos.push_back(f.AsFloat());
							positions.push_back(InverseWall(pos, origDir, i));
						}
						single.AddDoor(static_cast<FACE_DIRECTION>(InverseDirection(origDir, i)), positions);
					}
					for (auto window : s["windows"]) {
						int origDir = window["direction"].AsInt();
						vector<vector<float>> positions;
						for (auto p : window["positions"]) {
							vector<float> pos;
							for (auto f : p) pos.push_back(f.AsFloat());
							positions.push_back(InverseWall(pos, origDir, i));
						}
						single.AddWindow(static_cast<FACE_DIRECTION>(InverseDirection(origDir, i)), positions);
					}
					layout->templateSingles[basename][i].push_back(single);
				}
			}

			for (auto r : root["rows"]) {
				vector<float> rect(RECT_PARAM_COUNT);
				if (r["rect"].size() != RECT_PARAM_COUNT) {
					fin.close();
					delete layout;
					THROW_EXCEPTION(JsonFormatException, "Layout rect must have 8 floats.\n");
				}
				for (int i = 0; i < 8; i++) {
					rect[i] = r["rect"][i].AsFloat();
				}
				for (int i = 0; i < 4; i++) {
					Row row(InverseParams(rect, i));
					row.SetDirection(static_cast<FACE_DIRECTION>(InverseDirection(r["direction"].AsInt(), i)));
					for (auto door : r["doors"]) {
						int origDir = door["direction"].AsInt();
						vector<vector<float>> positions;
						for (auto p : door["positions"]) {
							vector<float> pos;
							for (auto f : p) pos.push_back(f.AsFloat());
							positions.push_back(InverseWall(pos, origDir, i));
						}
						row.AddDoor(static_cast<FACE_DIRECTION>(InverseDirection(origDir, i)), positions);
					}
					for (auto window : r["windows"]) {
						int origDir = window["direction"].AsInt();
						vector<vector<float>> positions;
						for (auto p : window["positions"]) {
							vector<float> pos;
							for (auto f : p) pos.push_back(f.AsFloat());
							positions.push_back(InverseWall(pos, origDir, i));
						}
						row.AddWindow(static_cast<FACE_DIRECTION>(InverseDirection(origDir, i)), positions);
					}
					layout->templateRows[basename][i].push_back(row);
				}
			}

			for (auto h : root["hatches"]) {
				vector<float> rect(RECT_PARAM_COUNT);
				if (h.size() != RECT_PARAM_COUNT) {
					fin.close();
					delete layout;
					THROW_EXCEPTION(JsonFormatException, "Layout rect must have 8 floats.\n");
				}
				for (int i = 0; i < 8; i++) {
					rect[i] = h[i].AsFloat();
				}
				for (int i = 0; i < 4; i++) {
					Hatch hatch(InverseParams(rect, i));
					layout->templateHatches[basename][i].push_back(hatch);
				}
			}

			if (root.ValidMember("navigation")) {
				auto& navRoot = root["navigation"];

				vector<vector<float>> rawNodes;
				for (auto n : navRoot["nodes"]) {
					if (n["position"].size() != POINT_PARAM_COUNT) {
						fin.close();
						delete layout;
						THROW_EXCEPTION(JsonFormatException, "Navigation node position must have 4 floats.\n");
					}
					vector<float> position(POINT_PARAM_COUNT);
					for (int i = 0; i < 4; i++) {
						position[i] = n["position"][i].AsFloat();
					}
					rawNodes.push_back(position);
				}

				vector<pair<vector<float>, vector<float>>> rawLines;
				for (auto l : navRoot["lines"]) {
					if (l["begin"].size() != POINT_PARAM_COUNT || l["end"].size() != POINT_PARAM_COUNT) {
						fin.close();
						delete layout;
						THROW_EXCEPTION(JsonFormatException, "Navigation line begin/end must have 4 floats.\n");
					}
					vector<float> begin(POINT_PARAM_COUNT), end(POINT_PARAM_COUNT);
					for (int i = 0; i < 4; i++) {
						begin[i] = l["begin"][i].AsFloat();
						end[i] = l["end"][i].AsFloat();
					}
					rawLines.emplace_back(begin, end);
				}

				auto readEndpoint = [](const JsonValue& e) -> NavigationEndpointTemplate {
					NavigationEndpointTemplate endpoint;
					endpoint.type = e["type"].AsString();
					if (e.ValidMember("idx")) endpoint.idx = e["idx"].AsInt();
					if (e.ValidMember("vertex")) endpoint.vertex = e["vertex"].AsInt();
					return endpoint;
				};

				vector<NavigationConnectionTemplate> rawConnections;
				for (auto c : navRoot["connections"]) {
					NavigationConnectionTemplate connTemplate;
					connTemplate.begin = readEndpoint(c["begin"]);
					connTemplate.end = readEndpoint(c["end"]);
					rawConnections.push_back(connTemplate);
				}

				for (int i = 0; i < 4; i++) {
					NavigationTemplate navTemplate;
					for (auto& position : rawNodes) {
						NavigationNodeTemplate nodeTemplate;
						nodeTemplate.position = InversePoint(position, i);
						navTemplate.nodes.push_back(nodeTemplate);
					}
					for (auto& [begin, end] : rawLines) {
						NavigationLineTemplate lineTemplate;
						lineTemplate.begin = InversePoint(begin, i);
						lineTemplate.end = InversePoint(end, i);
						navTemplate.lines.push_back(lineTemplate);
					}
					navTemplate.connections = rawConnections;
					layout->templateNavigation[basename][i] = navTemplate;
				}
			}
		}
		else {
			fin.close();
			delete layout;
			THROW_EXCEPTION(JsonFormatException, "Json syntax error: " + reader.GetErrorMessages() + ".\n");
		}
		fin.close();
	}

	return layout;
}

void Building::AddCabin(Cabin* cabin) {
	if (!cabin) {
		THROW_EXCEPTION(NullPointerException, "Cabin is null when adding to building.\n");
	}
	cabins.push_back(cabin);
}

void Building::ReadFloor(int level, int face, const string& name, Layout* layout) {
	if (!layout) {
		THROW_EXCEPTION(InvalidArgumentException, "Layout pointer is null.\n");
	}
	float width = construction.GetSizeX();
	float height = construction.GetSizeY();
	auto floor = new Floor(level, width, height);

	int idx = basements + level;
	if (idx < 0 || idx >= floors.size()) {
		THROW_EXCEPTION(OutOfRangeException, "Floor index out of range in ReadFloor.\n");
	}

	for (auto& stair : layout->templateStairs[name][face]) {
		stair.InstanciateQuad(width, height);
		floor->AddStair(stair);
	}

	for (auto& elevator : layout->templateElevators[name][face]) {
		elevator.InstanciateQuad(width, height);
		floor->AddElevator(elevator);
	}

	for (auto& ramp : layout->templateRamps[name][face]) {
		ramp.InstanciateQuad(width, height);
		floor->AddRamp(ramp);
	}

	for (auto& ceiling : layout->templateCeilings[name][face]) {
		ceiling.InstanciateQuad(width, height);
		floor->AddCeiling(ceiling);
	}

	for (auto& ground : layout->templateGrounds[name][face]) {
		ground.InstanciateQuad(width, height);
		floor->AddGround(ground);
	}

	for (auto& corridor : layout->templateCorridors[name][face]) {
		corridor.InstanciateQuad(width, height);
		floor->AddCorridor(corridor);
	}

	for (auto& single : layout->templateSingles[name][face]) {
		single.InstanciateQuad(width, height);
		floor->AddSingle(single);
	}

	for (auto& row : layout->templateRows[name][face]) {
		row.InstanciateQuad(width, height);
		floor->AddRow(row);
	}

	for (auto& hatch : layout->templateHatches[name][face]) {
		hatch.InstanciateQuad(width, height);
		floor->AddHatch(hatch);
	}

	floors[idx] = floor;
}

void Building::ReadFloors(int face, const string& name, Layout* layout) {
	if (!layout) {
		THROW_EXCEPTION(InvalidArgumentException, "Layout pointer is null.\n");
	}
	for (int i = 0; i < basements + layers; i++) {
		ReadFloor(i - basements, face, name, layout);
	}
}

void Building::ReadFloors(int face, const vector<string>& names, Layout* layout) {
	if (!layout) {
		THROW_EXCEPTION(InvalidArgumentException, "Layout pointer is null.\n");
	}
	if (names.size() != basements + layers) {
		THROW_EXCEPTION(InvalidArgumentException, "Template number and building layers mismatch.\n");
	}
	for (int i = 0; i < basements + layers; i++) {
		ReadFloor(i - basements, face, names[i], layout);
	}
}

void Building::AssignRoom(int level, int slot, const string& name,
	Component* component, RoomFactory* factory) {
	if (!component || !factory) {
		THROW_EXCEPTION(InvalidArgumentException, "Component or factory is null.\n");
	}
	int idx = basements + level;
	if (idx < 0 || idx >= floors.size()) {
		THROW_EXCEPTION(OutOfRangeException, "Floor index out of range when assigning room.\n");
	}
	auto& singles = floors[idx]->GetSingles();
	if (slot < 0 || slot >= singles.size()) {
		THROW_EXCEPTION(OutOfRangeException, "Single slot out of range.\n");
	}
	Room* room = new Room(factory, name);
	if (!room) {
		THROW_EXCEPTION(NullPointerException, "Failed to create room " + name + ".\n");
	}
	room->SetLayer(level);

	auto& single = singles[slot];
	room->SetPosition(
		single.GetPosX(), single.GetPosY(),
		single.GetSizeX(), single.GetSizeY());
	room->SetDirection(single.GetDirection());
	room->SetDoors(single.GetDoors());
	room->SetWindows(single.GetWindows());
	room->SetNumber(idx, floors[idx]->AssignNumber());
	component->AddRoom(room);

	rooms.push_back(room);
	singleRoomBySlot[level][slot] = room;
}

void Building::ArrangeRow(int level, int slot, const string& name, float acreage,
	Component* component, RoomFactory* factory) {
	if (!component || !factory) {
		THROW_EXCEPTION(InvalidArgumentException, "Component or factory is null.\n");
	}
	int idx = basements + level;
	if (idx < 0 || idx >= floors.size()) {
		THROW_EXCEPTION(OutOfRangeException, "Floor index out of range when assigning row.\n");
	}
	auto& rows = floors[idx]->GetRows();
	if (slot < 0 || slot >= rows.size()) {
		THROW_EXCEPTION(OutOfRangeException, "Failed to create room " + name + ".\n");
	}
	auto& row = rows[slot];

	float num = row.GetAcreage() / acreage;
	if (num < 1 || num - static_cast<int>(num) >= 0.5f)
		num = num + 1;

	if (row.GetDirection() == 0 || row.GetDirection() == 1) {
		float div = row.GetSizeY() / static_cast<int>(num);
		for (int i = 0; i < static_cast<int>(num); i++) {
			Room* room = new Room(factory, name);
			if (!room) {
				THROW_EXCEPTION(NullPointerException, "Failed to create room.\n");
			}
			room->SetLayer(level);

			room->SetVertices(
				row.GetLeft(), row.GetBottom() + div * i,
				row.GetRight(), row.GetBottom() + div * (i + 1));
			room->SetDirection(row.GetDirection());
			room->SetDoors(rows[slot].GetDoors());
			room->SetWindows(rows[slot].GetWindows());
			room->SetNumber(idx, floors[idx]->AssignNumber());
			component->AddRoom(room);

			rooms.push_back(room);
			rowRoomBySlot[level][slot].push_back(room);
		}
	}
	else {
		float div = row.GetSizeX() / static_cast<int>(num);
		for (int i = 0; i < static_cast<int>(num); i++) {
			Room* room = new Room(factory, name);
			if (!room) {
				THROW_EXCEPTION(NullPointerException, "Failed to create room.\n");
			}
			room->SetLayer(level);

			room->SetVertices(
				row.GetLeft() + div * i, row.GetBottom(),
				row.GetLeft() + div * (i + 1), row.GetTop());
			room->SetDirection(row.GetDirection());
			room->SetDoors(rows[slot].GetDoors());
			room->SetWindows(rows[slot].GetWindows());
			room->SetNumber(idx, floors[idx]->AssignNumber());
			component->AddRoom(room);

			rooms.push_back(room);
			rowRoomBySlot[level][slot].push_back(room);
		}
	}
}

vector<float> Building::InverseParams(const vector<float>& params, int face) {
	if (face < 0 || face >= 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Facing direction out of range [0,3].\n");
	}
	if (params.size() != RECT_PARAM_COUNT) {
		THROW_EXCEPTION(InvalidArgumentException, "Params must have 8 elements.\n");
	}
	auto inversed = params;
	switch (face) {
	case 0:
		inversed[0] = 1.f - params[6];
		inversed[1] = -params[7];
		inversed[2] = params[0];
		inversed[3] = params[1];
		inversed[4] = 1.f - params[2];
		inversed[5] = -params[3];
		inversed[6] = params[4];
		inversed[7] = params[5];
		break;
	case 1:
		inversed[0] = params[2];
		inversed[1] = params[3];
		inversed[2] = 1.f - params[4];
		inversed[3] = -params[5];
		inversed[4] = params[6];
		inversed[5] = params[7];
		inversed[6] = 1.f - params[0];
		inversed[7] = -params[1];
		break;
	case 2:
		// 不变
		break;
	case 3:
		inversed[0] = 1.f - params[4];
		inversed[1] = -params[5];
		inversed[2] = 1.f - params[6];
		inversed[3] = -params[7];
		inversed[4] = 1.f - params[0];
		inversed[5] = -params[1];
		inversed[6] = 1.f - params[2];
		inversed[7] = -params[3];
		break;
	default:
		break;
	}
	return inversed;
}

int Building::InverseDirection(int direction, int face) {
	if (face < 0 || face >= 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Facing direction out of range [0,3].\n");
	}
	switch (face) {
	case 0:
		if (direction >= 2) return 3 - direction;
		else return direction + 2;
	case 1:
		if (direction >= 2) return direction - 2;
		else return 3 - direction;
	case 2:
		return direction;
	case 3:
		return (5 - direction) % 4;
	default:
		return direction;
	}
}

vector<float> Building::InversePoint(const vector<float>& point, int face) {
	if (face < 0 || face >= 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Facing direction out of range [0,3].\n");
	}
	if (point.size() != POINT_PARAM_COUNT) {
		THROW_EXCEPTION(InvalidArgumentException, "Point must have 4 elements.\n");
	}
	switch (face) {
	case 0:
		return { 1.f - point[2], -point[3], point[0], point[1] };
	case 1:
		return { point[2], point[3], 1.f - point[0], -point[1] };
	case 2:
		return point;
	case 3:
		return { 1.f - point[0], -point[1], 1.f - point[2], -point[3] };
	default:
		return point;
	}
}


vector<float> Building::InverseWall(const vector<float>& pos, int direction, int face) {
	if (pos.size() != RECT_PARAM_COUNT) {
		THROW_EXCEPTION(InvalidArgumentException, "Wall position must have 8 elements.\n");
	}
	// Determine whether this rotation reverses the along-wall direction.
	// face=0 (90° CCW + x-flip): reverses vertical walls (WEST=0, EAST=1)
	// face=1 (90° CW + x-flip): reverses horizontal walls (NORTH=2, SOUTH=3)
	// face=2 (identity): no reversal
	// face=3 (180°): reverses all walls
	bool flip = false;
	switch (face) {
	case 0: flip = (direction == 0 || direction == 1); break;
	case 1: flip = (direction == 2 || direction == 3); break;
	case 3: flip = true; break;
	}
	if (!flip) return pos;
	// Reflect the along-wall (x) components: swap start x ↔ end x and apply 1-ratio.
	// The vertical (y) components describe opening bottom/top and are wall-direction-
	// independent, so they stay in place (y1 with start slot, y2 with end slot).
	return {
		1.f - pos[4], -pos[5], pos[2], pos[3],
		1.f - pos[0], -pos[1], pos[6], pos[7],
	};
}

// line的运行期状态：固定端点缓存 + 沿line收集到的锚点（用于排序后串成走廊骨架）
struct LineRuntime {
	// 起点世界坐标
	float beginX = 0.f, beginY = 0.f;

	// 终点世界坐标
	float endX = 0.f, endY = 0.f;

	// 起点处的导航节点（按需创建）
	Node* nodeAt0 = nullptr;

	// 终点处的导航节点（按需创建）
	Node* nodeAt1 = nullptr;

	// 沿line收集到的锚点（参数t, 节点）
	vector<pair<float, Node*>> anchors;
};

void Building::BuildNavigation(Layout* layout, Map* map) {
	if (!layout || !map) {
		THROW_EXCEPTION(InvalidArgumentException, "Layout or map pointer is null.\n");
	}

	vector<Node*> newNodes;
	vector<Connection*> newConnections;

	// 楼层间楼梯端点登记，留待所有楼层处理完后按相邻层贪心匹配
	vector<tuple<int, Node*, bool>> stairEndpoints;

	for (int idx = 0; idx < static_cast<int>(floors.size()); idx++) {
		int level = idx - basements;
		Floor* floor = floors[idx];
		if (!floor) continue;

		auto [templateName, face] = mod->templates[idx];
		auto navIt = layout->templateNavigation.find(templateName);
		if (navIt == layout->templateNavigation.end()) continue;
		const NavigationTemplate& navTemplate = navIt->second[face];
		if (navTemplate.nodes.empty() && navTemplate.lines.empty() && navTemplate.connections.empty()) continue;

		float floorWidth = floor->GetSizeX();
		float floorHeight = floor->GetSizeY();
		float z = height * level;

		auto instantiate = [this, floorWidth, floorHeight, z](const vector<float>& position) -> Node* {
			float lx = position[0] * floorWidth + position[1];
			float ly = position[2] * floorHeight + position[3];
			auto [wx, wy] = GetPosition(lx, ly);
			return new Node("building", wx, wy, z);
		};

		vector<Node*> floorFixedNodes;
		for (auto& nodeTemplate : navTemplate.nodes) {
			Node* node = instantiate(nodeTemplate.position);
			nodes.push_back(node);
			newNodes.push_back(node);
			floorFixedNodes.push_back(node);
		}

		vector<LineRuntime> lineRuntimes(navTemplate.lines.size());
		for (int i = 0; i < static_cast<int>(navTemplate.lines.size()); i++) {
			auto& lineTemplate = navTemplate.lines[i];
			lineRuntimes[i].beginX = lineTemplate.begin[0] * floorWidth + lineTemplate.begin[1];
			lineRuntimes[i].beginY = lineTemplate.begin[2] * floorHeight + lineTemplate.begin[3];
			lineRuntimes[i].endX = lineTemplate.end[0] * floorWidth + lineTemplate.end[1];
			lineRuntimes[i].endY = lineTemplate.end[2] * floorHeight + lineTemplate.end[3];
		}

		// 解析一个普通端点（line的vertex=-1除外，那种情况由调用方单独处理）为一组导航节点
		auto resolveEndpoint = [this, level, &floorFixedNodes, &lineRuntimes, &newNodes]
			(const NavigationEndpointTemplate& endpoint) -> vector<Node*> {
			if (endpoint.type == "node") {
				if (endpoint.idx < 0 || endpoint.idx >= static_cast<int>(floorFixedNodes.size())) {
					THROW_EXCEPTION(OutOfRangeException, "Navigation node index out of range.\n");
				}
				return { floorFixedNodes[endpoint.idx] };
			}
			else if (endpoint.type == "single") {
				auto levelIt = singleRoomBySlot.find(level);
				if (levelIt == singleRoomBySlot.end() || levelIt->second.find(endpoint.idx) == levelIt->second.end()) {
					THROW_EXCEPTION(OutOfRangeException, "Navigation single slot not found.\n");
				}
				return { levelIt->second.at(endpoint.idx)->GetNavigationNode() };
			}
			else if (endpoint.type == "row") {
				auto levelIt = rowRoomBySlot.find(level);
				if (levelIt == rowRoomBySlot.end() || levelIt->second.find(endpoint.idx) == levelIt->second.end()) {
					THROW_EXCEPTION(OutOfRangeException, "Navigation row slot not found.\n");
				}
				vector<Node*> result;
				for (auto room : levelIt->second.at(endpoint.idx)) {
					result.push_back(room->GetNavigationNode());
				}
				return result;
			}
			else if (endpoint.type == "line") {
				if (endpoint.idx < 0 || endpoint.idx >= static_cast<int>(lineRuntimes.size())) {
					THROW_EXCEPTION(OutOfRangeException, "Navigation line index out of range.\n");
				}
				auto& line = lineRuntimes[endpoint.idx];
				if (endpoint.vertex == 0) {
					if (!line.nodeAt0) {
						auto [wx, wy] = GetPosition(line.beginX, line.beginY);
						line.nodeAt0 = new Node("building", wx, wy, height * level);
						nodes.push_back(line.nodeAt0);
						newNodes.push_back(line.nodeAt0);
						line.anchors.emplace_back(0.f, line.nodeAt0);
					}
					return { line.nodeAt0 };
				}
				else if (endpoint.vertex == 1) {
					if (!line.nodeAt1) {
						auto [wx, wy] = GetPosition(line.endX, line.endY);
						line.nodeAt1 = new Node("building", wx, wy, height * level);
						nodes.push_back(line.nodeAt1);
						newNodes.push_back(line.nodeAt1);
						line.anchors.emplace_back(1.f, line.nodeAt1);
					}
					return { line.nodeAt1 };
				}
				THROW_EXCEPTION(InvalidArgumentException, "Line vertex -1 must be resolved against a single/row endpoint.\n");
			}
			THROW_EXCEPTION(InvalidArgumentException, "Endpoint type '" + endpoint.type + "' cannot be resolved here.\n");
		};

		for (auto& connTemplate : navTemplate.connections) {
			bool beginIsProjection = connTemplate.begin.type == "line" && connTemplate.begin.vertex == -1;
			bool endIsProjection = connTemplate.end.type == "line" && connTemplate.end.vertex == -1;

			// line的vertex=-1端：将房间中心动态投影到line上生成锚点并与房间连接
			if (beginIsProjection || endIsProjection) {
				const NavigationEndpointTemplate& lineSide = beginIsProjection ? connTemplate.begin : connTemplate.end;
				const NavigationEndpointTemplate& roomSide = beginIsProjection ? connTemplate.end : connTemplate.begin;
				if (roomSide.type != "single" && roomSide.type != "row") {
					THROW_EXCEPTION(JsonFormatException, "Line vertex -1 can only connect to a single or row endpoint.\n");
				}
				if (lineSide.idx < 0 || lineSide.idx >= static_cast<int>(lineRuntimes.size())) {
					THROW_EXCEPTION(OutOfRangeException, "Navigation line index out of range.\n");
				}

				vector<Room*> rooms;
				if (roomSide.type == "single") {
					auto levelIt = singleRoomBySlot.find(level);
					if (levelIt == singleRoomBySlot.end() || levelIt->second.find(roomSide.idx) == levelIt->second.end()) {
						THROW_EXCEPTION(OutOfRangeException, "Navigation single slot not found.\n");
					}
					rooms.push_back(levelIt->second.at(roomSide.idx));
				}
				else {
					auto levelIt = rowRoomBySlot.find(level);
					if (levelIt == rowRoomBySlot.end() || levelIt->second.find(roomSide.idx) == levelIt->second.end()) {
						THROW_EXCEPTION(OutOfRangeException, "Navigation row slot not found.\n");
					}
					rooms = levelIt->second.at(roomSide.idx);
				}

				auto& line = lineRuntimes[lineSide.idx];
				for (auto room : rooms) {
					float t = ProjectOntoLine(room->GetPosX(), room->GetPosY(),
						line.beginX, line.beginY, line.endX, line.endY);
					float px = line.beginX + t * (line.endX - line.beginX);
					float py = line.beginY + t * (line.endY - line.beginY);
					auto [wx, wy] = GetPosition(px, py);
					Node* projected = new Node("building", wx, wy, height * level);
					nodes.push_back(projected);
					newNodes.push_back(projected);
					line.anchors.emplace_back(t, projected);

					newConnections.push_back(new Connection(*projected, *room->GetNavigationNode()));
				}
				continue;
			}

			// outside端：连接到建筑边界四角中离对端最近的两个角点，各构建一条Connection
			if (connTemplate.begin.type == "outside" || connTemplate.end.type == "outside") {
				const NavigationEndpointTemplate& otherSide = connTemplate.begin.type == "outside" ? connTemplate.end : connTemplate.begin;
				vector<Node*> otherNodes = resolveEndpoint(otherSide);
				if (otherNodes.size() != 1) {
					THROW_EXCEPTION(JsonFormatException, "Outside endpoint must pair with a single navigation node.\n");
				}
				Node* otherNode = otherNodes[0];

				vector<pair<float, Node*>> candidates;
				for (int i = 0; i < 4; i++) {
					Node* corner = boundary.corners[i];
					if (!corner) continue;
					float dx = corner->GetX() - otherNode->GetX();
					float dy = corner->GetY() - otherNode->GetY();
					candidates.emplace_back(dx * dx + dy * dy, corner);
				}
				if (candidates.empty()) {
					THROW_EXCEPTION(NullPointerException, "Building boundary has no valid corner for outside connection.\n");
				}
				sort(candidates.begin(), candidates.end(),
					[](const pair<float, Node*>& a, const pair<float, Node*>& b) { return a.first < b.first; });

				int connectCount = min(static_cast<int>(candidates.size()), 2);
				for (int i = 0; i < connectCount; i++) {
					newConnections.push_back(new Connection(*candidates[i].second, *otherNode));
				}
				continue;
			}

			// upstair/downstair端：先登记，留待所有楼层处理完后按相邻层贪心匹配
			if (connTemplate.begin.type == "upstair" || connTemplate.begin.type == "downstair" ||
				connTemplate.end.type == "upstair" || connTemplate.end.type == "downstair") {
				bool beginIsStair = connTemplate.begin.type == "upstair" || connTemplate.begin.type == "downstair";
				const NavigationEndpointTemplate& stairSide = beginIsStair ? connTemplate.begin : connTemplate.end;
				const NavigationEndpointTemplate& otherSide = beginIsStair ? connTemplate.end : connTemplate.begin;
				vector<Node*> otherNodes = resolveEndpoint(otherSide);
				if (otherNodes.size() != 1) {
					THROW_EXCEPTION(JsonFormatException, "Stair endpoint must pair with a single navigation node.\n");
				}
				stairEndpoints.emplace_back(level, otherNodes[0], stairSide.type == "upstair");
				continue;
			}

			vector<Node*> beginNodes = resolveEndpoint(connTemplate.begin);
			vector<Node*> endNodes = resolveEndpoint(connTemplate.end);
			for (auto beginNode : beginNodes) {
				for (auto endNode : endNodes) {
					newConnections.push_back(new Connection(*beginNode, *endNode));
				}
			}
		}

		for (auto& line : lineRuntimes) {
			if (line.anchors.size() < 2) continue;
			sort(line.anchors.begin(), line.anchors.end(),
				[](const pair<float, Node*>& a, const pair<float, Node*>& b) { return a.first < b.first; });
			for (int i = 0; i + 1 < static_cast<int>(line.anchors.size()); i++) {
				newConnections.push_back(new Connection(*line.anchors[i].second, *line.anchors[i + 1].second));
			}
		}
	}

	// 相邻楼层之间按世界坐标贪心最近匹配upstair/downstair
	unordered_map<int, vector<pair<Node*, int>>> upstairsByLevel;
	unordered_map<int, vector<pair<Node*, int>>> downstairsByLevel;
	for (int i = 0; i < static_cast<int>(stairEndpoints.size()); i++) {
		auto& [level, node, isUp] = stairEndpoints[i];
		if (isUp) upstairsByLevel[level].emplace_back(node, i);
		else downstairsByLevel[level].emplace_back(node, i);
	}

	for (auto& [level, upList] : upstairsByLevel) {
		auto downIt = downstairsByLevel.find(level + 1);
		if (downIt == downstairsByLevel.end()) {
			debugf("Warning: building has upstair on level %d with no matching downstair on level %d.\n", level, level + 1);
			continue;
		}
		auto& downList = downIt->second;

		vector<tuple<float, int, int>> candidates;
		for (int i = 0; i < static_cast<int>(upList.size()); i++) {
			for (int j = 0; j < static_cast<int>(downList.size()); j++) {
				float dx = upList[i].first->GetX() - downList[j].first->GetX();
				float dy = upList[i].first->GetY() - downList[j].first->GetY();
				candidates.emplace_back(dx * dx + dy * dy, i, j);
			}
		}
		sort(candidates.begin(), candidates.end(),
			[](const tuple<float, int, int>& a, const tuple<float, int, int>& b) { return get<0>(a) < get<0>(b); });

		vector<bool> upUsed(upList.size(), false), downUsed(downList.size(), false);
		for (auto& [distSq, i, j] : candidates) {
			if (upUsed[i] || downUsed[j]) continue;
			upUsed[i] = true;
			downUsed[j] = true;
			newConnections.push_back(new Connection(*upList[i].first, *downList[j].first));
		}
	}

	// Room自己的中心点导航节点由Room持有，不在building->nodes里，
	// 但仍需要登记进Map::navigationNodes，否则AutoNavigation按id查不到这些节点
	for (auto room : rooms) {
		if (room && room->GetNavigationNode()) {
			newNodes.push_back(room->GetNavigationNode());
		}
	}

	map->ApplyDivideResult(newNodes, newConnections, {});
}

int EmptyBuilding::count = 0;

EmptyBuilding::EmptyBuilding() : id(count++) {

}

EmptyBuilding::~EmptyBuilding() {

}

const char* EmptyBuilding::GetId() {
	return "empty";
}

const char* EmptyBuilding::GetType() const {
	return "empty";
}

const char* EmptyBuilding::GetName() {
	name = "空建筑" + to_string(id);
	return name.data();
}

vector<float> EmptyBuilding::GetPowers() {
	return vector<float>(AREA_END, 0.f);
}

function<int(const Lot*, int, int)> EmptyBuilding::BuildingAssigner = [](const Lot*, int, int) {
	return 0;
};

float EmptyBuilding::RandomAcreage() {
	return 0.f;
}

void EmptyBuilding::LayoutBuilding(const Quad* quad) {

}

void EmptyBuilding::PlaceConstruction() {

}

void EmptyBuilding::PlacePivots(Quad* building) {

}

float Building::ProjectOntoLine(float px, float py, float ax, float ay, float bx, float by) {
	float dx = bx - ax, dy = by - ay;
	float lenSq = dx * dx + dy * dy;
	if (lenSq <= 0.f) return 0.f;
	float t = ((px - ax) * dx + (py - ay) * dy) / lenSq;
	if (t < 0.f) t = 0.f;
	if (t > 1.f) t = 1.f;
	return t;
}
