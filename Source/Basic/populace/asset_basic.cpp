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

}

