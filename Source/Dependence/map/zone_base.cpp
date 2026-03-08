#include "../common/error.h"

#include "zone_base.h"

#include <algorithm>


using namespace std;

Zone::Zone() :
    parentPlot(nullptr),
    stated(false),
    owner(-1),
    buildings() {
    // 全成员默认构造

}

Zone::~Zone() {
    // building在map中统一创建和析构

}

Plot* Zone::GetParent() const {
    // 获取所在地块
    return parentPlot;
}

void Zone::SetParent(Plot* plot) {
    // 设置所在地块
    parentPlot = plot;
}

int Zone::GetOwner() const {
    // 获取私人房东ID
    return owner;
}

void Zone::SetOwner(int owner) {
    // 设置私人房东ID
    this->owner = owner;
}

bool Zone::GetStated() const {
    // 获取是否由政府拥有
    return stated;
}

void Zone::SetStated(bool stated) {
    // 设置是否由政府拥有
    this->stated = stated;
}

Building* Zone::GetBuilding(const string& name) {
    // 获取一栋建筑
    auto it = buildings.find(name);
    if (it != buildings.end()) {
        return it->second;
    }
    return nullptr;
}

unordered_map<string, Building*>& Zone::GetBuildings() {
    // 获取园区内所有建筑
    return buildings;
}

void Zone::AddBuildings(BuildingFactory* factory, const vector<pair<string, float>>& list) {
    // 添加园区内建筑
    if (!factory) {
        THROW_EXCEPTION(InvalidConfigException, "Factory pointer is null.\n");
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
            THROW_EXCEPTION(InvalidConfigException, "Duplicate building name: " + building->GetName() + ".\n");
        }
        buildings[building->GetName()] = building;
    }
}

void Zone::ArrangeBuildings() {
    // 自动分布建筑
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

pair<float, float> Zone::GetPosition() const {
    // 获取世界位置
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
    // 注册构造器和析构器
    registries[id] = { creator, deleter };
    generators[id] = generator;
}

Zone* ZoneFactory::CreateZone(const string& id) {
    // 根据配置构造园区
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        return nullptr;
    }

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first();
    }
    return nullptr;
}

bool ZoneFactory::CheckRegistered(const string& id) {
    // 检查是否注册
    return registries.find(id) != registries.end();
}

void ZoneFactory::SetConfig(const string& name, bool config) {
    // 设置配置
    configs[name] = config;
}

vector<string> ZoneFactory::GetTypes() {
    // 获取所有启用园区
    vector<string> types;
    for (const auto& [name, enabled] : configs) {
        if(enabled)types.push_back(name);
    }
    return types;
}

vector<Zone*> ZoneFactory::CreateZones(const string& type, Plot* plot) {
    // 在地块内生成一类园区
    vector<Zone*> zones;
    auto config = configs.find(type);
    if (config == configs.end() || !config->second) {
        return zones;
    }
    auto genIt = generators.find(type);
    if (genIt == generators.end()) {
        debugf("Generator for zone type %s not found.\n", type.data());
        return zones;
    }
    int num = genIt->second(plot);
    for (int i = 0; i < num; i++) {
        auto zone = CreateZone(type);
        if (zone) {
            zones.push_back(zone);
        }
    }
    return zones;
}

void ZoneFactory::DestroyZone(Zone* zone) {
    // 析构园区
    if (!zone) {
        return;
    }
    auto it = registries.find(zone->GetType());
    if (it != registries.end()) {
        it->second.second(zone);
    }
    else {
        debugf("Deleter not found for %s.\n", zone->GetType().data());
    }
}