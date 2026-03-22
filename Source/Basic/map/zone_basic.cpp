#include "zone_basic.h"


using namespace std;

int ResidentialZone::count = 0;

ResidentialZone::ResidentialZone() : id(count++) {

}

ResidentialZone::~ResidentialZone() {

}

const char* ResidentialZone::GetId() {
	return "residential";
}

const char* ResidentialZone::GetType() const {
	return "residential";
}

const char* ResidentialZone::GetName() {
	name = "住宅园区" + to_string(id);
	return name.data();
}

void ResidentialZone::LayoutZone(Lot* lot) {
	Quad::SetAcreage(10000.f);
	buildings = { {"residential", 1.f}, {"residential", 1.f} };
}

function<int(Lot*)> ResidentialZone::ZoneAssigner = [](Lot* lot) {
	return 1;
};

