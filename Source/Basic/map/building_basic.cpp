#include "building_basic.h"


using namespace std;

int ResidentialBuilding::count = 0;

ResidentialBuilding::ResidentialBuilding() : id(count++) {

}

ResidentialBuilding::~ResidentialBuilding() {

}

const char* ResidentialBuilding::GetId() {
	return "residential";
}

const char* ResidentialBuilding::GetType() const {
	return "residential";
}

const char* ResidentialBuilding::GetName() {
	name = "住宅建筑" + to_string(id);
	return name.data();
}

vector<float> ResidentialBuilding::GetPowers() {
	return vector<float>(AREA_END, 1.f);
}

function<int(const Lot*)> ResidentialBuilding::BuildingAssigner = [](const Lot* lot) {
	return 1;
};

float ResidentialBuilding::RandomAcreage() {
	maxAcreage = 18000.f;
	minAcreage = 2000.f;
	return 2000.f * powf(1.f + GetRandom(1000) / 1000.f * 2.f, 2);
}

void ResidentialBuilding::LayoutBuilding(const Quad* quad) {
	if (quad->GetAcreage() < 1000) {
		layers = 1 + GetRandom(2);
	}
	else if (quad->GetAcreage() < 4000) {
		layers = 2 + GetRandom(3);
	}
	else {
		layers = 4 + GetRandom(4);
	}
	basements = 1;
	height = 0.4f;

	int direction = 0;
	if (quad->GetSizeX() > quad->GetSizeY()) {
		if (quad->GetSizeY() > 3.f) {
			direction = GetRandom(2);
		}
		else {
			direction = 2 + GetRandom(2);
		}
	}
	else {
		if (quad->GetSizeX() > 3.f) {
			direction = 2 + GetRandom(2);
		}
		else {
			direction = GetRandom(2);
		}
	}
	
	string component = "residential";
	AssignFloor(-1, direction, "straight_linear_1b");
	ArrangeRow(-1, 0, "residential", 200.f, component, 0);
	ArrangeRow(-1, 1, "residential", 200.f, component, 0);
	AssignFloor(0, direction, "straight_linear_1f");
	ArrangeRow(0, 0, "residential", 200.f, component, 0);
	ArrangeRow(0, 1, "residential", 200.f, component, 0);
	for (int i = 1; i < layers; ++i) {
		AssignFloor(i, direction, "straight_linear_2f");
		ArrangeRow(i, 0, "residential", 200.f, component, 0);
		ArrangeRow(i, 1, "residential", 200.f, component, 0);
	}
	
	script = { "empty", { "basic_building" } };
}

void ResidentialBuilding::PlaceConstruction() {
	construction = Quad(0.5f, 0.5f, 0.6f, 0.6f);
}
