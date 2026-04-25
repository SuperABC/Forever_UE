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

function<int(const Lot*, int, int)> ResidentialBuilding::BuildingAssigner = [](const Lot*, int, int) {
	return 1;
};

float ResidentialBuilding::RandomAcreage() {
	maxAcreage = 18000.f;
	minAcreage = 2000.f;
	return 2000.f * powf(1.f + GetRandom(1000) / 1000.f * 2.f, 2);
}

void ResidentialBuilding::LayoutBuilding(const Quad* quad) {
	if (quad->GetAcreage() < 1000) {
		layers = 3 + GetRandom(2);
	}
	else if (quad->GetAcreage() < 4000) {
		layers = 5 + GetRandom(3);
	}
	else {
		layers = 7 + GetRandom(4);
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

int ShopBuilding::count = 0;

ShopBuilding::ShopBuilding() : id(count++) {

}

ShopBuilding::~ShopBuilding() {

}

const char* ShopBuilding::GetId() {
	return "shop";
}

const char* ShopBuilding::GetType() const {
	return "shop";
}

const char* ShopBuilding::GetName() {
	name = "商店建筑" + to_string(id);
	return name.data();
}

vector<float> ShopBuilding::GetPowers() {
	return vector<float>(AREA_END, 1.f);
}

function<int(const Lot*, int, int)> ShopBuilding::BuildingAssigner = [](const Lot*, int, int) {
	return 0;
};

float ShopBuilding::RandomAcreage() {
	maxAcreage = 8000.f;
	minAcreage = 2000.f;
	return 2000.f * powf(1.f + GetRandom(1000) / 1000.f * 1.f, 2);
}

void ShopBuilding::LayoutBuilding(const Quad* quad) {
	layers = 2;
	height = 0.5f;

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
	
	string component = "shop";
	AssignFloor(0, direction, "lobby_linear_1f");
	AssignRoom(0, 0, "shop", component, 0);
	ArrangeRow(0, 0, "warehouse", 200.f, component, 0);
	ArrangeRow(0, 1, "warehouse", 200.f, component, 0);
	for (int i = 1; i < layers; ++i) {
		AssignFloor(i, direction, "lobby_linear_2f");
		AssignRoom(i, 0, "shop", component, 0);
		ArrangeRow(i, 0, "residential", 200.f, component, 0);
		ArrangeRow(i, 1, "residential", 200.f, component, 0);
	}
	
	script = { "empty", { "basic_building" } };
}

void ShopBuilding::PlaceConstruction() {
	construction = Quad(0.5f, 0.5f, 0.8f, 0.8f);
}

int FactoryBuilding::count = 0;

FactoryBuilding::FactoryBuilding() : id(count++) {

}

FactoryBuilding::~FactoryBuilding() {

}

const char* FactoryBuilding::GetId() {
	return "factory";
}

const char* FactoryBuilding::GetType() const {
	return "factory";
}

const char* FactoryBuilding::GetName() {
	name = "工厂建筑" + to_string(id);
	return name.data();
}

vector<float> FactoryBuilding::GetPowers() {
	return vector<float>(AREA_END, 1.f);
}

function<int(const Lot*, int, int)> FactoryBuilding::BuildingAssigner = [](const Lot*, int, int) {
	return 0;
};

float FactoryBuilding::RandomAcreage() {
	maxAcreage = 16000.f;
	minAcreage = 4000.f;
	return 2000.f * powf(1.f + GetRandom(1000) / 1000.f * 1.f, 2);
}

void FactoryBuilding::LayoutBuilding(const Quad* quad) {
	height = 0.6f;

	int direction = direction = GetRandom(4);
	
	string component = "factory";
	AssignFloor(0, direction, "single_room_1f");
	AssignRoom(0, 0, "factory", component, 0);
	
	script = { "empty", { "basic_building" } };
}

void FactoryBuilding::PlaceConstruction() {
	construction = Quad(0.5f, 0.5f, 0.8f, 0.8f);
}
