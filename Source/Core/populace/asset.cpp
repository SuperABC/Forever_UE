#include "asset.h"


using namespace std;

// 园区资产

ZoneAsset::ZoneAsset() {

}

ZoneAsset::~ZoneAsset() {

}

string ZoneAsset::GetId() {
	return "zone";
}

string ZoneAsset::GetType() const {
	return "zone";
}

string ZoneAsset::GetName() const {
	return "园区资产";
}

// 建筑资产

BuildingAsset::BuildingAsset() {

}

BuildingAsset::~BuildingAsset() {

}

string BuildingAsset::GetId() {
	return "building";
}

string BuildingAsset::GetType() const {
	return "building";
}

string BuildingAsset::GetName() const {
	return "建筑资产";
}

// 房间资产

RoomAsset::RoomAsset() {

}

RoomAsset::~RoomAsset() {

}

string RoomAsset::GetId() {
	return "room";
}

string RoomAsset::GetType() const {
	return "room";
}

string RoomAsset::GetName() const {
	return "房间资产";
}