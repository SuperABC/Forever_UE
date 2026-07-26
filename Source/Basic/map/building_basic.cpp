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
	if (quad->GetAcreage() < 4000) {
		layers = 3 + GetRandom(3);
	}
	else if (quad->GetAcreage() < 9000) {
		layers = 6 + GetRandom(4);
	}
	else {
		layers = 10 + GetRandom(5);
	}
	basements = 1;
	height = 0.4f;
	wallTexture = "/Game/Asset/Materials/White.White";

	int direction = GetRandom(4);
	int layout = 0;
	int size = 120.f;
	if (quad->GetSizeX() <= 3 || quad->GetSizeY() <= 3) {
		layout = 0;
		size = 40.f;
		if (quad->GetSizeX() > 3)direction = GetRandom(2);
		if(quad->GetSizeY() > 3)direction = 2 + GetRandom(2);
	}
	else if (quad->GetSizeX() <= 5 || quad->GetSizeY() <= 5) {
		layout = GetRandom(2);
		size = 160.f;
		if (quad->GetSizeX() > 5)direction = layout * 2 + GetRandom(2);
		if (quad->GetSizeY() > 5)direction = 2 - layout * 2 + GetRandom(2);
	}
	else if (quad->GetSizeX() <= 7 || quad->GetSizeY() <= 7) {
		layout = 2;
		size = 120.f;
		if (quad->GetSizeX() > 7)direction = GetRandom(2);
		if (quad->GetSizeY() > 7)direction = 2 + GetRandom(2);
	}
	else {
		layout = 3;
		size = 120.f;
		if (quad->GetSizeX() > quad->GetSizeY())direction = GetRandom(2);
		else direction = 2 + GetRandom(2);
	}

	string component = "residential";

	if (layout == 0) {
		AssignFloor(-1, direction, "default_straight_linear_b+");
		ArrangeRow(-1, 0, "residential", size, component, 0);
		ArrangeRow(-1, 1, "residential", size, component, 0);
		AssignFloor(0, direction, "default_straight_linear_f^+-");
		ArrangeRow(0, 0, "residential", size, component, 0);
		ArrangeRow(0, 1, "residential", size, component, 0);
		for (int i = 1; i < layers; i++) {
			AssignFloor(i, direction, "default_straight_linear_f+-");
			ArrangeRow(i, 0, "residential", size, component, 0);
			ArrangeRow(i, 1, "residential", size, component, 0);
		}
	}
	else if (layout == 1) {
		AssignFloor(-1, direction, "default_lobby_wing_b+");
		ArrangeRow(-1, 0, "residential", size, component, 0);
		ArrangeRow(-1, 1, "residential", size, component, 0);
		ArrangeRow(-1, 2, "residential", size, component, 0);
		ArrangeRow(-1, 3, "residential", size, component, 0);
		AssignFloor(0, direction, "default_lobby_wing_f^+-");
		ArrangeRow(0, 0, "residential", size, component, 0);
		ArrangeRow(0, 1, "residential", size, component, 0);
		ArrangeRow(0, 2, "residential", size, component, 0);
		ArrangeRow(0, 3, "residential", size, component, 0);
		for (int i = 1; i < layers; i++) {
			AssignFloor(i, direction, "default_lobby_wing_f+-");
			ArrangeRow(i, 0, "residential", size, component, 0);
			ArrangeRow(i, 1, "residential", size, component, 0);
			ArrangeRow(i, 2, "residential", size, component, 0);
			ArrangeRow(i, 3, "residential", size, component, 0);
		}
	}
	else if (layout == 2) {
		AssignFloor(-1, direction, "default_lshape_double_b+");
		AssignRoom(-1, 0, "residential", component, 0);
		AssignRoom(-1, 1, "residential", component, 0);
		ArrangeRow(-1, 0, "residential", size, component, 0);
		ArrangeRow(-1, 1, "residential", size, component, 0);
		ArrangeRow(-1, 2, "residential", size, component, 0);
		ArrangeRow(-1, 3, "residential", size, component, 0);
		ArrangeRow(-1, 4, "residential", size, component, 0);
		AssignFloor(0, direction, "default_lshape_double_f^+-");
		AssignRoom(0, 0, "residential", component, 0);
		AssignRoom(0, 1, "residential", component, 0);
		ArrangeRow(0, 0, "residential", size, component, 0);
		ArrangeRow(0, 1, "residential", size, component, 0);
		ArrangeRow(0, 2, "residential", size, component, 0);
		ArrangeRow(0, 3, "residential", size, component, 0);
		ArrangeRow(0, 4, "residential", size, component, 0);
		for (int i = 1; i < layers; i++) {
			AssignFloor(i, direction, "default_lshape_double_f+-");
			AssignRoom(i, 0, "residential", component, 0);
			AssignRoom(i, 1, "residential", component, 0);
			ArrangeRow(i, 0, "residential", size, component, 0);
			ArrangeRow(i, 1, "residential", size, component, 0);
			ArrangeRow(i, 2, "residential", size, component, 0);
			ArrangeRow(i, 3, "residential", size, component, 0);
			ArrangeRow(i, 4, "residential", size, component, 0);
		}
	}
	else if(layout == 3) {
		AssignFloor(-1, direction, "default_nshape_double_b+");
		AssignRoom(-1, 0, "empty", component, 0);
		AssignRoom(-1, 1, "residential", component, 0);
		AssignRoom(-1, 2, "residential", component, 0);
		ArrangeRow(-1, 0, "residential", size, component, 0);
		ArrangeRow(-1, 1, "residential", size, component, 0);
		ArrangeRow(-1, 2, "residential", size, component, 0);
		ArrangeRow(-1, 3, "residential", size, component, 0);
		ArrangeRow(-1, 4, "residential", size, component, 0);
		ArrangeRow(-1, 5, "residential", size, component, 0);
		ArrangeRow(-1, 6, "residential", size, component, 0);
		ArrangeRow(-1, 7, "residential", size, component, 0);
		AssignFloor(0, direction, "default_nshape_double_f^+-");
		AssignRoom(0, 0, "empty", component, 0);
		AssignRoom(0, 1, "residential", component, 0);
		AssignRoom(0, 2, "residential", component, 0);
		ArrangeRow(0, 0, "residential", size, component, 0);
		ArrangeRow(0, 1, "residential", size, component, 0);
		ArrangeRow(0, 2, "residential", size, component, 0);
		ArrangeRow(0, 3, "residential", size, component, 0);
		ArrangeRow(0, 4, "residential", size, component, 0);
		ArrangeRow(0, 5, "residential", size, component, 0);
		ArrangeRow(0, 6, "residential", size, component, 0);
		ArrangeRow(0, 7, "residential", size, component, 0);
		for (int i = 1; i < layers; i++) {
			AssignFloor(i, direction, "default_nshape_double_f+-");
			AssignRoom(i, 0, "empty", component, 0);
			AssignRoom(i, 1, "residential", component, 0);
			AssignRoom(i, 2, "residential", component, 0);
			ArrangeRow(i, 0, "residential", size, component, 0);
			ArrangeRow(i, 1, "residential", size, component, 0);
			ArrangeRow(i, 2, "residential", size, component, 0);
			ArrangeRow(i, 3, "residential", size, component, 0);
			ArrangeRow(i, 4, "residential", size, component, 0);
			ArrangeRow(i, 5, "residential", size, component, 0);
			ArrangeRow(i, 6, "residential", size, component, 0);
			ArrangeRow(i, 7, "residential", size, component, 0);
		}

		AddElevator("elevator1", 0, 0, 0, layers - 1, "empty", { "basic_elevator" });
		AddElevator("elevator2", 0, 1, 0, layers - 1, "empty", { "basic_elevator" });
	}

	script = { "empty", { "basic_building" } };
}

