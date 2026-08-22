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
	return 0;
};

float ResidentialBuilding::RandomAcreage() {
	maxAcreage = 18000.f;
	minAcreage = 2000.f;
	return 2000.f * powf(1.f + GetRandom(1000) / 1000.f * 2.f, 2);
}

void ResidentialBuilding::PlaceConstruction(const Quad* quad) {
	construction = Quad(0.5f, 0.5f, 0.8f, 0.8f);
}

void ResidentialBuilding::LayoutBuilding(const Quad* quad) {
	if (quad->GetAcreage() < 5000) {
		layers = 3 + GetRandom(3);
	}
	else if (quad->GetAcreage() < 10000) {
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
	float size = 120.f;
	if (quad->GetSizeX() <= 3 || quad->GetSizeY() <= 3) {
		layout = 0;
		size = 40.f;
		if (quad->GetSizeX() > 3)direction = FACE_WEST + GetRandom(2);
		if(quad->GetSizeY() > 3)direction = FACE_NORTH + GetRandom(2);
	}
	else if (quad->GetSizeX() <= 6 || quad->GetSizeY() <= 6) {
		layout = GetRandom(2);
		size = 160.f;
		if (quad->GetSizeX() > 6)direction = layout * 2 + GetRandom(2);
		if (quad->GetSizeY() > 6)direction = 2 - layout * 2 + GetRandom(2);
	}
	else if (quad->GetSizeX() <= 9 || quad->GetSizeY() <= 9) {
		layout = 2;
		size = 120.f;
		if (quad->GetSizeX() > 9)direction = FACE_WEST + GetRandom(2);
		if (quad->GetSizeY() > 9)direction = FACE_NORTH + GetRandom(2);
	}
	else {
		layout = 3;
		size = 120.f;
		if (quad->GetSizeX() > quad->GetSizeY())direction = FACE_WEST + GetRandom(2);
		else direction = FACE_NORTH + GetRandom(2);
	}

	string component = "residential";

	if (layout == 0) {
		AssignFloor(-1, direction, "preset_straight_linear_bg+");
		ArrangeRow(-1, 0, "residential", size, component, 0);
		ArrangeRow(-1, 1, "residential", size, component, 0);
		AssignFloor(0, direction, "preset_straight_linear_fg+-");
		ArrangeRow(0, 0, "residential", size, component, 0);
		ArrangeRow(0, 1, "residential", size, component, 0);
		for (int i = 1; i < layers; i++) {
			AssignFloor(i, direction, "preset_straight_linear_fu+-");
			ArrangeRow(i, 0, "residential", size, component, 0);
			ArrangeRow(i, 1, "residential", size, component, 0);
		}
	}
	else if (layout == 1) {
		AssignFloor(-1, direction, "preset_lobby_wing_bg+");
		ArrangeRow(-1, 0, "residential", size, component, 0);
		ArrangeRow(-1, 1, "residential", size, component, 0);
		ArrangeRow(-1, 2, "residential", size, component, 0);
		ArrangeRow(-1, 3, "residential", size, component, 0);
		AssignFloor(0, direction, "preset_lobby_wing_fg+-");
		ArrangeRow(0, 0, "residential", size, component, 0);
		ArrangeRow(0, 1, "residential", size, component, 0);
		ArrangeRow(0, 2, "residential", size, component, 0);
		ArrangeRow(0, 3, "residential", size, component, 0);
		for (int i = 1; i < layers; i++) {
			AssignFloor(i, direction, "preset_lobby_wing_fu+-");
			ArrangeRow(i, 0, "residential", size, component, 0);
			ArrangeRow(i, 1, "residential", size, component, 0);
			ArrangeRow(i, 2, "residential", size, component, 0);
			ArrangeRow(i, 3, "residential", size, component, 0);
		}

		AddElevator("elevator1", 0, 0, -basements, layers - 1, "empty", { "basic_elevator" });
		AddElevator("elevator2", 0, 1, -basements, layers - 1, "empty", { "basic_elevator" });
	}
	else if (layout == 2) {
		AssignFloor(-1, direction, "preset_lshape_double_bg+");
		AssignRoom(-1, 0, "residential", component, 0);
		AssignRoom(-1, 1, "residential", component, 0);
		ArrangeRow(-1, 0, "residential", size, component, 0);
		ArrangeRow(-1, 1, "residential", size, component, 0);
		ArrangeRow(-1, 2, "residential", size, component, 0);
		ArrangeRow(-1, 3, "residential", size, component, 0);
		ArrangeRow(-1, 4, "residential", size, component, 0);
		AssignFloor(0, direction, "preset_lshape_double_fg+-");
		AssignRoom(0, 0, "residential", component, 0);
		AssignRoom(0, 1, "residential", component, 0);
		ArrangeRow(0, 0, "residential", size, component, 0);
		ArrangeRow(0, 1, "residential", size, component, 0);
		ArrangeRow(0, 2, "residential", size, component, 0);
		ArrangeRow(0, 3, "residential", size, component, 0);
		ArrangeRow(0, 4, "residential", size, component, 0);
		for (int i = 1; i < layers; i++) {
			AssignFloor(i, direction, "preset_lshape_double_fu+-");
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
		AssignFloor(-1, direction, "preset_nshape_double_bg+");
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
		AssignFloor(0, direction, "preset_nshape_double_fg+-");
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
			AssignFloor(i, direction, "preset_nshape_double_fu+-");
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

		AddElevator("elevator1", 0, 0, -basements, layers - 1, "empty", { "basic_elevator" });
		AddElevator("elevator2", 0, 1, -basements, layers - 1, "empty", { "basic_elevator" });
	}

	script = { "empty", { "basic_building" } };
}

void ResidentialBuilding::PlacePivots(Quad* building) {

}

const char* ResidentialLowBuilding::GetId() {
	return "residential_low";
}

const char* ResidentialLowBuilding::GetType() const {
	return "residential_low";
}

vector<float> ResidentialLowBuilding::GetPowers() {
	vector<float> powers(AREA_END, 0.f);
	powers[AREA_RESIDENTIAL_HIGH] = 0.2f;
	powers[AREA_RESIDENTIAL_MIDDLE] = 0.2f;
	powers[AREA_RESIDENTIAL_LOW] = 1.f;
	return powers;
}

float ResidentialLowBuilding::RandomAcreage() {
	minAcreage = 2000.f;
	maxAcreage = 6000.f;
	return 2000.f + GetRandom(1000) / 1000.f * 4000.f;
}

const char* ResidentialMiddleBuilding::GetId() {
	return "residential_middle";
}

const char* ResidentialMiddleBuilding::GetType() const {
	return "residential_middle";
}

vector<float> ResidentialMiddleBuilding::GetPowers() {
	vector<float> powers(AREA_END, 0.f);
	powers[AREA_RESIDENTIAL_HIGH] = 0.2f;
	powers[AREA_RESIDENTIAL_MIDDLE] = 1.f;
	powers[AREA_RESIDENTIAL_LOW] = 0.2f;
	return powers;
}

float ResidentialMiddleBuilding::RandomAcreage() {
	minAcreage = 4000.f;
	maxAcreage = 12000.f;
	return 4000.f + GetRandom(1000) / 1000.f * 8000.f;
}

const char* ResidentialHighBuilding::GetId() {
	return "residential_high";
}

const char* ResidentialHighBuilding::GetType() const {
	return "residential_high";
}

vector<float> ResidentialHighBuilding::GetPowers() {
	vector<float> powers(AREA_END, 0.f);
	powers[AREA_RESIDENTIAL_HIGH] = 1.f;
	powers[AREA_RESIDENTIAL_MIDDLE] = 0.2f;
	powers[AREA_RESIDENTIAL_LOW] = 0.2f;
	return powers;
}

float ResidentialHighBuilding::RandomAcreage() {
	minAcreage = 6000.f;
	maxAcreage = 18000.f;
	return 6000.f + GetRandom(1000) / 1000.f * 12000.f;
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
	vector<float> powers(AREA_END, 0.f);
	powers[AREA_COMMERCIAL_HIGH] = 1.f;
	powers[AREA_COMMERCIAL_MIDDLE] = 1.f;
	powers[AREA_COMMERCIAL_LOW] = 1.f;
	return powers;
}

function<int(const Lot*, int, int)> ShopBuilding::BuildingAssigner = [](const Lot* lot, int, int) {
	if (lot->GetArea() == AREA_COMMERCIAL_HIGH ||
		lot->GetArea() == AREA_COMMERCIAL_MIDDLE ||
		lot->GetArea() == AREA_COMMERCIAL_LOW)return 1;
	else return 0;
};

float ShopBuilding::RandomAcreage() {
	maxAcreage = 16000.f;
	minAcreage = 4000.f;
	return 4000.f * powf(1.f + GetRandom(1000) / 1000.f * 1.f, 2);
}

void ShopBuilding::PlaceConstruction(const Quad* quad) {
	auto scaleX = 0.6f;
	auto scaleY = 0.6f;
	if (quad->GetSizeX() < 6)scaleX = (quad->GetSizeX() - 2.4f) / quad->GetSizeX();
	if (quad->GetSizeY() < 6)scaleY = (quad->GetSizeY() - 2.4f) / quad->GetSizeY();
	construction = Quad(0.5f, 0.5f, scaleX, scaleY);

	if (quad->GetSizeX() > quad->GetSizeY()) {
		direction = FACE_WEST + GetRandom(2);
	}
	else {
		direction = FACE_NORTH + GetRandom(2);
	}
}

void ShopBuilding::LayoutBuilding(const Quad* quad) {
	layers = 2;
	height = 0.4f;
	wallTexture = "/Game/Asset/Materials/White.White";

	string component = "shop";

	if (quad->GetAcreage() < 6000) {
		basements = 0;
		AssignFloor(0, direction, "preset_lobby_linear_fg+");
		AssignRoom(0, 0, "shop", component, 0);
		ArrangeRow(0, 0, "warehouse", 200.f, component, 0);
		ArrangeRow(0, 1, "warehouse", 200.f, component, 0);
		for (int i = 1; i < layers - 1; i++) {
			AssignFloor(i, direction, "preset_lobby_linear_fu+-");
			AssignRoom(i, 0, "warehouse", component, 0);
			ArrangeRow(i, 0, "warehouse", 200.f, component, 0);
			ArrangeRow(i, 1, "warehouse", 200.f, component, 0);
		}
		AssignFloor(layers - 1, direction, "preset_lobby_linear_fu-");
		AssignRoom(layers - 1, 0, "warehouse", component, 0);
		ArrangeRow(layers - 1, 0, "warehouse", 200.f, component, 0);
		ArrangeRow(layers - 1, 1, "warehouse", 200.f, component, 0);
	}
	else {
		basements = 1;
		if (GetRandom(2)) {
			AssignFloor(-1, direction, "preset_circle_double_pg+");
			AssignRoom(-1, 0, "parking", component, 0);
		}
		else {
			AssignFloor(-1, direction, "preset_circle_double_bg+");
			AssignRoom(-1, 0, "warehouse", component, 0);
			AssignRoom(-1, 1, "warehouse", component, 0);
			AssignRoom(-1, 2, "warehouse", component, 0);
			AssignRoom(-1, 3, "warehouse", component, 0);
			AssignRoom(-1, 4, "warehouse", component, 0);
			ArrangeRow(-1, 0, "warehouse", 100.f, component, 0);
			ArrangeRow(-1, 1, "warehouse", 100.f, component, 0);
			ArrangeRow(-1, 2, "warehouse", 100.f, component, 0);
			ArrangeRow(-1, 3, "warehouse", 100.f, component, 0);
			ArrangeRow(-1, 4, "warehouse", 100.f, component, 0);
			ArrangeRow(-1, 5, "warehouse", 100.f, component, 0);
			ArrangeRow(-1, 6, "warehouse", 100.f, component, 0);
		}
		AssignFloor(0, direction, "preset_circle_double_fg+-");
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
			AssignFloor(i, direction, "preset_circle_double_fu+-");
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
	vector<float> powers(AREA_END, 0.f);
	powers[AREA_INDUSTRIAL_HIGH] = 1.f;
	powers[AREA_INDUSTRIAL_MIDDLE] = 1.f;
	powers[AREA_INDUSTRIAL_LOW] = 1.f;
	return powers;
}

function<int(const Lot*, int, int)> FactoryBuilding::BuildingAssigner = [](const Lot* lot, int, int) {
	if (lot->GetArea() == AREA_INDUSTRIAL_HIGH ||
		lot->GetArea() == AREA_INDUSTRIAL_MIDDLE ||
		lot->GetArea() == AREA_INDUSTRIAL_LOW)return 1;
	else return 0;
};

float FactoryBuilding::RandomAcreage() {
	maxAcreage = 16000.f;
	minAcreage = 4000.f;
	return 2000.f * powf(1.f + GetRandom(1000) / 1000.f * 1.f, 2);
}

void FactoryBuilding::PlaceConstruction(const Quad* quad) {
	construction = Quad(0.5f, 0.5f, 0.6f, 0.6f);
}

void FactoryBuilding::LayoutBuilding(const Quad* quad) {
	height = 0.6f;
	wallTexture = "/Game/Asset/Materials/White.White";

	int direction = 0;
	if (quad->GetSizeX() > quad->GetSizeY()) {
		direction = FACE_NORTH + GetRandom(2);
		if (quad->GetSizeX() > 4) {
			basements = 1;
		}
	}
	else {
		direction = FACE_WEST + GetRandom(2);
		if (quad->GetSizeY() > 4) {
			basements = 1;
		}
	}

	string component = "factory";
	if (basements > 0) {
		AssignFloor(-1, direction, "preset_single_room_pg+");
		AssignRoom(-1, 0, "parking", component, 0);
		AssignFloor(0, direction, "preset_single_room_fg-");
	}
	else {
		AssignFloor(0, direction, "preset_single_room_fg");
	}
	AssignRoom(0, 0, "factory", component, 0);

	script = { "empty", { "basic_building" } };
}

void FactoryBuilding::PlacePivots(Quad* building) {

}

int HospitalBuilding::count = 0;

HospitalBuilding::HospitalBuilding() : id(count++) {

}

HospitalBuilding::~HospitalBuilding() {

}

const char* HospitalBuilding::GetId() {
	return "hospital";
}

const char* HospitalBuilding::GetType() const {
	return "hospital";
}

const char* HospitalBuilding::GetName() {
	name = "医院建筑" + to_string(id);
	return name.data();
}

vector<float> HospitalBuilding::GetPowers() {
	return vector<float>(AREA_END, 0.f);
}

function<int(const Lot*, int, int)> HospitalBuilding::BuildingAssigner = [](const Lot* lot, int, int) {
	if (lot->GetArea() == AREA_OFFICIAL_HIGH ||
		lot->GetArea() == AREA_OFFICIAL_MIDDLE ||
		lot->GetArea() == AREA_OFFICIAL_LOW)return 1;
	else return 0;
};

float HospitalBuilding::RandomAcreage() {
	minAcreage = 8000.f;
	maxAcreage = 16000.f;
	return 8000.f + GetRandom(1000) / 1000.f * 8000.f;
}

void HospitalBuilding::PlaceConstruction(const Quad* quad) {
	if (quad->GetSizeX() > quad->GetSizeY()) {
		direction = FACE_NORTH + GetRandom(2);
	}
	else {
		direction = FACE_WEST + GetRandom(2);
	}
	AssignConstruction(direction, 0.5f, 0.4f, 0.8f, 0.4f);
}

void HospitalBuilding::LayoutBuilding(const Quad* quad) {
	basements = GetRandom(3);
	layers = 5 + GetRandom(6);
	height = 0.4f;
	wallTexture = "/Game/Asset/Materials/White.White";

	string component = "hospital";
	float size = 160.f;

	if (basements > 0) {
		AssignFloor(-basements, direction, "preset_entrance_wing_bu+");
		AssignRoom(-basements, 0, "empty", component, 0);
		ArrangeRow(-basements, 0, "morgue", size, component, 0);
		ArrangeRow(-basements, 1, "morgue", size, component, 0);
		ArrangeRow(-basements, 2, "morgue", size, component, 0);
		ArrangeRow(-basements, 3, "morgue", size, component, 0);
	}
	for(int i = -basements + 1; i < 0; i++) {
		AssignFloor(i, direction, "preset_entrance_wing_bu+-");
		AssignRoom(i, 0, "empty", component, 0);
		ArrangeRow(i, 0, "morgue", size, component, 0);
		ArrangeRow(i, 1, "morgue", size, component, 0);
		ArrangeRow(i, 2, "morgue", size, component, 0);
		ArrangeRow(i, 3, "morgue", size, component, 0);
	}
	if (basements == 0) {
		AssignFloor(0, direction, "preset_entrance_wing_fg+");
	}
	else {
		AssignFloor(0, direction, "preset_entrance_wing_fg+-");
	}
	AssignRoom(0, 0, "register", component, 0);
	ArrangeRow(0, 0, "doctor", size, component, 0);
	ArrangeRow(0, 1, "doctor", size, component, 0);
	ArrangeRow(0, 2, "doctor", size, component, 0);
	ArrangeRow(0, 3, "doctor", size, component, 0);
	for (int i = 1; i < layers - 1; i++) {
		AssignFloor(i, direction, "preset_entrance_wing_fu+-");
		AssignRoom(i, 0, "waiting", component, 0);
		ArrangeRow(i, 0, "doctor", size, component, 0);
		ArrangeRow(i, 1, "doctor", size, component, 0);
		ArrangeRow(i, 2, "doctor", size, component, 0);
		ArrangeRow(i, 3, "doctor", size, component, 0);
	}
	AssignFloor(layers - 1, direction, "preset_entrance_wing_fu-");
	AssignRoom(layers - 1, 0, "waiting", component, 0);
	ArrangeRow(layers - 1, 0, "doctor", size, component, 0);
	ArrangeRow(layers - 1, 1, "doctor", size, component, 0);
	ArrangeRow(layers - 1, 2, "doctor", size, component, 0);
	ArrangeRow(layers - 1, 3, "doctor", size, component, 0);

	AddElevator("elevator1", 0, 0, -basements, layers - 1, "empty", { "basic_elevator" });
	AddElevator("elevator2", 0, 1, -basements, layers - 1, "empty", { "basic_elevator" });
	AddElevator("elevator3", 0, 2, -basements, layers - 1, "empty", { "basic_elevator" });
	AddElevator("elevator4", 0, 3, -basements, layers - 1, "empty", { "basic_elevator" });

	script = { "empty", { "basic_building" } };
}

void HospitalBuilding::PlacePivots(Quad* building) {

}

int OfficeBuilding::count = 0;

OfficeBuilding::OfficeBuilding() : id(count++) {

}

OfficeBuilding::~OfficeBuilding() {

}

const char* OfficeBuilding::GetId() {
	return "office";
}

const char* OfficeBuilding::GetType() const {
	return "office";
}

const char* OfficeBuilding::GetName() {
	name = "办公建筑" + to_string(id);
	return name.data();
}

vector<float> OfficeBuilding::GetPowers() {
	vector<float> powers(AREA_END, 0.f);
	powers[AREA_OFFICIAL_HIGH] = 1.f;
	powers[AREA_OFFICIAL_MIDDLE] = 1.f;
	powers[AREA_OFFICIAL_LOW] = 1.f;
	return powers;
}

function<int(const Lot*, int, int)> OfficeBuilding::BuildingAssigner = [](const Lot*, int, int) {
	return 0;
};

float OfficeBuilding::RandomAcreage() {
	maxAcreage = 18000.f;
	minAcreage = 2000.f;
	return 2000.f * powf(1.f + GetRandom(1000) / 1000.f * 2.f, 2);
}

void OfficeBuilding::PlaceConstruction(const Quad* quad) {
	construction = Quad(0.5f, 0.5f, 0.6f, 0.6f);
}

void OfficeBuilding::LayoutBuilding(const Quad* quad) {
	if (quad->GetAcreage() < 5000) {
		layers = 3 + GetRandom(3);
	}
	else if (quad->GetAcreage() < 10000) {
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
	float size = 120.f;
	if (quad->GetSizeX() <= 3 || quad->GetSizeY() <= 3) {
		layout = 0;
		size = 40.f;
		if (quad->GetSizeX() > 3)direction = FACE_WEST + GetRandom(2);
		if(quad->GetSizeY() > 3)direction = FACE_NORTH + GetRandom(2);
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
		if (quad->GetSizeX() > 7)direction = FACE_WEST + GetRandom(2);
		if (quad->GetSizeY() > 7)direction = FACE_NORTH + GetRandom(2);
	}
	else {
		layout = 3;
		size = 120.f;
		if (quad->GetSizeX() > quad->GetSizeY())direction = FACE_WEST + GetRandom(2);
		else direction = FACE_NORTH + GetRandom(2);
	}

	string component = "empty";

	if (layout == 0) {
		AssignFloor(-1, direction, "preset_straight_linear_bg+");
		ArrangeRow(-1, 0, "office", size, component, 0);
		ArrangeRow(-1, 1, "office", size, component, 0);
		AssignFloor(0, direction, "preset_straight_linear_fg+-");
		ArrangeRow(0, 0, "office", size, component, 0);
		ArrangeRow(0, 1, "office", size, component, 0);
		for (int i = 1; i < layers; i++) {
			AssignFloor(i, direction, "preset_straight_linear_fu+-");
			ArrangeRow(i, 0, "office", size, component, 0);
			ArrangeRow(i, 1, "office", size, component, 0);
		}
	}
	else if (layout == 1) {
		AssignFloor(-1, direction, "preset_lobby_wing_bg+");
		ArrangeRow(-1, 0, "office", size, component, 0);
		ArrangeRow(-1, 1, "office", size, component, 0);
		ArrangeRow(-1, 2, "office", size, component, 0);
		ArrangeRow(-1, 3, "office", size, component, 0);
		AssignFloor(0, direction, "preset_lobby_wing_fg+-");
		ArrangeRow(0, 0, "office", size, component, 0);
		ArrangeRow(0, 1, "office", size, component, 0);
		ArrangeRow(0, 2, "office", size, component, 0);
		ArrangeRow(0, 3, "office", size, component, 0);
		for (int i = 1; i < layers; i++) {
			AssignFloor(i, direction, "preset_lobby_wing_fu+-");
			ArrangeRow(i, 0, "office", size, component, 0);
			ArrangeRow(i, 1, "office", size, component, 0);
			ArrangeRow(i, 2, "office", size, component, 0);
			ArrangeRow(i, 3, "office", size, component, 0);
		}

		AddElevator("elevator1", 0, 0, -basements, layers - 1, "empty", { "basic_elevator" });
		AddElevator("elevator2", 0, 1, -basements, layers - 1, "empty", { "basic_elevator" });
	}
	else if (layout == 2) {
		AssignFloor(-1, direction, "preset_lshape_double_bg+");
		AssignRoom(-1, 0, "office", component, 0);
		AssignRoom(-1, 1, "office", component, 0);
		ArrangeRow(-1, 0, "office", size, component, 0);
		ArrangeRow(-1, 1, "office", size, component, 0);
		ArrangeRow(-1, 2, "office", size, component, 0);
		ArrangeRow(-1, 3, "office", size, component, 0);
		ArrangeRow(-1, 4, "office", size, component, 0);
		AssignFloor(0, direction, "preset_lshape_double_fg+-");
		AssignRoom(0, 0, "office", component, 0);
		AssignRoom(0, 1, "office", component, 0);
		ArrangeRow(0, 0, "office", size, component, 0);
		ArrangeRow(0, 1, "office", size, component, 0);
		ArrangeRow(0, 2, "office", size, component, 0);
		ArrangeRow(0, 3, "office", size, component, 0);
		ArrangeRow(0, 4, "office", size, component, 0);
		for (int i = 1; i < layers; i++) {
			AssignFloor(i, direction, "preset_lshape_double_fu+-");
			AssignRoom(i, 0, "office", component, 0);
			AssignRoom(i, 1, "office", component, 0);
			ArrangeRow(i, 0, "office", size, component, 0);
			ArrangeRow(i, 1, "office", size, component, 0);
			ArrangeRow(i, 2, "office", size, component, 0);
			ArrangeRow(i, 3, "office", size, component, 0);
			ArrangeRow(i, 4, "office", size, component, 0);
		}
	}
	else if(layout == 3) {
		AssignFloor(-1, direction, "preset_nshape_double_bg+");
		AssignRoom(-1, 0, "office", component, 0);
		AssignRoom(-1, 1, "office", component, 0);
		AssignRoom(-1, 2, "office", component, 0);
		ArrangeRow(-1, 0, "office", size, component, 0);
		ArrangeRow(-1, 1, "office", size, component, 0);
		ArrangeRow(-1, 2, "office", size, component, 0);
		ArrangeRow(-1, 3, "office", size, component, 0);
		ArrangeRow(-1, 4, "office", size, component, 0);
		ArrangeRow(-1, 5, "office", size, component, 0);
		ArrangeRow(-1, 6, "office", size, component, 0);
		ArrangeRow(-1, 7, "office", size, component, 0);
		AssignFloor(0, direction, "preset_nshape_double_fg+-");
		AssignRoom(0, 0, "office", component, 0);
		AssignRoom(0, 1, "office", component, 0);
		AssignRoom(0, 2, "office", component, 0);
		ArrangeRow(0, 0, "office", size, component, 0);
		ArrangeRow(0, 1, "office", size, component, 0);
		ArrangeRow(0, 2, "office", size, component, 0);
		ArrangeRow(0, 3, "office", size, component, 0);
		ArrangeRow(0, 4, "office", size, component, 0);
		ArrangeRow(0, 5, "office", size, component, 0);
		ArrangeRow(0, 6, "office", size, component, 0);
		ArrangeRow(0, 7, "office", size, component, 0);
		for (int i = 1; i < layers; i++) {
			AssignFloor(i, direction, "preset_nshape_double_fu+-");
			AssignRoom(i, 0, "office", component, 0);
			AssignRoom(i, 1, "office", component, 0);
			AssignRoom(i, 2, "office", component, 0);
			ArrangeRow(i, 0, "office", size, component, 0);
			ArrangeRow(i, 1, "office", size, component, 0);
			ArrangeRow(i, 2, "office", size, component, 0);
			ArrangeRow(i, 3, "office", size, component, 0);
			ArrangeRow(i, 4, "office", size, component, 0);
			ArrangeRow(i, 5, "office", size, component, 0);
			ArrangeRow(i, 6, "office", size, component, 0);
			ArrangeRow(i, 7, "office", size, component, 0);
		}

		AddElevator("elevator1", 0, 0, -basements, layers - 1, "empty", { "basic_elevator" });
		AddElevator("elevator2", 0, 1, -basements, layers - 1, "empty", { "basic_elevator" });
	}

	script = { "empty", { "basic_building" } };
}

void OfficeBuilding::PlacePivots(Quad* building) {

}

int HotelBuilding::count = 0;

HotelBuilding::HotelBuilding() : id(count++) {
	direction = GetRandom(4);
}

HotelBuilding::~HotelBuilding() {

}

const char* HotelBuilding::GetId() {
	return "hotel";
}

const char* HotelBuilding::GetType() const {
	return "hotel";
}

const char* HotelBuilding::GetName() {
	name = "酒店建筑" + to_string(id);
	return name.data();
}

vector<float> HotelBuilding::GetPowers() {
	vector<float> powers(AREA_END, 0.f);
	powers[AREA_COMMERCIAL_HIGH] = 0.2f;
	powers[AREA_COMMERCIAL_MIDDLE] = 0.2f;
	powers[AREA_COMMERCIAL_LOW] = 0.2f;
	return powers;
}

function<int(const Lot*, int, int)> HotelBuilding::BuildingAssigner = [](const Lot* lot, int, int) {
	if (lot->GetArea() == AREA_COMMERCIAL_HIGH ||
		lot->GetArea() == AREA_COMMERCIAL_MIDDLE ||
		lot->GetArea() == AREA_COMMERCIAL_LOW)return 1;
	else return 0;
};

float HotelBuilding::RandomAcreage() {
	minAcreage = 4000.f;
	maxAcreage = 16000.f;
	return 4000.f * powf(1.f + GetRandom(1000) / 1000.f * 1.f, 2);
}

void HotelBuilding::PlaceConstruction(const Quad* quad) {
	AssignConstruction(direction, 0.5f, 0.5f, 0.8f, 0.8f);
}

void HotelBuilding::LayoutBuilding(const Quad* quad) {
	basements = 1;
	layers = 3 + GetRandom(6);
	height = 0.4f;
	wallTexture = "/Game/Asset/Materials/White.White";

	string component = "empty";
	float size = 160.f;

	AssignFloor(-1, direction, "preset_tshape_double_bg+");
	AssignRoom(-1, 0, "empty", component, 0);
	ArrangeRow(-1, 0, "bunk", size, component, 0);
	ArrangeRow(-1, 1, "bunk", size, component, 0);
	ArrangeRow(-1, 2, "bunk", size, component, 0);
	ArrangeRow(-1, 3, "bunk", size, component, 0);
	ArrangeRow(-1, 4, "bunk", size, component, 0);
	ArrangeRow(-1, 5, "bunk", size, component, 0);

	AssignFloor(0, direction, "preset_tshape_double_fg+-");
	AssignRoom(0, 0, "lobby", component, 0);
	ArrangeRow(0, 0, "bunk", size, component, 0);
	ArrangeRow(0, 1, "bunk", size, component, 0);
	ArrangeRow(0, 2, "bunk", size, component, 0);
	ArrangeRow(0, 3, "bunk", size, component, 0);
	ArrangeRow(0, 4, "bunk", size, component, 0);
	ArrangeRow(0, 5, "bunk", size, component, 0);

	for (int i = 1; i < layers; i++) {
		AssignFloor(i, direction, "preset_tshape_double_fu+-");
		AssignRoom(i, 0, "empty", component, 0);
		ArrangeRow(i, 0, "bunk", size, component, 0);
		ArrangeRow(i, 1, "bunk", size, component, 0);
		ArrangeRow(i, 2, "bunk", size, component, 0);
		ArrangeRow(i, 3, "bunk", size, component, 0);
		ArrangeRow(i, 4, "bunk", size, component, 0);
		ArrangeRow(i, 5, "bunk", size, component, 0);
	}

	AddElevator("elevator1", 0, 0, -basements, layers - 1, "empty", { "basic_elevator" });
	AddElevator("elevator2", 0, 1, -basements, layers - 1, "empty", { "basic_elevator" });

	script = { "empty", { "basic_building" } };
}

void HotelBuilding::PlacePivots(Quad* building) {

}
