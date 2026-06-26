#include "station_basic.h"


using namespace std;

int AirportStation::count = 0;

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

int AirportBuilding::count = 0;

AirportBuilding::AirportBuilding() : id(count++) {

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

void AirportBuilding::LayoutBuilding(const Quad* quad) {
	layers = 1;
	basements = 0;
	height = 2.0f;
	wallTexture = "/Game/Asset/Materials/White.White";

	int direction = GetRandom(4);

	string component = "empty";
	AssignFloor(0, direction, "single_room_f^");
	AssignRoom(0, 0, "empty", component, 0);

	script = { "empty", { "basic_building" } };
}

void AirportBuilding::PlaceConstruction() {
	construction = Quad(0.3f, 0.5f, 0.4f, 0.8f);
}

void AirportBuilding::PlacePivots(Quad* building) {

}
