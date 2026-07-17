#include "asset_basic.h"


using namespace std;

int ZoneAsset::count = 0;

ZoneAsset::ZoneAsset() : id(count++) {

}

ZoneAsset::~ZoneAsset() {

}

const char* ZoneAsset::GetId() {
	return "zone";
}

const char* ZoneAsset::GetType() const {
	return "zone";
}

const char* ZoneAsset::GetName() {
	name = "园区资产" + to_string(id);
	return name.data();
}

void ZoneAsset::DefineAsset() {
	DefineEstate();
}

int BuildingAsset::count = 0;

BuildingAsset::BuildingAsset() : id(count++) {
}

BuildingAsset::~BuildingAsset() {
}

const char* BuildingAsset::GetId() {
	return "building";
}

const char* BuildingAsset::GetType() const {
	return "building";
}

const char* BuildingAsset::GetName() {
	name = "建筑资产" + to_string(id);
	return name.data();
}

void BuildingAsset::DefineAsset() {
	DefineEstate();
}

int RoomAsset::count = 0;

RoomAsset::RoomAsset() : id(count++) {
}

RoomAsset::~RoomAsset() {
}

const char* RoomAsset::GetId() {
	return "room";
}

const char* RoomAsset::GetType() const {
	return "room";
}

const char* RoomAsset::GetName() {
	name = "房间资产" + to_string(id);
	return name.data();
}

void RoomAsset::DefineAsset() {
	DefineEstate();
}

int ContainerAsset::count = 0;

ContainerAsset::ContainerAsset() : id(count++) {
}

ContainerAsset::~ContainerAsset() {
}

const char* ContainerAsset::GetId() {
	return "container";
}

const char* ContainerAsset::GetType() const {
	return "container";
}

const char* ContainerAsset::GetName() {
	name = "容器" + std::to_string(id);
	return name.data();
}

void ContainerAsset::DefineAsset() {
	DefineContainer(1.0f, 1.0f, 10.0f, true);
}

int VehicleAsset::count = 0;

VehicleAsset::VehicleAsset() : id(count++) {
}

VehicleAsset::~VehicleAsset() {
}

const char* VehicleAsset::GetId() {
	return "vehicle";
}

const char* VehicleAsset::GetType() const {
	return "vehicle";
}

const char* VehicleAsset::GetName() {
	name = "载具资产" + to_string(id);
	return name.data();
}

void VehicleAsset::DefineAsset() {
	DefineVehicle();
}

