#include "asset.h"


using namespace std;

string ZoneAsset::GetId() {
	return "zoneAsset";
}

string ZoneAsset::GetType() const {
	return "zoneAsset";
}

string ZoneAsset::GetName() const {
	return "园区资产";
}

ZoneAsset::ZoneAsset(Zone* zone) : zone(zone) {

}

ZoneAsset::~ZoneAsset() {

}

Zone* ZoneAsset::GetZone() {
	return zone;
}

void ZoneAsset::SetZone(Zone* zone) {
	this->zone = zone;
}

string BuildingAsset::GetId() {
	return "buildingAsset";
}

string BuildingAsset::GetType() const {
	return "buildingAsset";
}

string BuildingAsset::GetName() const {
	return "建筑资产";
}

BuildingAsset::BuildingAsset(Building* building) : building(building) {

}

BuildingAsset::~BuildingAsset() {

}

Building* BuildingAsset::GetBuilding() {
	return building;
}

void BuildingAsset::SetBuilding(Building* building) {
	this->building = building;
}

string RoomAsset::GetId() {
	return "roomAsset";
}

string RoomAsset::GetType() const {
	return "roomAsset";
}

string RoomAsset::GetName() const {
	return "房间资产";
}

RoomAsset::RoomAsset(Room* room) : room(room) {

}

RoomAsset::~RoomAsset() {

}

Room* RoomAsset::GetRoom() {
	return room;
}

void RoomAsset::SetRoom(Room* room) {
	this->room = room;
}
