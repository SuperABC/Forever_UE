#include "room_basic.h"


using namespace std;

int ResidentialRoom::count = 0;

ResidentialRoom::ResidentialRoom() : id(count++) {

}

ResidentialRoom::~ResidentialRoom() {

}

const char* ResidentialRoom::GetId() {
	return "residential";
}

const char* ResidentialRoom::GetType() const {
	return "residential";
}

const char* ResidentialRoom::GetName() {
	name = "住宅房间" + to_string(id);
	return name.data();
}

void ResidentialRoom::ConfigRoom() {
	isResidential = true;
	residentialCapacity = 1;
}
