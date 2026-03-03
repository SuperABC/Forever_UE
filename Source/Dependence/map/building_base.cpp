#include "../common/error.h"
#include "../common/json.h"
#include "../common/utility.h"

#include "building_base.h"

#include <filesystem>
#include <string>
#include <fstream>


using namespace std;

Stair::Stair(vector<float> params) {
    this->params = params;
}

void Stair::SetDirection(FACE_DIRECTION direction) {
    this->direction = direction;
}

FACE_DIRECTION Stair::GetDirection() const {
    return direction;
}

void Stair::InstanciateQuad(float width, float height) {
    float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
    float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
    float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
    float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
    Quad::SetPosition(x, y, w, h);
}

Ceiling::Ceiling(vector<float> params) {
    this->params = params;
}

void Ceiling::InstanciateQuad(float width, float height) {
    float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
    float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
    float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
    float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
    Quad::SetPosition(x, y, w, h);
}

Corridor::Corridor(vector<float> params) {
    walls.resize(4);

    this->params = params;
}

void Corridor::AddWall(int direction) {
    walls[direction] = true;
}

void Corridor::AddDoor(FACE_DIRECTION direction, std::vector<std::vector<float>> positions) {
    for (auto p : positions) {
        doors[direction].push_back({ p, Quad() });
    }
}

void Corridor::AddWindow(FACE_DIRECTION direction, std::vector<std::vector<float>> positions) {
    for (auto p : positions) {
        windows[direction].push_back({ p, Quad() });
    }
}

void Corridor::InstanciateQuad(float width, float height) {
    float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
    float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
    float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
    float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
    Quad::SetPosition(x, y, w, h);
}

Single::Single(vector<float> params) {
    this->params = params;
}

void Single::AddDoor(FACE_DIRECTION direction, std::vector<std::vector<float>> positions) {
    for (auto p : positions) {
        doors[direction].push_back({ p, Quad() });
    }
}

void Single::AddWindow(FACE_DIRECTION direction, std::vector<std::vector<float>> positions) {
    for (auto p : positions) {
        windows[direction].push_back({ p, Quad() });
    }
}

void Single::InstanciateQuad(float width, float height) {
    float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
    float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
    float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
    float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
    Quad::SetPosition(x, y, w, h);
}

Row::Row(vector<float> params) {
    this->params = params;
}

void Row::SetDirection(FACE_DIRECTION direction) {
    this->direction = direction;
}

FACE_DIRECTION Row::GetDirection() const {
    return direction;
}

void Row::AddDoor(FACE_DIRECTION direction, std::vector<std::vector<float>> positions) {
    for (auto p : positions) {
        doors[direction].push_back({ p, Quad() });
    }
}

void Row::AddWindow(FACE_DIRECTION direction, std::vector<std::vector<float>> positions) {
    for (auto p : positions) {
        windows[direction].push_back({ p, Quad() });
    }
}

void Row::InstanciateQuad(float width, float height) {
    float x = (params[0] * width + params[1] + params[4] * width + params[5]) / 2.f;
    float y = (params[2] * height + params[3] + params[6] * height + params[7]) / 2.f;
    float w = abs(params[4] * width + params[5] - params[0] * width - params[1]);
    float h = abs(params[6] * height + params[7] - params[2] * height - params[3]);
    Quad::SetPosition(x, y, w, h);
}

Floor::Floor(int level, float width, float height) : level(level) {
    SetVertices(0, 0, width, height);
}

