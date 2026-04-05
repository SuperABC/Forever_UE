#include "station.h"


using namespace std;

Station::Station(StationFactory* factory, const string& station) :
	mod(factory->CreateStation(station)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()) {

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

int EmptyStation::count = 0;

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


