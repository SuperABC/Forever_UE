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
