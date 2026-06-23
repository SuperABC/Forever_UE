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
	type(mod->GetType()),
	name(mod->GetName()),
	parentBlock(nullptr),
	address(""),
	buildings(),
	script(nullptr) {

}

Zone::~Zone() {
	factory->DestroyZone(mod);

	for (auto &[_, building] : buildings) {
		delete building;
	}
	buildings.clear();

	if(script)delete script;
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

	auto blockAddress = GetParent()->GetAddress();
	address = blockAddress + " " + GetName();
	return address.data();
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

void Zone::GetPosition(float& x, float& y) const {
	auto block = GetParent();
	if (block) {
		auto center = block->GetPosition(GetPosX(), GetPosY());
		x = center.first;
		y = center.second;
	}
}

void Zone::LayoutZone(const Lot* block) {
	mod->LayoutZone(block);
	SetAcreage(mod->acreage);

	float acreageTmp = 0.f;
	int attempt = 0;
	for (int i = 0; i < (int)mod->buildings.size(); i++) {
		if (acreageTmp >= mod->acreage || attempt > 16) {
			break;
		}

		const auto& [buildingType, ratio] = mod->buildings[i];
		Building* building = new Building(Map::buildingFactory, buildingType);

		float acreageBuilding = building->RandomAcreage() * ratio;
		float acreageMin = building->GetAcreageMin() * ratio;
		float acreageMax = building->GetAcreageMax() * ratio;
		if (mod->acreage - acreageTmp < acreageMin) {
			attempt++;
			i--;
			delete building;
			continue;
		}
		else if (mod->acreage - acreageTmp < acreageBuilding) {
			acreageBuilding = mod->acreage - acreageTmp;
		}

		acreageTmp += acreageBuilding;
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

void Zone::ArrangeBuildings() {
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
			float acreageTmp = building->GetAcreage();
			float acreageMax = building->GetAcreageMax();
			float acreageMin = building->GetAcreageMin();

			float acreageExpand = acreageMax - acreageTmp;

			if (acreageExpand > acreageRemain && acreageRemain > 0) {
				float acreageNew = acreageTmp + acreageRemain;
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

	Quad container(GetSizeX() / 2, GetSizeY() / 2, GetSizeX(), GetSizeY());
	container.DivideSpace(elements);

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

function<int(const Lot*)> EmptyZone::ZoneAssigner = [](const Lot* lot) -> int {
	return 0;
	};
