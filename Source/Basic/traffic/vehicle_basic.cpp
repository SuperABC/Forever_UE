#include "vehicle_basic.h"


using namespace std;

int CityVehicle::count = 0;

CityVehicle::CityVehicle() : id(count++) {

}

CityVehicle::~CityVehicle() {

}

const char* CityVehicle::GetId() {
	return "city";
}

const char* CityVehicle::GetType() const {
	return "city";
}

const char* CityVehicle::GetName() {
	name = "城市载具" + to_string(id);
	return name.data();
}

string CityVehicle::GetLabel() {
	return "";
}

void CityVehicle::SetupVehicle() {
	path = "/Game/3rdParty/CitySampleVehicles/vehicle01_Van/BP_vehicle01_Van.BP_vehicle01_Van_C";
	script = { "empty", { "basic_vehicle" } };
}

int PlaneVehicle::count = 0;

PlaneVehicle::PlaneVehicle() : id(count++) {

}

PlaneVehicle::~PlaneVehicle() {

}

const char* PlaneVehicle::GetId() {
	return "plane";
}

const char* PlaneVehicle::GetType() const {
	return "plane";
}

const char* PlaneVehicle::GetName() {
	name = "飞机载具" + to_string(id);
	return name.data();
}

string PlaneVehicle::GetLabel() {
	return "plane";
}

void PlaneVehicle::SetupVehicle() {

}
