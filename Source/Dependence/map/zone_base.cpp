#include "../common/error.h"
#include "../common/utility.h"

#include "zone_base.h"

#include <algorithm>


using namespace std;

// 全成员默认构造
Zone::Zone() :
    parentPlot(nullptr),
    stated(false),
    owner(-1),
    buildings() {

}

// building在map中统一创建和析构
Zone::~Zone() {

}

// 获取所在地块
Plot* Zone::GetParent() const {
    return parentPlot;
}

// 设置所在地块
void Zone::SetParent(Plot* plot) {
    parentPlot = plot;
}

// 获取私人房东ID
int Zone::GetOwner() const {
    return owner;
}

// 设置私人房东ID
void Zone::SetOwner(int owner) {
    this->owner = owner;
}

// 获取是否由政府拥有
bool Zone::GetStated() const {
    return stated;
}

// 设置是否由政府拥有
void Zone::SetStated(bool stated) {
    this->stated = stated;
}

// 获取一栋建筑
Building* Zone::GetBuilding(const string& name) {
    auto it = buildings.find(name);
    if (it != buildings.end()) {
        return it->second;
    }
    return nullptr;
}

// 获取园区内所有建筑
unordered_map<string, Building*>& Zone::GetBuildings() {
    return buildings;
}

// 添加园区内建筑
void Zone::AddBuildings(BuildingFactory* factory, const vector<pair<string, float>>& list) {
    if (!factory) {
        THROW_EXCEPTION(RuntimeException, "Building factory is null.\n");
    }

    float acreageTmp = 0.f;
    int attempt = 0;
    for (int i = 0; i < (int)list.size(); i++) {
        if (acreageTmp >= GetAcreage() || attempt > 16) {
            break;
        }

        const auto& [buildingType, ratio] = list[i];
        Building* building = factory->CreateBuilding(buildingType);
        if (!building) {
            attempt++;
            i--;
            continue;
        }

        float acreageBuilding = building->RandomAcreage() * ratio;
        float acreageMin = building->GetAcreageMin() * ratio;
        float acreageMax = building->GetAcreageMax() * ratio;
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
        if (buildings.find(building->GetName()) != buildings.end()) {
            THROW_EXCEPTION(RuntimeException, "Duplicate building name: " + building->GetName() + ".\n");
        }
        buildings[building->GetName()] = building;
    }
}

// 自动分布建筑
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
        emptyRect = new Plot();
        emptyRect->SetAcreage(acreageRemain);
        elements.push_back(emptyRect);
    }

    for (const auto& [name, building] : buildings) {
        if (building) {
            elements.push_back(static_cast<Quad*>(building));
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

// 获取园区中心世界位置
pair<float, float> Zone::GetPosition() const {
    auto plot = GetParent();
    if (plot) {
        auto center = plot->GetPosition(GetPosX(), GetPosY());
        return center;
    }

    return { 0.f, 0.f };
}

// 获取完整地址
string Zone::GetAddress() const {
    auto plotAddress = GetParent()->GetAddress();
    return plotAddress + " " + GetName();
}

// 注册园区
void ZoneFactory::RegisterZone(const string& id, GeneratorFunc generator,
    function<Zone* ()> creator, function<void(Zone*)> deleter) {
    registries[id] = { creator, deleter };
    generators[id] = generator;
}

// 创建园区
Zone* ZoneFactory::CreateZone(const string& id) const {
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Zone %s not enabled when creating.\n", id.data());
        return nullptr;
    }

    auto it = registries.find(id);
    if (it == registries.end()) {
        debugf("Warning: Zone %s not registered when creating.\n", id.data());
        return nullptr;
    }

    if (it->second.first) {
        return it->second.first();
    } else {
        THROW_EXCEPTION(NullPointerException, "Zone " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool ZoneFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void ZoneFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 获取所有启用园区
vector<string> ZoneFactory::GetTypes() const {
    vector<string> types;
    for (const auto& [name, enabled] : configs) {
        if (enabled) types.push_back(name);
    }
    return types;
}

// 在地块内生成一类园区
vector<Zone*> ZoneFactory::CreateZones(const string& type, Plot* plot) const {
    vector<Zone*> zones;
    auto config = configs.find(type);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Zone %s not enabled when creating.\n", type.data());
        return zones;
    }

    auto genIt = generators.find(type);
    if (genIt == generators.end()) {
        debugf("Warning: Generator for zone %s not found when creating.\n", type.data());
        return zones;
    }

    int num = genIt->second(plot);
    for (int i = 0; i < num; ++i) {
        Zone* zone = CreateZone(type);
        if (zone) {
            zones.push_back(zone);
        }
    }
    return zones;
}

// 析构园区
void ZoneFactory::DestroyZone(Zone* zone) const {
    if (!zone) {
        debugf("Warning: Zone is null when deleting.\n");
        return;
    }

    auto it = registries.find(zone->GetType());
    if (it == registries.end()) {
        debugf("Warning: Zone %s not registered when deleting.\n", zone->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(zone);
    } else {
        THROW_EXCEPTION(NullPointerException, "Zone " + zone->GetType() + " deleter is null.\n");
    }
}

