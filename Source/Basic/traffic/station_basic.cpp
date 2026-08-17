#include "station_basic.h"


using namespace std;

int AirportStation::count = 0;

function<vector<int>& (const vector<Lot*>& blocks)> AirportStation::StationAssigner =
[](const vector<Lot*>& blocks) -> vector<int>&{
	static vector<int> result;
	result = vector<int>(blocks.size(), 0);
	result[0] = 1;
	return result;
};

AirportStation::AirportStation() : id(count++) {

}

AirportStation::~AirportStation() {

}

const char* AirportStation::GetId() {
	return "airport";
}

const char* AirportStation::GetType() const {
	return "airport";
}

const char* AirportStation::GetName() {
	name = "机场站点" + to_string(id);
	return name.data();
}

void AirportStation::LayoutStation(const Lot* block) {
	buildingType = "airport";
	buildingAcreage = 25600.f;
}

void AirportStation::PlaceInterface(const Quad* building, const vector<Node*>& pivots) {
	// 机场站点需要5个节点：1个入口节点 + 2组跑道接口（各2个节点）
	if (pivots.size() != 5)return;
	nodes = { pivots[0] };
	interfaces.push_back({ "air", {pivots[1], pivots[2]} });
	interfaces.push_back({ "air", {pivots[3], pivots[4]} });
}

int AirportBuilding::count = 0;

AirportBuilding::AirportBuilding() : id(count++) {
	direction = GetRandom(4);
}

AirportBuilding::~AirportBuilding() {

}

const char* AirportBuilding::GetId() {
	return "airport";
}

const char* AirportBuilding::GetType() const {
	return "airport";
}

const char* AirportBuilding::GetName() {
	name = "机场建筑" + to_string(id);
	return name.data();
}

vector<float> AirportBuilding::GetPowers() {
	return vector<float>(AREA_END, 0.f);
}

function<int(const Lot*, int, int)> AirportBuilding::BuildingAssigner = [](const Lot*, int, int) {
	return 0;
};

float AirportBuilding::RandomAcreage() {
	return 0.f;
}

void AirportBuilding::PlaceConstruction(const Quad* quad) {
	AssignConstruction(direction, 0.5f, 0.3f, 0.8f, 0.4f);
}

void AirportBuilding::LayoutBuilding(const Quad* quad) {
	layers = 1;
	basements = 0;
	height = 2.0f;
	wallTexture = "/Game/Asset/Materials/White.White";

	string component = "empty";
	AssignFloor(0, direction, "preset_single_room_f^");
	AssignRoom(0, 0, "empty", component, 0);

	script = { "empty", { "basic_building" } };
}

void AirportBuilding::PlacePivots(Quad* building) {
	AddPivot({ 0.5f, 0.0f, 0.8f, 0.0f }, direction);
	AddPivot({ 0.2f, 0.0f, 0.8f, 0.0f }, direction);
	AddPivot({ 0.1f, 0.0f, 0.8f, 0.0f }, direction);
	AddPivot({ 0.8f, 0.0f, 0.8f, 0.0f }, direction);
	AddPivot({ 0.9f, 0.0f, 0.8f, 0.0f }, direction);
}
