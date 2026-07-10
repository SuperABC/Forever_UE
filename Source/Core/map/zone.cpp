#include "zone.h"

#include "map/map.h"
#include "map/block.h"
#include "map/building.h"
#include "populace/person.h"
#include "story/story.h"
#include "story/script.h"

#include <algorithm>


using namespace std;

Zone::Zone(ZoneFactory* factory, const string& zone) :
	mod(factory->CreateZone(zone)),
	factory(factory),
	type(),
	name(),
	parentBlock(nullptr),
	address(""),
	pivots(),
	nodes(),
	boundary(),
	buildings(),
	script(nullptr) {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Zone " + zone + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Zone::~Zone() {
	factory->DestroyZone(mod);

	for (auto& pivot : pivots) {
		delete pivot;
	}
	pivots.clear();

	for (auto node : nodes) {
		delete node;
	}
	nodes.clear();

	for (auto &[_, building] : buildings) {
		delete building;
	}
	buildings.clear();

	if (script) {
		delete script;
	}
	script = nullptr;
}

string Zone::GetType() const {
	return type;
}

string Zone::GetName() const {
	return name;
}

Block* Zone::GetParent() const {
	return parentBlock;
}

void Zone::SetParent(Block* block) {
	parentBlock = block;
}

Building* Zone::GetBuilding(const string& name) {
	auto it = buildings.find(name);
	if (it != buildings.end()) {
		return it->second;
	}
	return nullptr;
}

const unordered_map<string, Building*>& Zone::GetBuildings() {
	return buildings;
}

string Zone::GetAddress() {
	if (address != "") {
		return address.data();
	}

	auto block = GetParent();
	if (!block) {
		THROW_EXCEPTION(NullPointerException, "Zone's parent block is null.\n");
	}
	auto blockAddress = block->GetAddress();
	address = blockAddress + " " + GetName();
	return address.data();
}

const vector<Node*> Zone::GetPivots() {
	return pivots;
}

const vector<Node*>& Zone::GetNodes() const {
	return nodes;
}

const QuadBoundary& Zone::GetBoundary() const {
	return boundary;
}

void Zone::AddNodes(const vector<Node*>& newNodes) {
	nodes.insert(nodes.end(), newNodes.begin(), newNodes.end());
}

void Zone::SetBoundary(const QuadBoundary& boundary) {
	this->boundary = boundary;
}

bool Zone::GetStated() const {
	return stated;
}

void Zone::SetStated(bool stated) {
	this->stated = stated;
}

Person* Zone::GetOwner() const {
	return owner;
}

void Zone::SetOwner(Person* owner) {
	this->owner = owner;
}

Script* Zone::GetScript() const {
	return script;
}

pair<float, float> Zone::GetPosition() const {
	auto block = GetParent();
	if (!block) {
		THROW_EXCEPTION(NullPointerException, "Zone's parent block is null.\n");
	}
	return block->GetPosition(GetPosX(), GetPosY());
}

pair<float, float> Zone::GetPosition(float x, float y) const {
	auto block = GetParent();
	if (!block) {
		THROW_EXCEPTION(NullPointerException, "Zone's parent block is null.\n");
	}
	return block->GetPosition(GetPosX() - GetSizeX() / 2.f + x, GetPosY() - GetSizeY() / 2.f + y);
}

void Zone::LayoutZone(const Lot* block) {
	mod->LayoutZone(block);
	SetAcreage(mod->acreage);

	float acreageTemporary = 0.f;
	int attempt = 0;
	for (int i = 0; i < static_cast<int>(mod->buildings.size()); i++) {
		if (acreageTemporary >= mod->acreage || attempt > MAX_ALLOCATION_ATTEMPTS) {
			break;
		}

		const auto& [buildingType, ratio] = mod->buildings[i];
		Building* building = new Building(Map::buildingFactory, buildingType);

		float acreageBuilding = building->RandomAcreage() * ratio;
		float acreageMin = building->GetAcreageMin() * ratio;
		float acreageMax = building->GetAcreageMax() * ratio;
		if (mod->acreage - acreageTemporary < acreageMin) {
			attempt++;
			i--;
			delete building;
			continue;
		}
		else if (mod->acreage - acreageTemporary < acreageBuilding) {
			acreageBuilding = mod->acreage - acreageTemporary;
		}

		acreageTemporary += acreageBuilding;
		building->SetAcreage(acreageBuilding);
		if (buildings.find(building->GetName()) != buildings.end()) {
			THROW_EXCEPTION(RuntimeException, "Duplicate building name: " + building->GetName() + ".\n");
		}
		buildings[building->GetName()] = building;
	}

	script = new Script(Story::scriptFactory, mod->script.first);
	for (auto s : mod->script.second) {
		script->ReadMilestones(Config::GetScript(s));
	}
	script->SetValue("self.name", name);
}