Floor::~Floor() {

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

int Floor::GetLevel() const {
    return level;
}

std::vector<Stair>& Floor::GetStairs() {
    return stairs;
}

std::vector<Ceiling>& Floor::GetCeilings() {
    return ceilings;
}

std::vector<Corridor>& Floor::GetCorridors() {
    return corridors;
}

std::vector<Single>& Floor::GetSingles() {
    return singles;
}

std::vector<Row>& Floor::GetRows() {
    return rows;
}

int Floor::AssignNumber() {
    return number++;
}

Building::Building() {

}

Building::~Building() {
    for(auto floor : floors) {
        if(floor)delete floor;
    }
     for(auto component : components) {
        if(component)delete component;
    }
     for(auto room : rooms) {
        if(room)delete room;
	 }
}

void Building::SetParent(Plot* plot) {
    parentPlot = plot;
}

void Building::SetParent(Zone* zone) {
    parentZone = zone;
}

Plot* Building::GetParentPlot() const {
    return parentPlot;
}

Zone* Building::GetParentZone() const {
    return parentZone;
}

int Building::GetOwner() const {
    return ownerId;
}

void Building::SetOwner(int id) {
    ownerId = id;
}

bool Building::GetStateOwned() const {
    return stateOwned;
}

void Building::SetStateOwned(bool state) {
    stateOwned = state;
}

int Building::GetLayers() const {
    return layers;
}

int Building::GetBasements() const {
    return basements;
}

const Quad Building::GetConstruction() const {
	return construction;
}

vector<Component*>& Building::GetComponents() {
    return components;
}

vector<Room*>& Building::GetRooms() {
    return rooms;
}

Floor* Building::GetFloor(int level) const {
    if (basements + level >= 0 && basements + level < floors.size())
        return floors[basements + level];
    else return nullptr;
}

void Building::FinishInit() {
	construction = LayoutConstruction();
}

pair<float, float> Building::GetPosition() const {
	auto zone = GetParentZone();
    if(zone) {
        auto plot = zone->GetParent();
        if(plot) {
            auto center = plot->GetPosition(
                zone->GetPosX() - zone->GetSizeX() / 2 + GetPosX() - GetSizeX() / 2.f + construction.GetPosX(),
                zone->GetPosY() - zone->GetSizeY() / 2 + GetPosY() - GetSizeY() / 2.f + construction.GetPosY());
            return center;
        }
	}
    else {
        auto plot = GetParentPlot();
        if(plot) {
            auto center = plot->GetPosition(
                GetPosX() - GetSizeX() / 2.f + construction.GetPosX(),
                GetPosY() - GetSizeY() / 2.f + construction.GetPosY());
            return center;
		}
    }

	return { 0.f, 0.f };
}

Layout* Building::ReadTemplates(string path) {
    if (!filesystem::exists(REPLACE_PATH(path))) {
        THROW_EXCEPTION(IOException, "Path does not exist: " + path + ".\n");
    }

    auto layout = new Layout();

    for (const auto& entry : filesystem::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            string filename = entry.path().filename().string();
            string basename = filename.substr(0, filename.length() - 5);
            string extension = filename.substr(filename.length() - 4, filename.length());
            if (extension != "json")continue;

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
                        THROW_EXCEPTION(JsonFormatException, "Layout format error: rect must have 8 floats.\n");
                    }
                    for (int i = 0; i < 8; i++) {
                        rect[i] = s["rect"][i].AsFloat();
                    }
                    for (int i = 0; i < 4; i++) {
                        Stair stair(InverseParams(rect, i));
                        stair.SetDirection((FACE_DIRECTION)InverseDirection(s["direction"].AsInt(), i));
                        layout->templateStairs[basename][i].push_back(stair);
                    }
                }
                for (auto c : root["ceilings"]) {
                    vector<float> rect(8);
                    if (c.size() != 8) {
                        THROW_EXCEPTION(JsonFormatException, "Layout format error: rect must have 8 floats.\n");
                    }
                    for (int i = 0; i < 8; i++) {
                        rect[i] = c[i].AsFloat();
                    }
                    for (int i = 0; i < 4; i++) {
                        Ceiling ceiling(InverseParams(rect, i));
                        layout->templateCeilings[basename][i].push_back(ceiling);
                    }
                }
                for (auto c : root["corridors"]) {
                    vector<float> rect(8);
                    if (c["rect"].size() != 8) {
                        THROW_EXCEPTION(JsonFormatException, "Layout format error: rect must have 8 floats.\n");
                    }
                    for (int i = 0; i < 8; i++) {
                        rect[i] = c["rect"][i].AsFloat();
                    }
                    for (int i = 0; i < 4; i++) {
                        Corridor corridor(InverseParams(rect, 0));
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
                        THROW_EXCEPTION(JsonFormatException, "Layout format error: rect must have 8 floats.\n");
                    }
                    for (int i = 0; i < 8; i++) {
                        rect[i] = s["rect"][i].AsFloat();
                    }
                    for (int i = 0; i < 4; i++) {
                        Single single(InverseParams(rect, 0));
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
                        THROW_EXCEPTION(JsonFormatException, "Layout format error: rect must have 8 floats.\n");
                    }
                    for (int i = 0; i < 8; i++) {
                        rect[i] = r["rect"][i].AsFloat();
                    }
                    for (int i = 0; i < 4; i++) {
                        Row row(InverseParams(rect, 0));
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
    }

    return layout;
}

void Building::ReadFloor(int level, int face, string name, Layout* layout) {
    auto width = construction.GetSizeX();
    auto height = construction.GetSizeY();
    auto floor = new Floor(level, width, height);

    for (auto stair : layout->templateStairs[name][face]) {
        stair.InstanciateQuad(width, height);
        floor->AddStair(stair);
    }

    for (auto corridor : layout->templateCorridors[name][face]) {
        corridor.InstanciateQuad(width, height);
        floor->AddCorridor(corridor);
    }

    for (auto single : layout->templateSingles[name][face]) {
        single.InstanciateQuad(width, height);
        floor->AddSingle(single);
    }

    for (auto row : layout->templateRows[name][face]) {
        row.InstanciateQuad(width, height);
        floor->AddRow(row);
    }

    floors[basements + level] = floor;
}

void Building::ReadFloors(int face, string name, Layout* layout) {
    for (int i = 0; i < basements + layers; i++) {
        ReadFloor(i, face, name, layout);
    }
}

void Building::ReadFloors(int face, vector<string> names, Layout* layout) {
    if (names.size() != basements + layers) {
        THROW_EXCEPTION(InvalidArgumentException, "Template number and building layers mismatch.\n");
    }

    for (int i = 0; i < basements + layers; i++) {
        ReadFloor(i, face, names[i], layout);
    }
}

void Building::AssignRoom(int level, int slot, string name, Component* component, RoomFactory* factory) {
    Room* room = factory->CreateRoom(name);
    room->SetLayer(level);
    room->SetPosition(
        floors[basements + level]->GetSingles()[slot].GetPosX(),
        floors[basements + level]->GetSingles()[slot].GetPosY(),
        floors[basements + level]->GetSingles()[slot].GetSizeX(),
        floors[basements + level]->GetSingles()[slot].GetSizeY());
    component->AddRoom(room);
    room->SetAddress(floors[basements + level]->AssignNumber());
    rooms.push_back(room);
}

void Building::ArrangeRow(int level, int slot, string name, float acreage, Component* component, RoomFactory* factory) {
    auto row = floors[basements + level]->GetRows()[slot];

    float num = row.GetAcreage() / acreage;
    if (num - (int)num >= 0.5f)num = num + 1;

    if (row.GetDirection() == 0 || row.GetDirection() == 1) {
        float div = row.GetSizeY() / (int)num;
        for (int i = 0; i < (int)num; i++) {
            Room* room = factory->CreateRoom(name);
            room->SetLayer(level);
            room->SetVertices(row.GetLeft(), row.GetBottom() + div * i,
                row.GetRight(), row.GetBottom() + div * (i + 1));
            component->AddRoom(room);
            room->SetAddress(floors[basements + level]->AssignNumber());
            rooms.push_back(room);
        }
    }
    else {
        float div = row.GetSizeX() / (int)num;
        for (int i = 0; i < (int)num; i++) {
            Room* room = factory->CreateRoom(name);
            room->SetLayer(level);
            room->SetVertices(row.GetLeft() + div * i, row.GetBottom(),
                row.GetLeft() + div * (i + 1), row.GetTop());
            component->AddRoom(room);
            room->SetAddress(floors[basements + level]->AssignNumber());
            rooms.push_back(room);
        }
    }
}

Component* Building::CreateComponent(string name, ComponentFactory* factory) {
    Component* component = factory->CreateComponent(name);

    if (!component) {
        return nullptr;
    }

    components.push_back(component);
    return component;
}

vector<float> Building::InverseParams(vector<float>& params, int face) {
    if (face < 0 || face >= 4) {
        THROW_EXCEPTION(InvalidArgumentException, "Wrong facing direction.\n");
    }
    auto inversed = params;
    switch (face) {
    case 0:
        inversed[0] = 1.f - params[4];
        inversed[1] = -params[5];
        inversed[2] = 1.f - params[6];
        inversed[3] = -params[7];
        inversed[4] = 1.f - params[0];
        inversed[5] = -params[1];
        inversed[6] = 1.f - params[2];
        inversed[7] = -params[3];
        break;
    case 1:
        inversed[6] = params[2];
        inversed[7] = params[3];
        inversed[0] = 1.f - params[4];
        inversed[1] = -params[5];
        inversed[2] = params[6];
        inversed[3] = params[7];
        inversed[4] = 1.f - params[0];
        inversed[5] = -params[1];
        break;
    case 2:
        break;
    case 3:
        inversed[2] = 1.f - params[6];
        inversed[3] = -params[7];
        inversed[4] = params[0];
        inversed[5] = params[1];
        inversed[6] = 1.f - params[2];
        inversed[7] = -params[3];
        inversed[0] = params[4];
        inversed[1] = params[5];
        break;
    default:
        break;
    }

    return inversed;
}

int Building::InverseDirection(int direction, int face) {
    if (face < 0 || face >= 4) {
        THROW_EXCEPTION(InvalidArgumentException, "Wrong facing direction.\n");
    }
    switch (face) {
    case 0:
        if (direction >= 2)return direction - 2;
        else return 3 - direction;
    case 1:
        if (direction >= 2)return 3 - direction;
        else return direction + 2;
    case 2:
        return direction;
    case 3:
        return (5 - direction) % 4;
    }

    return direction;
}

void BuildingFactory::RegisterBuilding(const string& id, function<Building* ()> creator, vector<float> power) {
    registries[id] = creator;
    powers[id] = power;
}

Building* BuildingFactory::CreateBuilding(const string& id) {
    if(configs.find(id) == configs.end() || !configs.find(id)->second)return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second();
    }
    return nullptr;
}

bool BuildingFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void BuildingFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}

const unordered_map<string, vector<float>>& BuildingFactory::GetPowers() const {
    return powers;
}


