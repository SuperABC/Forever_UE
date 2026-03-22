#include "block.h"
#include "utility.h"
#include "error.h"

#include <cmath>
#include <utility>
#include <algorithm>


using namespace std;

Block::Block() :
	Lot(), address(), zones(), buildings() {

}

Block::Block(Lot lot) :
	Lot(lot), address(), zones(), buildings() {

}

Block::~Block() {
	for (auto [road, _] : roads) {
		delete road;
	}
	for (auto [_, zone] : zones) {
		delete zone;
	}
	for (auto [_, building] : buildings) {
		delete building;
	}
}

const vector<pair<Connection*, float>>& Block::GetRoads() const {
	return roads;
}

void Block::SetRoads(const vector<pair<Connection*, float>>& r) {
	roads = r;
}

unordered_map<string, Zone*>& Block::GetZones() {
	return zones;
}

unordered_map<string, Building*>& Block::GetBuildings() {
	return buildings;
}

void Block::AddZone(const string& name, Zone* zone) {
	if (zones.find(name) != zones.end()) {
		THROW_EXCEPTION(RuntimeException, "Duplicate zone name: " + name + ".\n");
	}
	zones[name] = zone;
}

void Block::AddBuilding(const string& name, Building* building) {
	if (buildings.find(name) != buildings.end()) {
		THROW_EXCEPTION(RuntimeException, "Duplicate building name: " + name + ".\n");
	}
	buildings[name] = building;
}

Zone* Block::GetZone(const string& name) const {
	for (const auto& [zoneName, zonePtr] : zones) {
		if (zoneName == name) {
			return zonePtr;
		}
	}
	return nullptr;
}

Building* Block::GetBuilding(const string& name) const {
	for (const auto& [bldName, bldPtr] : buildings) {
		if (bldName == name) {
			return bldPtr;
		}
	}
	return nullptr;
}

void Block::RemoveZone(const string& name) {
	for (auto it = zones.begin(); it != zones.end(); ) {
		if (it->first == name) {
			it = zones.erase(it);
		}
		else {
			++it;
		}
	}
}

void Block::RemoveBuilding(const string& name) {
	for (auto it = buildings.begin(); it != buildings.end(); ) {
		if (it->first == name) {
			it = buildings.erase(it);
		}
		else {
			++it;
		}
	}
}

void Block::SetAddress(const string& road, int id) {
	address.first = road;
	address.second = id;
}

string Block::GetAddress() const {
	return address.first + " " + to_string(address.second);
}

