#include "vehicle.h"


using namespace std;

// 飞机载具

string PlaneVehicle::GetId() {
	return "plane";
}

string PlaneVehicle::GetType() const {
	return "plane";
}

string PlaneVehicle::GetName() const {
	return "飞机";
}