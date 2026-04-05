#include "vehicle_basic.h"


using namespace std;

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
