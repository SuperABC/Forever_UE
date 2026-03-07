#include "building.h"

#include <cmath>

using namespace std;

// 默认住宅建筑

int DefaultResidentialBuilding::count = 0;

DefaultResidentialBuilding::DefaultResidentialBuilding() : name(count++) {

}

DefaultResidentialBuilding::~DefaultResidentialBuilding() {

}

string DefaultResidentialBuilding::GetId() {
	return "default_residential";
}

string DefaultResidentialBuilding::GetType() const {
	return "default_residential";
}

string DefaultResidentialBuilding::GetName() const {
	return "默认住宅建筑" + to_string(name);
}

vector<float> DefaultResidentialBuilding::GetPower() {
	return { 0.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
}

float DefaultResidentialBuilding::RandomAcreage() const {
	return 600.f * powf(1.f + GetRandom(1000) / 1000.f * 3.f, 2);
}

float DefaultResidentialBuilding::GetAcreageMin() const {
	return 600.f;
}

float DefaultResidentialBuilding::GetAcreageMax() const {
	return 9600.f;
}

Quad DefaultResidentialBuilding::LayoutConstruction() {
	if (GetAcreage() < 1000) {
		SetLayers(1 + GetRandom(2));
	}
	else if (GetAcreage() < 4000) {
		SetLayers(2 + GetRandom(3));
	}
	else {
		SetLayers(4 + GetRandom(4));
	}
	SetBasements(1);

	return Quad(0.5f * GetSizeX(), 0.5f * GetSizeY(), 0.8f * GetSizeX(), 0.8f * GetSizeY());
}

void DefaultResidentialBuilding::LayoutRooms(
	ComponentFactory* componentFactory, RoomFactory* roomFactory, Layout* layout) {
	if (!componentFactory || !roomFactory || !layout) {
		THROW_EXCEPTION(InvalidArgumentException, "Null pointer passed to LayoutRooms.\n");
	}
	AllocateFloors();

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

	auto component = CreateComponent("default_residential", componentFactory);
	if (!component) {
		THROW_EXCEPTION(NullPointerException, "Failed to create default residential component.\n");
	}
	ReadFloor(-1, direction, "straight_linear_1b", layout);
	ArrangeRow(-1, 0, "default_residential", 200.f, component, roomFactory);
	ArrangeRow(-1, 1, "default_residential", 200.f, component, roomFactory);
	ReadFloor(0, direction, "straight_linear_1f", layout);
	ArrangeRow(0, 0, "default_residential", 200.f, component, roomFactory);
	ArrangeRow(0, 1, "default_residential", 200.f, component, roomFactory);
	for (int i = 1; i < GetLayers(); ++i) {
		ReadFloor(i, direction, "straight_linear_2f", layout);
		ArrangeRow(i, 0, "default_residential", 200.f, component, roomFactory);
		ArrangeRow(i, 1, "default_residential", 200.f, component, roomFactory);
	}
}

// 默认工作建筑

int DefaultWorkingBuilding::count = 0;

DefaultWorkingBuilding::DefaultWorkingBuilding() : name(count++) {}

DefaultWorkingBuilding::~DefaultWorkingBuilding() {}

string DefaultWorkingBuilding::GetId() {
	return "default_working";
}

string DefaultWorkingBuilding::GetType() const {
	return "default_working";
}

string DefaultWorkingBuilding::GetName() const {
	return "默认工作建筑" + to_string(name);
}

vector<float> DefaultWorkingBuilding::GetPower() {
	return { 0.f, .5f, .5f, .5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
}

float DefaultWorkingBuilding::RandomAcreage() const {
	return 1000.f * powf(1.f + GetRandom(1000) / 1000.f * 3.f, 2);
}

float DefaultWorkingBuilding::GetAcreageMin() const {
	return 1000.f;
}

float DefaultWorkingBuilding::GetAcreageMax() const {
	return 16000.f;
}

Quad DefaultWorkingBuilding::LayoutConstruction() {
	if (GetAcreage() < 3000) {
		SetLayers(1 + GetRandom(2));
	}
	else if (GetAcreage() < 10000) {
		SetLayers(2 + GetRandom(3));
	}
	else {
		SetLayers(4 + GetRandom(4));
	}
	SetBasements(1);

	return Quad(0.5f * GetSizeX(), 0.5f * GetSizeY(), 0.6f * GetSizeX(), 0.6f * GetSizeY());
}

void DefaultWorkingBuilding::LayoutRooms(
	ComponentFactory* componentFactory, RoomFactory* roomFactory, Layout* layout) {
	if (!componentFactory || !roomFactory || !layout) {
		THROW_EXCEPTION(InvalidArgumentException, "Null pointer passed to LayoutRooms.\n");
	}
	AllocateFloors();

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

	auto component = CreateComponent("default_working", componentFactory);
	if (!component) {
		THROW_EXCEPTION(NullPointerException, "Failed to create default working component.\n");
	}
	ReadFloor(-1, direction, "lobby_linear_1b", layout);
	AssignRoom(-1, 0, "default_working", component, roomFactory);
	ArrangeRow(-1, 0, "default_working", 200.f, component, roomFactory);
	ArrangeRow(-1, 1, "default_working", 200.f, component, roomFactory);
	ReadFloor(0, direction, "lobby_linear_1f", layout);
	AssignRoom(0, 0, "default_working", component, roomFactory);
	ArrangeRow(0, 0, "default_working", 200.f, component, roomFactory);
	ArrangeRow(0, 1, "default_working", 200.f, component, roomFactory);
	for (int i = 1; i < GetLayers(); ++i) {
		ReadFloor(i, direction, "lobby_linear_2f", layout);
		AssignRoom(i, 0, "default_working", component, roomFactory);
		ArrangeRow(i, 0, "default_working", 200.f, component, roomFactory);
		ArrangeRow(i, 1, "default_working", 200.f, component, roomFactory);
	}
}