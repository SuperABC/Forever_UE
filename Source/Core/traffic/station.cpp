#include "station.h"


using namespace std;

Station::Station(StationFactory* factory, const string& station) :
	mod(factory->CreateStation(station)),
	factory(factory),
	type(),
	name(),
	building() {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Station " + station + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Station::~Station() {
	factory->DestroyStation(mod);
}

string Station::GetType() const {
	return type;
}

string Station::GetName() const {
	return name;
}

string Station::GetBuilding() const {
	return building;
}

void Station::LayoutStation(const Lot* block) {
	mod->LayoutStation(block);
}

string Station::GetBuildingType() const {
	return mod->buildingType;
}

float Station::GetBuildingAcreage() const {
	return mod->buildingAcreage;
}

void Station::PlaceInterface(const Quad* building) {
	mod->PlaceInterface(building);
}

vector<pair<string, vector<float>>> Station::GetInterfaces() const {
	return mod->interfaces;
}

void Station::SetBuilding(string building) {
	this->building = building;
}

int EmptyStation::count = 0;

function<vector<int>& (const vector<Lot*>& blocks)> EmptyStation::StationAssigner =
[](const vector<Lot*>& blocks) -> vector<int>& {
	static vector<int> result;
	result = vector<int>(blocks.size(), 0);
	return result;
};

EmptyStation::EmptyStation() : id(count++) {

}

EmptyStation::~EmptyStation() {

}

const char* EmptyStation::GetId() {
	return "empty";
}

const char* EmptyStation::GetType() const {
	return "empty";
}

const char* EmptyStation::GetName() {
	name = "空站点" + to_string(id);
	return name.data();
}

void EmptyStation::LayoutStation(const Lot* block) {

}

void EmptyStation::PlaceInterface(const Quad* building) {

}


