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

function<int(Lot*)> ResidentialBuilding::BuildingAssigner = [](Lot* lot) {
	return 1;
};

float ResidentialBuilding::RandomAcreage() {
	maxAcreage = 9600.f;
	minAcreage = 600.f;
	return 600.f * powf(1.f + GetRandom(1000) / 1000.f * 3.f, 2);
}

void ResidentialBuilding::LayoutBuilding() {
	if (GetAcreage() < 1000) {
		layers = 1 + GetRandom(2);
	}
	else if (GetAcreage() < 4000) {
		layers = 2 + GetRandom(3);
	}
	else {
		layers = 4 + GetRandom(4);
	}
	basements = 1;
	height = 0.4f;

	int direction = 0;
	if (GetSizeX() > GetSizeY()) {
		if (GetSizeY() > 3.f) {
			direction = GetRandom(2);
		}
		else {
			direction = 2 + GetRandom(2);
		}
	}
	else {
		if (GetSizeX() > 3.f) {
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
}

void ResidentialBuilding::PlaceConstruction() {
	construction = Quad(0.5f * GetSizeX(), 0.5f * GetSizeY(), 0.8f * GetSizeX(), 0.8f * GetSizeY());
}
