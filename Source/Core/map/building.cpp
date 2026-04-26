#include "building.h"

#include "map/map.h"
#include "map/block.h"
#include "map/zone.h"
#include "map/component.h"
#include "map/room.h"
#include "populace/person.h"
#include "story/story.h"
#include "story/script.h"

#include "json.h"

#include <fstream>
#include <filesystem>


using namespace std;

Stair::Stair(vector<float> params) :
	direction(FACE_WEST),
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

void Stair::InstanciateQuad(float width, float height) {
	if (params.size() != 8) {
		THROW_EXCEPTION(InvalidArgumentException, "Stair rect must have 8 elements.\n");
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
	if (params.size() != 8) {
		THROW_EXCEPTION(InvalidArgumentException, "Ceiling rect must have 8 elements.\n");
	}

	float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
	float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
	float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
	float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
	Quad::SetPosition(x, y, w, h);
}

Corridor::Corridor(vector<float> params) :
	walls(4, false), doors(), windows(), params(params) {

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
	if (params.size() != 8) {
		THROW_EXCEPTION(InvalidArgumentException, "Corridor rect must have 8 elements.\n");
	}

	float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
	float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
	float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
	float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
	Quad::SetPosition(x, y, w, h);
}

Single::Single(vector<float> params) :
	doors(), windows(), params(params) {

}

Single::~Single() {

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
	if (params.size() != 8) {
		THROW_EXCEPTION(InvalidArgumentException, "Single rect must have 8 elements.\n");
	}

	float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
	float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
	float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
	float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
	Quad::SetPosition(x, y, w, h);
}

Row::Row(vector<float> params) :
	direction(FACE_WEST), doors(), windows(), params(params) {

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
	if (params.size() != 8) {
		THROW_EXCEPTION(InvalidArgumentException, "Row rect must have 8 elements.\n");
	}

	float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
	float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
	float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
	float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
	Quad::SetPosition(x, y, w, h);
}

Floor::Floor(int level, float width, float height)
	: level(level), number(0) {
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

vector<Ceiling>& Floor::GetCeilings() {
	return ceilings;
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

void Floor::AddCeiling(Ceiling ceiling) {
	ceilings.push_back(ceiling);
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

int Floor::AssignNumber() {
	return number++;
}

Building::Building(BuildingFactory* factory, const string& building) :
	mod(factory->CreateBuilding(building)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	parentZone(nullptr),
	parentBlock(nullptr),
	layers(1),
	basements(0),
	height(0.4f),
	construction(),
	wallTexture(),
	script(nullptr) {

}

Building::~Building() {
	factory->DestroyBuilding(mod);
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

	if(script)delete script;
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

std::string Building::GetWallTexture() const {
	return wallTexture;
}

string Building::GetAddress() {
	if (address != "") {
		return address.data();
	}

	auto zone = GetParentZone();
	if(zone) {
		auto zoneAddress = GetParentZone()->GetAddress();
		address = zoneAddress + " " + GetName();
		return address;
	}
	else {
		auto blockAddress = GetParentBlock()->GetAddress();
		address = blockAddress + " " + GetName();
		return address;
	}

}

vector<Component*>& Building::GetComponents() {
	return components;
}

vector<Room*>& Building::GetRooms() {
	return rooms;
}

Floor* Building::GetFloor(int level) const {
	int idx = basements + level;
	if (idx >= 0 && idx < floors.size())
		return floors[idx];
	return nullptr;
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

void Building::GetPosition(float& x, float& y) const {
	auto block = GetParentBlock();
	if(!block) {
		THROW_EXCEPTION(NullPointerException, "Building parent block is null.\n");
	}

	auto zone = GetParentZone();
	if(zone) {
		auto center = block->GetPosition(zone->GetPosX() - zone->GetSizeX() / 2.f + GetPosX(),
			zone->GetPosY() - zone->GetSizeY() / 2.f + GetPosY());
		x = center.first;
		y = center.second;
	}
	else{
		auto center = block->GetPosition(GetPosX(), GetPosY());
		x = center.first;
		y = center.second;
	}
}

void Building::LayoutBuilding(Layout* layout) {
	mod->LayoutBuilding(this);
	basements = mod->basements;
	layers = mod->layers;
	height = mod->height;
	wallTexture = mod->wallTexture;

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
			AssignRoom(get<0>(single), get<1>(single), get<2>(single), componentMap[component], Map::roomFactory);
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
	}

	script = new Script(Story::scriptFactory, mod->script.first);
	for (auto s : mod->script.second) {
		script->ReadMilestones(Config::GetScript(s));
	}
	script->SetValue("self.name", name);
}

Layout* Building::ReadTemplates(const vector<string>& paths) {
	auto layout = new Layout();

	for (auto path : paths) {
		if (!filesystem::exists(path)) {
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
			THROW_EXCEPTION(IOException, "Failed to open file: " + path + "\n");
		}

		if (reader.Parse(fin, root)) {
			layout->templateStairs[basename] = vector<vector<Stair>>(4);
			layout->templateCeilings[basename] = vector<vector<Ceiling>>(4);
			layout->templateCorridors[basename] = vector<vector<Corridor>>(4);
			layout->templateSingles[basename] = vector<vector<Single>>(4);
			layout->templateRows[basename] = vector<vector<Row>>(4);

			for (auto s : root["stairs"]) {
				vector<float> rect(8);
				if (s["rect"].size() != 8) {
					THROW_EXCEPTION(JsonFormatException, "Layout rect must have 8 floats.\n");
				}
				for (int i = 0; i < 8; ++i) {
					rect[i] = s["rect"][i].AsFloat();
				}
				for (int i = 0; i < 4; ++i) {
					Stair stair(InverseParams(rect, i));
					stair.SetDirection((FACE_DIRECTION)InverseDirection(s["direction"].AsInt(), i));
					layout->templateStairs[basename][i].push_back(stair);
				}
			}

			for (auto c : root["ceilings"]) {
				vector<float> rect(8);
				if (c.size() != 8) {
					THROW_EXCEPTION(JsonFormatException, "Layout rect must have 8 floats.\n");
				}
				for (int i = 0; i < 8; ++i) {
					rect[i] = c[i].AsFloat();
				}
				for (int i = 0; i < 4; ++i) {
					Ceiling ceiling(InverseParams(rect, i));
					layout->templateCeilings[basename][i].push_back(ceiling);
				}
			}

			for (auto c : root["corridors"]) {
				vector<float> rect(8);
				if (c["rect"].size() != 8) {
					THROW_EXCEPTION(JsonFormatException, "Layout rect must have 8 floats.\n");
				}
				for (int i = 0; i < 8; ++i) {
					rect[i] = c["rect"][i].AsFloat();
				}
				for (int i = 0; i < 4; ++i) {
					Corridor corridor(InverseParams(rect, i));
					for (auto wall : c["walls"]) {
						corridor.AddWall(InverseDirection(wall.AsInt(), i));
					}
					for (auto door : c["doors"]) {
						vector<vector<float>> positions;
						for (auto p : door["positions"]) {
							positions.emplace_back();
							for (auto f : p) {
								positions.back().push_back(f.AsFloat());
							}
						}
						corridor.AddDoor((FACE_DIRECTION)InverseDirection(door["direction"].AsInt(), i), positions);
					}
					for (auto window : c["windows"]) {
						vector<vector<float>> positions;
						for (auto p : window["positions"]) {
							positions.emplace_back();
							for (auto f : p) {
								positions.back().push_back(f.AsFloat());
							}
						}
						corridor.AddWindow((FACE_DIRECTION)InverseDirection(window["direction"].AsInt(), i), positions);
					}
					layout->templateCorridors[basename][i].push_back(corridor);
				}
			}

			for (auto s : root["singles"]) {
				vector<float> rect(8);
				if (s["rect"].size() != 8) {
					THROW_EXCEPTION(JsonFormatException, "Layout rect must have 8 floats.\n");
				}
				for (int i = 0; i < 8; ++i) {
					rect[i] = s["rect"][i].AsFloat();
				}
				for (int i = 0; i < 4; ++i) {
					Single single(InverseParams(rect, i));
					for (auto door : s["doors"]) {
						vector<vector<float>> positions;
						for (auto p : door["positions"]) {
							positions.emplace_back();
							for (auto f : p) {
								positions.back().push_back(f.AsFloat());
							}
						}
						single.AddDoor((FACE_DIRECTION)InverseDirection(door["direction"].AsInt(), i), positions);
					}
					for (auto window : s["windows"]) {
						vector<vector<float>> positions;
						for (auto p : window["positions"]) {
							positions.emplace_back();
							for (auto f : p) {
								positions.back().push_back(f.AsFloat());
							}
						}
						single.AddWindow((FACE_DIRECTION)InverseDirection(window["direction"].AsInt(), i), positions);
					}
					layout->templateSingles[basename][i].push_back(single);
				}
			}

			for (auto r : root["rows"]) {
				vector<float> rect(8);
				if (r["rect"].size() != 8) {
					THROW_EXCEPTION(JsonFormatException, "Layout rect must have 8 floats.\n");
				}
				for (int i = 0; i < 8; ++i) {
					rect[i] = r["rect"][i].AsFloat();
				}
				for (int i = 0; i < 4; ++i) {
					Row row(InverseParams(rect, i));
					row.SetDirection((FACE_DIRECTION)InverseDirection(r["direction"].AsInt(), i));
					for (auto door : r["doors"]) {
						vector<vector<float>> positions;
						for (auto p : door["positions"]) {
							positions.emplace_back();
							for (auto f : p) {
								positions.back().push_back(f.AsFloat());
							}
						}
						row.AddDoor((FACE_DIRECTION)InverseDirection(door["direction"].AsInt(), i), positions);
					}
					for (auto window : r["windows"]) {
						vector<vector<float>> positions;
						for (auto p : window["positions"]) {
							positions.emplace_back();
							for (auto f : p) {
								positions.back().push_back(f.AsFloat());
							}
						}
						row.AddWindow((FACE_DIRECTION)InverseDirection(window["direction"].AsInt(), i), positions);
					}
					layout->templateRows[basename][i].push_back(row);
				}
			}
		}
		else {
			fin.close();
			THROW_EXCEPTION(JsonFormatException, "Json syntax error: " + reader.GetErrorMessages() + ".\n");
		}
		fin.close();
	}

	return layout;
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

	for (auto& ceiling : layout->templateCeilings[name][face]) {
		ceiling.InstanciateQuad(width, height);
		floor->AddCeiling(ceiling);
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

	floors[idx] = floor;
}

void Building::ReadFloors(int face, const string& name, Layout* layout) {
	if (!layout) {
		THROW_EXCEPTION(InvalidArgumentException, "Layout pointer is null.\n");
	}
	for (int i = 0; i < basements + layers; ++i) {
		ReadFloor(i, face, name, layout);
	}
}

void Building::ReadFloors(int face, const vector<string>& names, Layout* layout) {
	if (!layout) {
		THROW_EXCEPTION(InvalidArgumentException, "Layout pointer is null.\n");
	}
	if (names.size() != basements + layers) {
		THROW_EXCEPTION(InvalidArgumentException, "Template number and building layers mismatch.\n");
	}
	for (int i = 0; i < basements + layers; ++i) {
		ReadFloor(i, face, names[i], layout);
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
	room->SetDoors(single.GetDoors());
	room->SetWindows(single.GetWindows());
	room->SetNumber(idx, floors[idx]->AssignNumber());
	component->AddRoom(room);

	rooms.push_back(room);
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
		THROW_EXCEPTION(NullPointerException, "Failed to create room " + name + ".\n");
	}
	auto& row = rows[slot];

	float num = row.GetAcreage() / acreage;
	if (num < 1 || num - (int)num >= 0.5f)
		num = num + 1;

	if (row.GetDirection() == 0 || row.GetDirection() == 1) {
		float div = row.GetSizeY() / (int)num;
		for (int i = 0; i < (int)num; ++i) {
			Room* room = new Room(factory, name);
			if (!room) {
				THROW_EXCEPTION(NullPointerException, "Failed to create room.\n");
			}
			room->SetLayer(level);

			room->SetVertices(
				row.GetLeft(), row.GetBottom() + div * i,
				row.GetRight(), row.GetBottom() + div * (i + 1));
			room->SetDoors(rows[slot].GetDoors());
			room->SetWindows(rows[slot].GetWindows());
			room->SetNumber(idx, floors[idx]->AssignNumber());
			component->AddRoom(room);

			rooms.push_back(room);
		}
	}
	else {
		float div = row.GetSizeX() / (int)num;
		for (int i = 0; i < (int)num; ++i) {
			Room* room = new Room(factory, name);
			if (!room) {
				THROW_EXCEPTION(NullPointerException, "Failed to create room.\n");
			}
			room->SetLayer(level);

			room->SetVertices(
				row.GetLeft() + div * i, row.GetBottom(),
				row.GetLeft() + div * (i + 1), row.GetTop());
			room->SetDoors(rows[slot].GetDoors());
			room->SetWindows(rows[slot].GetWindows());
			room->SetNumber(idx, floors[idx]->AssignNumber());
			component->AddRoom(room);

			rooms.push_back(room);
		}
	}
}

vector<float> Building::InverseParams(const vector<float>& params, int face) {
	if (face < 0 || face >= 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Facing direction out of range [0,3].\n");
	}
	if (params.size() != 8) {
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

