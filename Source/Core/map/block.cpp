#include "block.h"

#include "map/zone.h"
#include "map/building.h"
#include "map/geometry.h"

#include <cmath>
#include <utility>
#include <algorithm>


using namespace std;

Block::Block() :
	Lot(),
	roads(),
	intersections(),
	nodes(),
	boundary(),
	address(),
	zones(),
	buildings() {

}

Block::Block(Lot lot) :
	Lot(lot),
	roads(),
	intersections(),
	nodes(),
	boundary(),
	address(),
	zones(),
	buildings() {

}

Block::~Block() {
	for(auto road : roads) {
		delete road;
	}
	roads.clear();
	for(auto intersection : intersections) {
		delete intersection;
	}
	intersections.clear();
	for (auto node : nodes) {
		delete node;
	}
	nodes.clear();
	for (auto &[_, zone] : zones) {
		delete zone;
	}
	zones.clear();
	for (auto &[_, building] : buildings) {
		delete building;
	}
	buildings.clear();
}

const vector<Road*>& Block::GetRoads() const {
	return roads;
}

void Block::SetRoads(const vector<Road*>& r) {
	roads = r;
}

const vector<Intersection*>& Block::GetIntersections() const {
	return intersections;
}

void Block::SetIntersections(const vector<Intersection*>& i) {
	intersections = i;
}

const vector<Node*>& Block::GetNodes() const {
	return nodes;
}

const QuadBoundary& Block::GetBoundary() const {
	return boundary;
}

void Block::AddNodes(const vector<Node*>& newNodes) {
	nodes.insert(nodes.end(), newNodes.begin(), newNodes.end());
}

void Block::SetBoundary(const QuadBoundary& boundary) {
	this->boundary = boundary;
}

unordered_map<string, Zone*>& Block::GetZones() {
	return zones;
}

unordered_map<string, Building*>& Block::GetBuildings() {
	return buildings;
}

void Block::AddZone(const string& name, Zone* zone) {
	if (zone == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Zone is null when adding to block.\n");
	}
	if (zones.find(name) != zones.end()) {
		THROW_EXCEPTION(RuntimeException, "Duplicate zone name: " + name + ".\n");
	}
	zones[name] = zone;
}

void Block::AddBuilding(const string& name, Building* building) {
	if (building == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Building is null when adding to block.\n");
	}
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

