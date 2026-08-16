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

void ResidentialZone::LayoutZone(const Lot* lot) {
	acreage = 20000.f;
	mesh = "/Game/Asset/Meshes/Fence.Fence";
	unit = 0.2f;
	buildings = { {"residential_high", 1.f}, {"residential_high", 1.f}, {"residential_high", 1.f}, {"residential_high", 1.f} };
	script = { "empty", { "basic_zone" } };
}

void ResidentialZone::PlacePivots(Quad* zone) {

}

function<int(const Lot*)> ResidentialZone::ZoneAssigner = [](const Lot* lot) -> int {
	if (lot->GetArea() == AREA_RESIDENTIAL_HIGH ||
		lot->GetArea() == AREA_RESIDENTIAL_MIDDLE)return 1;
	else return 0;
};

