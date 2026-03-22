#include "zone_basic.h"


using namespace std;

int ResidentialZone::count = 0;

ResidentialZone::ResidentialZone() : id(count++) {

}

ResidentialZone::~ResidentialZone() {

}

string ResidentialZone::GetId() {
	return "residential";
}

string ResidentialZone::GetType() const {
	return "residential";
}

string ResidentialZone::GetName() const {
	return "住宅园区" + to_string(id);
}

void ResidentialZone::LayoutZone(Lot* lot) {
	Quad::SetAcreage(10000.f);
	buildings = { {"residential", 1.f}, {"residential", 1.f} };
}

function<int(Lot*)> ResidentialZone::ZoneAssigner = [](Lot* lot) {
	return 1;
};

