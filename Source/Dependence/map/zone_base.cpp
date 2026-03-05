#include "zone_base.h"

#include "../common/error.h"

#include <algorithm>

#undef REPLACEMENT_OPERATOR_NEW_AND_DELETE


using namespace std;

Zone::~Zone() {

}

void Zone::SetParent(Plot* plot) {
    parentPlot = plot;
}

Plot* Zone::GetParent() const {
    return parentPlot;
}

int Zone::GetOwner() const {
    return ownerId;
}

void Zone::SetOwner(int id) {
    ownerId = id;
}

bool Zone::GetStateOwned() const {
    return stateOwned;
}

void Zone::SetStateOwned(bool state) {
    stateOwned = state;
}

Building* Zone::GetBuilding(string name) {
    return buildings[name];
}

unordered_map<string, Building*>& Zone::GetBuildings() {
    return buildings;
}

void Zone::AddBuildings(BuildingFactory* factory, vector<pair<string, float>> list) {
    float acreageTmp = 0.f;
    int attempt = 0;
    for (size_t i = 0; i < list.size(); i++) {
        if (acreageTmp >= GetAcreage() || attempt > 16)break;

        Building* building = factory->CreateBuilding(list[i].first);
        if (!building) {
            attempt++;
            i--;
            continue;
        }

        float acreageBuilding = building->RandomAcreage() * list[i].second;
        float acreageMin = building->GetAcreageMin() * list[i].second;
        float acreageMax = building->GetAcreageMax() * list[i].second;
        if (GetAcreage() - acreageTmp < acreageMin) {
            attempt++;
            i--;
            continue;
        }
        else if (GetAcreage() - acreageTmp < acreageBuilding) {
            acreageBuilding = GetAcreage() - acreageTmp;
        }

        acreageTmp += acreageBuilding;
        building->SetAcreage(acreageBuilding);
        if(buildings.find(building->GetName()) != buildings.end()) {
            THROW_EXCEPTION(InvalidConfigException, "Duplicate building name: " + building->GetName() + ".\n");
		}
        buildings[building->GetName()] = building;
    }
}

