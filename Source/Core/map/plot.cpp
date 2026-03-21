#include "plot.h"
#include "utility.h"
#include "error.h"

#include <cmath>
#include <utility>
#include <algorithm>


using namespace std;

Plot::Plot() :
	Lot(), address() {//, zones(), buildings() {

}

Plot::Plot(Lot lot) :
	Lot(lot), address() {//, zones(), buildings() {

}

Plot::~Plot() {
	for (auto [road, _] : roads) {
		delete road;
	}
}

const vector<pair<Connection*, float>>& Plot::GetRoads() const {
	return roads;
}

void Plot::SetRoads(const vector<pair<Connection*, float>>& r) {
	roads = r;
}

//unordered_map<string, Zone*>& Plot::GetZones() {
//	return zones;
//}
//
//unordered_map<string, Building*>& Plot::GetBuildings() {
//	return buildings;
//}
//
//void Plot::AddZone(const string& name, Zone* zone) {
//	if (zones.find(name) != zones.end()) {
//		THROW_EXCEPTION(RuntimeException, "Duplicate zone name: " + name + ".\n");
//	}
//	zones[name] = zone;
//}
//
//void Plot::AddBuilding(const string& name, Building* building) {
//	if (buildings.find(name) != buildings.end()) {
//		THROW_EXCEPTION(RuntimeException, "Duplicate building name: " + name + ".\n");
//	}
//	buildings[name] = building;
//}
//
//Zone* Plot::GetZone(const string& name) const {
//	for (const auto& [zoneName, zonePtr] : zones) {
//		if (zoneName == name) {
//			return zonePtr;
//		}
//	}
//	return nullptr;
//}
//
//Building* Plot::GetBuilding(const string& name) const {
//	for (const auto& [bldName, bldPtr] : buildings) {
//		if (bldName == name) {
//			return bldPtr;
//		}
//	}
//	return nullptr;
//}
//
//void Plot::RemoveZone(const string& name) {
//	for (auto it = zones.begin(); it != zones.end(); ) {
//		if (it->first == name) {
//			it = zones.erase(it);
//		}
//		else {
//			++it;
//		}
//	}
//}
//
//void Plot::RemoveBuilding(const string& name) {
//	for (auto it = buildings.begin(); it != buildings.end(); ) {
//		if (it->first == name) {
//			it = buildings.erase(it);
//		}
//		else {
//			++it;
//		}
//	}
//}

void Plot::SetAddress(const string& road, int id) {
	address.first = road;
	address.second = id;
}

string Plot::GetAddress() const {
	return address.first + " " + to_string(address.second);
}

