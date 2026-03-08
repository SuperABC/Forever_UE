#include "../common/error.h"
#include "../common/json.h"
#include "../common/utility.h"

#include "building_base.h"

#include <filesystem>
#include <fstream>
#include <string>


using namespace std;

// 楼梯

Stair::Stair(vector<float> params) :
	direction(FACE_WEST), params(move(params)) {

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

// 天花板

Ceiling::Ceiling(vector<float> params) :
	params(move(params)) {

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

// 走廊
Corridor::Corridor(vector<float> params) :
	walls(4, false), doors(), windows(), params(move(params)) {

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

// 独立房间

Single::Single(vector<float> params) :
	doors(), windows(), params(move(params)) {

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

// 联排房间

Row::Row(vector<float> params) :
	direction(FACE_WEST), doors(), windows(), params(move(params)) {

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

// 楼层

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

Building::Building() :
	parentZone(nullptr),
	parentPlot(nullptr),
	stated(false),
	owner(-1),
	layers(1),
	basements(0),
	height(0.4f),
	construction() {
	// 全成员默认构造

}

Building::~Building() {
	// room/component在map中统一创建和析构
	for (auto floor : floors) {
		if (floor) delete floor;
	}
}

void Building::SetParent(Plot* plot) {
	// 获取所在地块
	parentPlot = plot;
}

void Building::SetParent(Zone* zone) {
	// 设置所在地块
	parentZone = zone;
}

Plot* Building::GetParentPlot() const {
	// 获取所在园区
	return parentPlot;
}

Zone* Building::GetParentZone() const {
	// 设置所在园区
	return parentZone;
}

int Building::GetOwner() const {
	// 获取私人房东ID
	return owner;
}

void Building::SetOwner(int owner) {
	// 设置私人房东ID
	this->owner = owner;
}

bool Building::GetStated() const {
	// 获取是否由政府拥有
	return stated;
}

void Building::SetStated(bool stated) {
	// 设置是否由政府拥有
	this->stated = stated;
}

int Building::GetLayers() const {
	// 获取总地面层数
	return layers;
}

void Building::SetLayers(int layers) {
	// 设置总地面层数
	this->layers = layers;
}

int Building::GetBasements() const {
	// 获取总地下层数
	return basements;
}

void Building::SetBasements(int basements) {
	// 设置总地下层数
	this->basements = basements;
}

float Building::GetHeight() const {
	// 获取层高
	return height;
}

void Building::SetHeight(float height) {
	// 设置层高
	this->height = height;
}

const Quad Building::GetConstruction() const {
	// 获取建筑楼体范围
	return construction;
}

vector<Component*>& Building::GetComponents() {
	// 获取楼内全部组合
	return components;
}

vector<Room*>& Building::GetRooms() {
	// 获取楼内全部房间
	return rooms;
}

Floor* Building::GetFloor(int level) const {
	// 获取楼层
	int idx = basements + level;
	if (idx >= 0 && idx < floors.size())
		return floors[idx];
	return nullptr;
}

void Building::FinishInit() {
	// 完成初始化
	construction = LayoutConstruction();
}

pair<float, float> Building::GetPosition() const {
	// 获取世界位置
	auto zone = GetParentZone();
	if (zone) {
		auto plot = zone->GetParent();
		if (plot) {
			auto center = plot->GetPosition(
				zone->GetPosX() - zone->GetSizeX() / 2 + GetPosX() - GetSizeX() / 2.f + construction.GetPosX(),
				zone->GetPosY() - zone->GetSizeY() / 2 + GetPosY() - GetSizeY() / 2.f + construction.GetPosY());
			return center;
		}
	}
	else {
		auto plot = GetParentPlot();
		if (plot) {
			auto center = plot->GetPosition(
				GetPosX() - GetSizeX() / 2.f + construction.GetPosX(),
				GetPosY() - GetSizeY() / 2.f + construction.GetPosY());
			return center;
		}
	}
	return { 0.f, 0.f };
}

Layout* Building::ReadTemplates(string path) {
	// 读取所有布局模板
	if (!filesystem::exists(REPLACE_PATH(path))) {
		THROW_EXCEPTION(IOException, "Path does not exist: " + path + ".\n");
	}

	auto layout = new Layout();

	for (const auto& entry : filesystem::directory_iterator(path)) {
		if (!entry.is_regular_file()) continue;

		string filename = entry.path().filename().string();
		string basename = filename.substr(0, filename.length() - 5);
		string extension = filename.substr(filename.length() - 4);
		if (extension != "json") continue;

		JsonReader reader;
		JsonValue root;

		ifstream fin(entry.path());
		if (!fin.is_open()) {
			THROW_EXCEPTION(IOException, "Failed to open file: " + path + ".\n");
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

void Building::AllocateFloors() {
	// 分配楼层空间（将floors操作移动至对应dll，否则析构时崩溃）
	floors.resize(GetBasements() + GetLayers());
}

void Building::ReadFloor(int level, int face, string name, Layout* layout) {
	// 根据模板生成一层楼层
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

void Building::ReadFloors(int face, string name, Layout* layout) {
	// 按照单一模板生成所有楼层
	if (!layout) {
		THROW_EXCEPTION(InvalidArgumentException, "Layout pointer is null.\n");
	}
	for (int i = 0; i < basements + layers; ++i) {
		ReadFloor(i, face, name, layout);
	}
}

void Building::ReadFloors(int face, vector<string> names, Layout* layout) {
	// 按照层数个模板生成所有楼层
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

void Building::AssignRoom(int level, int slot, string name,
	Component* component, RoomFactory* factory) {
	// 为模板中第slot个独立房间生成房间
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
	Room* room = factory->CreateRoom(name);
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
	room->SetAddress(floors[idx]->AssignNumber());
	component->AddRoom(room);

	rooms.push_back(room);
}

void Building::ArrangeRow(int level, int slot, string name, float acreage,
	Component* component, RoomFactory* factory) {
	// 为模板中第slot个联排房间生成房间
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
	if (num - (int)num >= 0.5f)
		num = num + 1;

	if (row.GetDirection() == 0 || row.GetDirection() == 1) {
		float div = row.GetSizeY() / (int)num;
		for (int i = 0; i < (int)num; ++i) {
			Room* room = factory->CreateRoom(name);
			if (!room) {
				THROW_EXCEPTION(NullPointerException, "Failed to create room.\n");
			}
			room->SetLayer(level);

			room->SetVertices(
				row.GetLeft(), row.GetBottom() + div * i,
				row.GetRight(), row.GetBottom() + div * (i + 1));
			room->SetDoors(rows[slot].GetDoors());
			room->SetWindows(rows[slot].GetWindows());
			room->SetAddress(floors[idx]->AssignNumber());
			component->AddRoom(room);

			rooms.push_back(room);
		}
	}
	else {
		float div = row.GetSizeX() / (int)num;
		for (int i = 0; i < (int)num; ++i) {
			Room* room = factory->CreateRoom(name);
			if (!room) {
				THROW_EXCEPTION(NullPointerException, "Failed to create room.\n");
			}
			room->SetLayer(level);

			room->SetVertices(
				row.GetLeft() + div * i, row.GetBottom(),
				row.GetLeft() + div * (i + 1), row.GetTop());
			room->SetDoors(rows[slot].GetDoors());
			room->SetWindows(rows[slot].GetWindows());
			room->SetAddress(floors[idx]->AssignNumber());
			component->AddRoom(room);

			rooms.push_back(room);
		}
	}
}

Component* Building::CreateComponent(string name, ComponentFactory* factory) {
	// 在建筑内生成空组合
	if (!factory) {
		THROW_EXCEPTION(InvalidArgumentException, "Factory pointer is null.\n");
	}
	Component* component = factory->CreateComponent(name);
	if (!component) {
		debugf("Failed to create component %s\n", name.data());
		return nullptr;
	}
	components.push_back(component);
	return component;
}

vector<float> Building::InverseParams(vector<float>& params, int face) {
	// 根据转向修改矩形参数
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
	// 根据转向修改朝向参数
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

void BuildingFactory::RegisterBuilding(const string& id, const vector<float>& power,
	function<Building* ()> creator, function<void(Building*)> deleter) {
	// 注册构造器和析构器
	registries[id] = { creator, deleter };
	powers[id] = power;
}

Building* BuildingFactory::CreateBuilding(const string& id) {
	// 根据配置构造建筑
	auto cit = configs.find(id);
	if (cit == configs.end() || !cit->second) return nullptr;

	auto it = registries.find(id);
	if (it != registries.end()) {
		return it->second.first();
	}
	return nullptr;
}

bool BuildingFactory::CheckRegistered(const string& id) {
	// 检查是否注册
	return registries.find(id) != registries.end();
}

void BuildingFactory::SetConfig(const string& name, bool config) {
	// 设置启用配置
	configs[name] = config;
}

const unordered_map<string, vector<float>> BuildingFactory::GetPowers() {
	// 获取所有启用建筑全地块权重
	auto result = powers;
	for (auto &[name, power] : powers) {
		if (configs.find(name) == configs.end() || !configs[name]) {
			result.erase(name);
		}
	}
	return result;
}

void BuildingFactory::DestroyBuilding(Building* building) const {
	// 析构建筑
	if (!building) return;
	auto it = registries.find(building->GetType());
	if (it != registries.end()) {
		it->second.second(building);
	}
	else {
		debugf("Deleter not found for %s\n", building->GetType().data());
	}
}