void Zone::ArrangeBuildings(Map* map) {
	if (buildings.empty()) {
		return;
	}

	float acreageTotal = GetAcreage();
	float acreageUsed = 0.f;

	for (const auto& [_, building] : buildings) {
		if (!building) {
			debugf("Warning: null building found in zone, skipping.\n");
			continue;
		}
		acreageUsed += building->GetAcreage();
	}
	float acreageRemain = acreageTotal - acreageUsed;

	bool acreageAllocate = false;
	if (acreageRemain > 0) {
		for (auto& [_, building] : buildings) {
			if (!building) continue;
			float acreageTemporary = building->GetAcreage();
			float acreageMax = building->GetAcreageMax();
			float acreageMin = building->GetAcreageMin();

			float acreageExpand = acreageMax - acreageTemporary;

			if (acreageExpand > acreageRemain && acreageRemain > 0) {
				float acreageNew = acreageTemporary + acreageRemain;
				if (acreageNew >= acreageMin && acreageNew <= acreageMax) {
					building->SetAcreage(acreageNew);
					acreageUsed += acreageRemain;
					acreageRemain = 0.f;
					acreageAllocate = true;
					break;
				}
			}
		}
	}

	vector<Quad*> elements;
	Quad* emptyRect = nullptr;
	if (acreageRemain > 0 && !acreageAllocate) {
		emptyRect = new Block();
		emptyRect->SetAcreage(acreageRemain);
		elements.push_back(emptyRect);
	}

	for (const auto& [_, building] : buildings) {
		if (building) {
			elements.push_back(building);
		}
	}

	if (elements.empty()) {
		return;
	}
	if (!parentBlock) {
		THROW_EXCEPTION(NullPointerException, "Zone's parent block is null.\n");
	}

	Quad container(GetSizeX() / 2, GetSizeY() / 2, GetSizeX(), GetSizeY());

	auto toWorld = [this](float x, float y) -> pair<float, float> {
		return parentBlock->GetPosition(GetPosX() - GetSizeX() / 2.f + x, GetPosY() - GetSizeY() / 2.f + y);
		};
	vector<Node*> newNodes;
	vector<Connection*> newConnections, removedConnections;
	unordered_map<Quad*, QuadBoundary> elementBoundaries;
	container.DivideSpace(elements, boundary, toWorld, "zone", newNodes, newConnections, removedConnections, elementBoundaries);

	AddNodes(newNodes);
	if (map) {
		map->ApplyDivideResult(newNodes, newConnections, removedConnections);
	}

	// 自身的边可能被本次划分切断，置空失效边，避免持有悬空指针
	boundary.Invalidate(removedConnections);

	for (auto& [_, building] : buildings) {
		if (!building) continue;
		auto it = elementBoundaries.find(building);
		if (it != elementBoundaries.end()) {
			building->SetBoundary(it->second);
		}
	}

	if (emptyRect) {
		delete emptyRect;
	}
}

void Zone::ClearZero() {
	for (auto it = buildings.begin(); it != buildings.end(); ) {
		Building* building = it->second;
		if (building != nullptr && building->GetAcreage() == 0) {
			delete building;
			it = buildings.erase(it);
		}
		else {
			++it;
		}
	}
}

void Zone::PlacePivots(Quad* zone) {
	mod->PlacePivots(zone);

	for (auto& pivot : mod->pivots) {
		float x = pivot[0] * zone->GetSizeX() + pivot[1];
		float y = pivot[2] * zone->GetSizeY() + pivot[3];
		auto [wx, wy] = GetPosition(x, y);
		pivots.push_back(new Node("zone", wx, wy));
	}
}

int EmptyZone::count = 0;

EmptyZone::EmptyZone() : id(count++) {

}

EmptyZone::~EmptyZone() {

}

const char* EmptyZone::GetId() {
	return "empty";
}

const char* EmptyZone::GetType() const {
	return "empty";
}

const char* EmptyZone::GetName() {
	name = "空园区" + to_string(id);
	return name.data();
}

void EmptyZone::LayoutZone(const Lot* lot) {

}

void EmptyZone::PlacePivots(Quad* zone) {

}

function<int(const Lot*)> EmptyZone::ZoneAssigner = [](const Lot* lot) -> int {
	return 0;
	};
