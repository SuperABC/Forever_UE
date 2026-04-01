#include "common/config.h"

#include "map.h"
#include "zone.h"

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
		if (!building) {
			attempt++;
			i--;
			continue;
		}

		float acreageBuilding = building->RandomAcreage() * ratio;
		float acreageMin = building->GetAcreageMin() * ratio;
		float acreageMax = building->GetAcreageMax() * ratio;
		if (mod->acreage - acreageTmp < acreageMin) {
			attempt++;
			i--;
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

	for (const auto& [name, building] : buildings) {
		if (!building) {
			debugf("Warning: null building found in zone, skipping.\n");
			continue;
		}
		acreageUsed += building->GetAcreage();
	}
	float acreageRemain = acreageTotal - acreageUsed;

	bool acreageAllocate = false;
	if (acreageRemain > 0) {
		for (auto& [name, building] : buildings) {
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

	for (const auto& [name, building] : buildings) {
		if (building) {
			elements.push_back(building);
		}
	}

	if (elements.empty()) {
		return;
	}

	sort(elements.begin(), elements.end(), [](Quad* a, Quad* b) {
		return a->GetAcreage() > b->GetAcreage();
		});

	Quad container = Quad(GetSizeX() / 2, GetSizeY() / 2, GetSizeX(), GetSizeY());
	if (elements.size() == 1) {
		elements[0]->SetPosition(container.GetPosX(), container.GetPosY(), container.GetSizeX(), container.GetSizeY());
	}
	else {
		class Chunk : public Quad {
		public:
			Chunk(Quad* r1, Quad* r2) : r1(r1), r2(r2) {
				acreage = r1->GetAcreage() + r2->GetAcreage();
			}
			Quad* r1, * r2;
		};
		while (elements.size() > 2) {
			Chunk* tmp = new Chunk(elements[elements.size() - 1], elements[elements.size() - 2]);
			elements.pop_back();
			int i = (int)elements.size() - 2;
			for (; i >= 0; i--) {
				if (tmp->GetAcreage() > elements[i]->GetAcreage()) {
					elements[i + 1] = elements[i];
				}
				else {
					elements[i + 1] = tmp;
					break;
				}
			}
			if (i < 0) {
				elements[0] = tmp;
			}
		}

		if (container.GetSizeX() > container.GetSizeY()) {
			if (GetRandom(2)) {
				int divX = int(container.GetLeft() +
					(container.GetRight() - container.GetLeft()) * elements[0]->GetAcreage() / container.GetAcreage());
				if (abs(divX - container.GetLeft()) < 2) divX = (int)container.GetLeft();
				if (abs(divX - container.GetRight()) < 2) divX = (int)container.GetRight();
				elements[0]->SetVertices(container.GetLeft(), container.GetBottom(), (float)divX, container.GetTop());
				elements[1]->SetVertices((float)divX, container.GetBottom(), container.GetRight(), container.GetTop());
			}
			else {
				int divX = int(container.GetLeft() +
					(container.GetRight() - container.GetLeft()) * elements[1]->GetAcreage() / container.GetAcreage());
				if (abs(divX - container.GetLeft()) < 2) divX = (int)container.GetLeft();
				if (abs(divX - container.GetRight()) < 2) divX = (int)container.GetRight();
				elements[1]->SetVertices(container.GetLeft(), container.GetBottom(), (float)divX, container.GetTop());
				elements[0]->SetVertices((float)divX, container.GetBottom(), container.GetRight(), container.GetTop());
			}
		}
		else {
			if (GetRandom(2)) {
				int divY = int(container.GetBottom() +
					(container.GetTop() - container.GetBottom()) * elements[0]->GetAcreage() / container.GetAcreage());
				if (abs(divY - container.GetBottom()) < 2) divY = (int)container.GetBottom();
				if (abs(divY - container.GetTop()) < 2) divY = (int)container.GetTop();
				elements[0]->SetVertices(container.GetLeft(), container.GetBottom(), container.GetRight(), (float)divY);
				elements[1]->SetVertices(container.GetLeft(), (float)divY, container.GetRight(), container.GetTop());
			}
			else {
				int divY = int(container.GetBottom() +
					(container.GetTop() - container.GetBottom()) * elements[1]->GetAcreage() / container.GetAcreage());
				if (abs(divY - container.GetBottom()) < 2) divY = (int)container.GetBottom();
				if (abs(divY - container.GetTop()) < 2) divY = (int)container.GetTop();
				elements[1]->SetVertices(container.GetLeft(), container.GetBottom(), container.GetRight(), (float)divY);
				elements[0]->SetVertices(container.GetLeft(), (float)divY, container.GetRight(), container.GetTop());
			}
		}

		while (!elements.empty()) {
			auto tmp = elements.back();
			elements.pop_back();
			if (auto chunk = dynamic_cast<Chunk*>(tmp)) {
				Quad* rect1 = chunk->r1;
				Quad* rect2 = chunk->r2;

				if (tmp->GetAcreage() > 0) {
					if (tmp->GetSizeX() > tmp->GetSizeY()) {
						if (GetRandom(2)) {
							int divX = int(tmp->GetLeft() +
								tmp->GetSizeX() * rect1->GetAcreage() / tmp->GetAcreage());
							if (abs(divX - tmp->GetLeft()) < 2) divX = (int)tmp->GetLeft();
							if (abs(divX - tmp->GetRight()) < 2) divX = (int)tmp->GetRight();
							rect1->SetVertices(tmp->GetLeft(), tmp->GetBottom(), (float)divX, tmp->GetTop());
							rect2->SetVertices((float)divX, tmp->GetBottom(), tmp->GetRight(), tmp->GetTop());
						}
						else {
							int divX = int(tmp->GetLeft() +
								tmp->GetSizeX() * rect2->GetAcreage() / tmp->GetAcreage());
							if (abs(divX - tmp->GetLeft()) < 2) divX = (int)tmp->GetLeft();
							if (abs(divX - tmp->GetRight()) < 2) divX = (int)tmp->GetRight();
							rect2->SetVertices(tmp->GetLeft(), tmp->GetBottom(), (float)divX, tmp->GetTop());
							rect1->SetVertices((float)divX, tmp->GetBottom(), tmp->GetRight(), tmp->GetTop());
						}
					}
					else {
						if (GetRandom(2)) {
							int divY = int(tmp->GetBottom() +
								tmp->GetSizeY() * rect1->GetAcreage() / tmp->GetAcreage());
							if (abs(divY - tmp->GetBottom()) < 2) divY = (int)tmp->GetBottom();
							if (abs(divY - tmp->GetTop()) < 2) divY = (int)tmp->GetTop();
							rect1->SetVertices(tmp->GetLeft(), tmp->GetBottom(), tmp->GetRight(), (float)divY);
							rect2->SetVertices(tmp->GetLeft(), (float)divY, tmp->GetRight(), tmp->GetTop());
						}
						else {
							int divY = int(tmp->GetBottom() +
								tmp->GetSizeY() * rect2->GetAcreage() / tmp->GetAcreage());
							if (abs(divY - tmp->GetBottom()) < 2) divY = (int)tmp->GetBottom();
							if (abs(divY - tmp->GetTop()) < 2) divY = (int)tmp->GetTop();
							rect2->SetVertices(tmp->GetLeft(), tmp->GetBottom(), tmp->GetRight(), (float)divY);
							rect1->SetVertices(tmp->GetLeft(), (float)divY, tmp->GetRight(), tmp->GetTop());
						}
					}
					if (dynamic_cast<Chunk*>(rect1)) {
						elements.push_back(rect1);
					}
					if (dynamic_cast<Chunk*>(rect2)) {
						elements.push_back(rect2);
					}
				}

				delete chunk;
			}
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
