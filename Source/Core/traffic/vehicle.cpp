#include "vehicle.h"


using namespace std;

Vehicle::Vehicle(VehicleFactory* factory, const string& vehicle) :
	mod(factory->CreateVehicle(vehicle)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()) {

}

Vehicle::~Vehicle() {
	factory->DestroyVehicle(mod);
}

string Vehicle::GetType() const {
	return type;
}

string Vehicle::GetName() const {
	return name;
}

int EmptyVehicle::count = 0;

EmptyVehicle::EmptyVehicle() : id(count++) {

}

EmptyVehicle::~EmptyVehicle() {

}

const char* EmptyVehicle::GetId() {
	return "empty";
}

const char* EmptyVehicle::GetType() const {
	return "empty";
}

const char* EmptyVehicle::GetName() {
	name = "空载具" + to_string(id);
	return name.data();
}