void ResidentialBuilding::PlaceConstruction() {
	construction = Quad(0.5f, 0.5f, 0.6f, 0.6f);
}

void ResidentialBuilding::PlacePivots(Quad* building) {

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
	return 1;
};

float ShopBuilding::RandomAcreage() {
	maxAcreage = 8000.f;
	minAcreage = 2000.f;
	return 2000.f * powf(1.f + GetRandom(1000) / 1000.f * 1.f, 2);
}

void ShopBuilding::LayoutBuilding(const Quad* quad) {
	layers = 2;
	height = 0.4f;
	wallTexture = "/Game/Asset/Materials/White.White";

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

	if (quad->GetAcreage() < 4000) {
		AssignFloor(0, direction, "default_lobby_linear_f^+");
		AssignRoom(0, 0, "shop", component, 0);
		ArrangeRow(0, 0, "warehouse", 200.f, component, 0);
		ArrangeRow(0, 1, "warehouse", 200.f, component, 0);
		for (int i = 1; i < layers; i++) {
			AssignFloor(i, direction, "default_lobby_linear_f-");
			AssignRoom(i, 0, "residential", component, 0);
			ArrangeRow(i, 0, "residential", 200.f, component, 0);
			ArrangeRow(i, 1, "residential", 200.f, component, 0);
		}
	}
	else {
		AssignFloor(0, direction, "default_circle_double_f^+");
		AssignRoom(0, 0, "shop", component, 0);
		AssignRoom(0, 1, "shop", component, 0);
		AssignRoom(0, 2, "warehouse", component, 0);
		AssignRoom(0, 3, "warehouse", component, 0);
		AssignRoom(0, 4, "warehouse", component, 0);
		AssignRoom(0, 5, "warehouse", component, 0);
		ArrangeRow(0, 0, "warehouse", 100.f, component, 0);
		ArrangeRow(0, 1, "warehouse", 100.f, component, 0);
		ArrangeRow(0, 2, "warehouse", 100.f, component, 0);
		ArrangeRow(0, 3, "warehouse", 100.f, component, 0);
		ArrangeRow(0, 4, "warehouse", 100.f, component, 0);
		ArrangeRow(0, 5, "warehouse", 100.f, component, 0);
		for (int i = 1; i < layers; i++) {
			AssignFloor(i, direction, "default_circle_double_f+-");
			AssignRoom(i, 0, "warehouse", component, 0);
			AssignRoom(i, 1, "warehouse", component, 0);
			AssignRoom(i, 2, "warehouse", component, 0);
			AssignRoom(i, 3, "warehouse", component, 0);
			AssignRoom(i, 4, "warehouse", component, 0);
			ArrangeRow(i, 0, "warehouse", 100.f, component, 0);
			ArrangeRow(i, 1, "warehouse", 100.f, component, 0);
			ArrangeRow(i, 2, "warehouse", 100.f, component, 0);
			ArrangeRow(i, 3, "warehouse", 100.f, component, 0);
			ArrangeRow(i, 4, "warehouse", 100.f, component, 0);
			ArrangeRow(i, 5, "warehouse", 100.f, component, 0);
			ArrangeRow(i, 6, "warehouse", 100.f, component, 0);
		}
	}

	script = { "empty", { "basic_building" } };
}

void ShopBuilding::PlaceConstruction() {
	construction = Quad(0.5f, 0.5f, 0.8f, 0.8f);
}

void ShopBuilding::PlacePivots(Quad* building) {

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
	basements = 1;
	height = 0.6f;
	wallTexture = "/Game/Asset/Materials/White.White";

	int direction = GetRandom(4);

	string component = "factory";
	AssignFloor(-1, direction, "default_single_room_b+");
	AssignRoom(-1, 0, "parking", component, 0);
	AssignFloor(0, direction, "default_single_room_f^-");
	AssignRoom(0, 0, "factory", component, 0);

	script = { "empty", { "basic_building" } };
}

void FactoryBuilding::PlaceConstruction() {
	construction = Quad(0.5f, 0.5f, 0.8f, 0.8f);
}

void FactoryBuilding::PlacePivots(Quad* building) {

}