void Zone::ArrangeBuildings() {
    if (buildings.empty()) return;

    float acreageTotal = GetAcreage();
    float acreageUsed = 0.f;

    for (const auto& building : buildings) {
        acreageUsed += building.second->GetAcreage();
    }
    float acreageRemain = acreageTotal - acreageUsed;

    bool acreageAllocate = false;
    if (acreageRemain > 0) {
        for (auto& building : buildings) {
            float acreageTmp = building.second->GetAcreage();
            float acreageMax = building.second->GetAcreageMax();
            float acreageMin = building.second->GetAcreageMin();

            float acreageExpand = acreageMax - acreageTmp;

            if (acreageExpand > acreageRemain && acreageRemain > 0) {
                float acreageNew = acreageTmp + acreageRemain;
                if (acreageNew >= acreageMin && acreageNew <= acreageMax) {
                    building.second->SetAcreage(acreageNew);
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
        emptyRect = new Plot();
        emptyRect->SetAcreage(acreageRemain);
        elements.push_back(emptyRect);
    }

    for (const auto& building : buildings) {
        elements.push_back(static_cast<Quad*>(building.second));
    }

    if (elements.empty()) return;

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
            Chunk(Quad* r1, Quad* r2) : r1(r1), r2(r2) { acreage = r1->GetAcreage() + r2->GetAcreage(); }
            Quad *r1, *r2;
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
            if (i < 0)elements[0] = tmp;
        }

        if (container.GetSizeX() > container.GetSizeY()) {
            if (GetRandom(2)) {
                int divX = int(container.GetLeft() +
                    (container.GetRight() - container.GetLeft()) * elements[0]->GetAcreage() / container.GetAcreage());
                if (abs(divX - container.GetLeft()) < 2)divX = (int)container.GetLeft();
                if (abs(divX - container.GetRight()) < 2)divX = (int)container.GetRight();
                elements[0]->SetVertices(container.GetLeft(), container.GetBottom(), (float)divX, container.GetTop());
                elements[1]->SetVertices((float)divX, container.GetBottom(), container.GetRight(), container.GetTop());
            }
            else {
                int divX = int(container.GetLeft() +
                    (container.GetRight() - container.GetLeft()) * elements[1]->GetAcreage() / container.GetAcreage());
                if (abs(divX - container.GetLeft()) < 2)divX = (int)container.GetLeft();
                if (abs(divX - container.GetRight()) < 2)divX = (int)container.GetRight();
                elements[1]->SetVertices(container.GetLeft(), container.GetBottom(), (float)divX, container.GetTop());
                elements[0]->SetVertices((float)divX, container.GetBottom(), container.GetRight(), container.GetTop());
            }
        }
        else {
            if (GetRandom(2)) {
                int divY = int(container.GetBottom() +
                    (container.GetTop() - container.GetBottom()) * elements[0]->GetAcreage() / container.GetAcreage());
                if (abs(divY - container.GetBottom()) < 2)divY = (int)container.GetBottom();
                if (abs(divY - container.GetTop()) < 2)divY = (int)container.GetTop();
                elements[0]->SetVertices(container.GetLeft(), container.GetBottom(), container.GetRight(), (float)divY);
                elements[1]->SetVertices(container.GetLeft(), (float)divY, container.GetRight(), container.GetTop());
            }
            else {
                int divY = int(container.GetBottom() +
                    (container.GetTop() - container.GetBottom()) * elements[1]->GetAcreage() / container.GetAcreage());
                if (abs(divY - container.GetBottom()) < 2)divY = (int)container.GetBottom();
                if (abs(divY - container.GetTop()) < 2)divY = (int)container.GetTop();
                elements[1]->SetVertices(container.GetLeft(), container.GetBottom(), container.GetRight(), (float)divY);
                elements[0]->SetVertices(container.GetLeft(), (float)divY, container.GetRight(), container.GetTop());
            }
        }

        while (elements.size() > 0) {
            auto tmp = elements.back();
            elements.pop_back();
            if (auto chunk = dynamic_cast<Chunk *>(tmp)) {
                Quad* rect1 = chunk->r1;
                Quad* rect2 = chunk->r2;

                if (tmp->GetAcreage() > 0) {
                    if (tmp->GetSizeX() > tmp->GetSizeY()) {
                        if (GetRandom(2)) {
                            int divX = int(tmp->GetLeft() +
                                tmp->GetSizeX() * rect1->GetAcreage() / tmp->GetAcreage());
                            if (abs(divX - tmp->GetLeft()) < 2)divX = (int)tmp->GetLeft();
                            if (abs(divX - tmp->GetRight()) < 2)divX = (int)tmp->GetRight();
                            rect1->SetVertices(tmp->GetLeft(), tmp->GetBottom(), (float)divX, tmp->GetTop());
                            rect2->SetVertices((float)divX, tmp->GetBottom(), tmp->GetRight(), tmp->GetTop());
                        }
                        else {
                            int divX = int(tmp->GetLeft() +
                                tmp->GetSizeX() * rect2->GetAcreage() / tmp->GetAcreage());
                            if (abs(divX - tmp->GetLeft()) < 2)divX = (int)tmp->GetLeft();
                            if (abs(divX - tmp->GetRight()) < 2)divX = (int)tmp->GetRight();
                            rect2->SetVertices(tmp->GetLeft(), tmp->GetBottom(), (float)divX, tmp->GetTop());
                            rect1->SetVertices((float)divX, tmp->GetBottom(), tmp->GetRight(), tmp->GetTop());
                        }
                    }
                    else {
                        if (GetRandom(2)) {
                            int divY = int(tmp->GetBottom() +
                                tmp->GetSizeY() * rect1->GetAcreage() / tmp->GetAcreage());
                            if (abs(divY - tmp->GetBottom()) < 2)divY = (int)tmp->GetBottom();
                            if (abs(divY - tmp->GetTop()) < 2)divY = (int)tmp->GetTop();
                            rect1->SetVertices(tmp->GetLeft(), tmp->GetBottom(), tmp->GetRight(), (float)divY);
                            rect2->SetVertices(tmp->GetLeft(), (float)divY, tmp->GetRight(), tmp->GetTop());
                        }
                        else {
                            int divY = int(tmp->GetBottom() +
                                tmp->GetSizeY() * rect2->GetAcreage() / tmp->GetAcreage());
                            if (abs(divY - tmp->GetBottom()) < 2)divY = (int)tmp->GetBottom();
                            if (abs(divY - tmp->GetTop()) < 2)divY = (int)tmp->GetTop();
                            rect2->SetVertices(tmp->GetLeft(), tmp->GetBottom(), tmp->GetRight(), (float)divY);
                            rect1->SetVertices(tmp->GetLeft(), (float)divY, tmp->GetRight(), tmp->GetTop());
                        }
                    }
                    if (dynamic_cast<Chunk *>(rect1))elements.push_back(rect1);
                    if (dynamic_cast<Chunk *>(rect2))elements.push_back(rect2);
                }

				delete chunk;
            }
        }
    }

    if (emptyRect) {
		delete emptyRect;
    }
}

pair<float, float> Zone::GetPosition() const {
    auto plot = GetParent();
    if (plot) {
        auto center = plot->GetPosition(
            GetPosX() - GetSizeX() / 2.f,
            GetPosY() - GetSizeY() / 2.f);
        return center;
    }

    return { 0.f, 0.f };
}

void ZoneFactory::RegisterZone(const string& id, GeneratorFunc generator,
        function<Zone* ()> creator, function<void(Zone*)> deleter) {
    registries[id] = { creator, deleter };
    generators[id] = generator;
}

Zone* ZoneFactory::CreateZone(const string& id) {
    if(configs.find(id) == configs.end() || !configs.find(id)->second)return nullptr;
    
    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first();
    }
    return nullptr;
}

bool ZoneFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void ZoneFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}

vector<string> ZoneFactory::GetTypes() {
    vector<string> types;
    for (auto config : configs) {
        types.push_back(config.first);
    }
    return types;
}

vector<Zone*> ZoneFactory::CreateZones(string type, Plot* plot) {
    vector<Zone*> zones;
    auto num = generators[type](plot);
    for (int i = 0; i < num; i++) {
        auto zone = CreateZone(type);
        if(zone)zones.push_back(zone);
    }
    return zones;
}

void ZoneFactory::DestroyZone(Zone* zone) {
    if(!zone)return;
    auto it = registries.find(zone->GetType());
    if (it != registries.end()) {
        return it->second.second(zone);
    }
    else{
        debugf(("Deleter not found for " + zone->GetType() + ".\n").data());
    }
}

