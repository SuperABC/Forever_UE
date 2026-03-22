#include "zone.h"


using namespace std;

int EmptyZone::count = 0;

EmptyZone::EmptyZone() : id(count++) {

}

EmptyZone::~EmptyZone() {

}

string EmptyZone::GetId() {
	return "empty";
}

string EmptyZone::GetType() const {
	return "empty";
}

string EmptyZone::GetName() const {
	return "空园区" + to_string(id);
}

void EmptyZone::LayoutZone(Lot* lot) {

}

function<int(Lot*)> EmptyZone::ZoneAssigner = [](Lot* lot) {
	return 0;
};

Zone::Zone(ZoneFactory* factory, string zone) :
	mod(factory->CreateZone(zone)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	parentBlock(nullptr),
	fullAddress(""), 
	stated(false) {

}

Zone::~Zone() {
	factory->DestroyZone(mod);
}

string Zone::GetType() const {
	return type;
}

string Zone::GetName() const {
	return name;
}

void Zone::LayoutZone(Lot* block) {
	mod->LayoutZone(block);
}

// 获取所在地块
Block* Zone::GetParent() const {
    return parentBlock;
}

// 设置所在地块
void Zone::SetParent(Block* block) {
    parentBlock = block;
}

// 获取私人房东ID
//int Zone::GetOwner() const {
//    return owner;
//}

// 设置私人房东ID
//void Zone::SetOwner(int owner) {
//    this->owner = owner;
//}

// 获取是否由政府拥有
bool Zone::GetStated() const {
    return stated;
}

// 设置是否由政府拥有
void Zone::SetStated(bool stated) {
    this->stated = stated;
}

// 获取一栋建筑
//Building* Zone::GetBuilding(const char* name) {
//    auto it = buildings.find(name);
//    if (it != buildings.end()) {
//        return it->second;
//    }
//    return nullptr;
//}

// 获取园区内所有建筑
//const unordered_map<string, Building*>& Zone::GetBuildings() {
//    return buildings;
//}

// 添加园区内建筑
//void Zone::AddBuildings(BuildingFactory* factory, const vector<pair<string, float>>& list) {
//    if (!factory) {
//        THROW_EXCEPTION(RuntimeException, "Building factory is null.\n");
//    }
//
//    float acreageTmp = 0.f;
//    int attempt = 0;
//    for (int i = 0; i < (int)list.size(); i++) {
//        if (acreageTmp >= GetAcreage() || attempt > 16) {
//            break;
//        }
//
//        const auto& [buildingType, ratio] = list[i];
//        Building* building = factory->CreateBuilding(buildingType);
//        if (!building) {
//            attempt++;
//            i--;
//            continue;
//        }
//
//        float acreageBuilding = building->RandomAcreage() * ratio;
//        float acreageMin = building->GetAcreageMin() * ratio;
//        float acreageMax = building->GetAcreageMax() * ratio;
//        if (GetAcreage() - acreageTmp < acreageMin) {
//            attempt++;
//            i--;
//            continue;
//        }
//        else if (GetAcreage() - acreageTmp < acreageBuilding) {
//            acreageBuilding = GetAcreage() - acreageTmp;
//        }
//
//        acreageTmp += acreageBuilding;
//        building->SetAcreage(acreageBuilding);
//        if (buildings.find(building->GetName()) != buildings.end()) {
//            THROW_EXCEPTION(RuntimeException, "Duplicate building name: " + building->GetName() + ".\n");
//        }
//        buildings[building->GetName()] = building;
//    }
//}

// 自动分布建筑
//void Zone::ArrangeBuildings() {
//    if (buildings.empty()) {
//        return;
//    }
//
//    float acreageTotal = GetAcreage();
//    float acreageUsed = 0.f;
//
//    for (const auto& [name, building] : buildings) {
//        if (!building) {
//            debugf("Warning: null building found in zone, skipping.\n");
//            continue;
//        }
//        acreageUsed += building->GetAcreage();
//    }
//    float acreageRemain = acreageTotal - acreageUsed;
//
//    bool acreageAllocate = false;
//    if (acreageRemain > 0) {
//        for (auto& [name, building] : buildings) {
//            if (!building) continue;
//            float acreageTmp = building->GetAcreage();
//            float acreageMax = building->GetAcreageMax();
//            float acreageMin = building->GetAcreageMin();
//
//            float acreageExpand = acreageMax - acreageTmp;
//
//            if (acreageExpand > acreageRemain && acreageRemain > 0) {
//                float acreageNew = acreageTmp + acreageRemain;
//                if (acreageNew >= acreageMin && acreageNew <= acreageMax) {
//                    building->SetAcreage(acreageNew);
//                    acreageUsed += acreageRemain;
//                    acreageRemain = 0.f;
//                    acreageAllocate = true;
//                    break;
//                }
//            }
//        }
//    }
//
//    vector<Quad*> elements;
//    Quad* emptyRect = nullptr;
//    if (acreageRemain > 0 && !acreageAllocate) {
//        emptyRect = new Block();
//        emptyRect->SetAcreage(acreageRemain);
//        elements.push_back(emptyRect);
//    }
//
//    for (const auto& [name, building] : buildings) {
//        if (building) {
//            elements.push_back(static_cast<Quad*>(building));
//        }
//    }
//
//    if (elements.empty()) {
//        return;
//    }
//
//    sort(elements.begin(), elements.end(), [](Quad* a, Quad* b) {
//        return a->GetAcreage() > b->GetAcreage();
//        });
//
//    Quad container = Quad(GetSizeX() / 2, GetSizeY() / 2, GetSizeX(), GetSizeY());
//    if (elements.size() == 1) {
//        elements[0]->SetPosition(container.GetPosX(), container.GetPosY(), container.GetSizeX(), container.GetSizeY());
//    }
//    else {
//        class Chunk : public Quad {
//        public:
//            Chunk(Quad* r1, Quad* r2) : r1(r1), r2(r2) {
//                acreage = r1->GetAcreage() + r2->GetAcreage();
//            }
//            Quad* r1, * r2;
//        };
//        while (elements.size() > 2) {
//            Chunk* tmp = new Chunk(elements[elements.size() - 1], elements[elements.size() - 2]);
//            elements.pop_back();
//            int i = (int)elements.size() - 2;
//            for (; i >= 0; i--) {
//                if (tmp->GetAcreage() > elements[i]->GetAcreage()) {
//                    elements[i + 1] = elements[i];
//                }
//                else {
//                    elements[i + 1] = tmp;
//                    break;
//                }
//            }
//            if (i < 0) {
//                elements[0] = tmp;
//            }
//        }
//
//        if (container.GetSizeX() > container.GetSizeY()) {
//            if (GetRandom(2)) {
//                int divX = int(container.GetLeft() +
//                    (container.GetRight() - container.GetLeft()) * elements[0]->GetAcreage() / container.GetAcreage());
//                if (abs(divX - container.GetLeft()) < 2) divX = (int)container.GetLeft();
//                if (abs(divX - container.GetRight()) < 2) divX = (int)container.GetRight();
//                elements[0]->SetVertices(container.GetLeft(), container.GetBottom(), (float)divX, container.GetTop());
//                elements[1]->SetVertices((float)divX, container.GetBottom(), container.GetRight(), container.GetTop());
//            }
//            else {
//                int divX = int(container.GetLeft() +
//                    (container.GetRight() - container.GetLeft()) * elements[1]->GetAcreage() / container.GetAcreage());
//                if (abs(divX - container.GetLeft()) < 2) divX = (int)container.GetLeft();
//                if (abs(divX - container.GetRight()) < 2) divX = (int)container.GetRight();
//                elements[1]->SetVertices(container.GetLeft(), container.GetBottom(), (float)divX, container.GetTop());
//                elements[0]->SetVertices((float)divX, container.GetBottom(), container.GetRight(), container.GetTop());
//            }
//        }
//        else {
//            if (GetRandom(2)) {
//                int divY = int(container.GetBottom() +
//                    (container.GetTop() - container.GetBottom()) * elements[0]->GetAcreage() / container.GetAcreage());
//                if (abs(divY - container.GetBottom()) < 2) divY = (int)container.GetBottom();
//                if (abs(divY - container.GetTop()) < 2) divY = (int)container.GetTop();
//                elements[0]->SetVertices(container.GetLeft(), container.GetBottom(), container.GetRight(), (float)divY);
//                elements[1]->SetVertices(container.GetLeft(), (float)divY, container.GetRight(), container.GetTop());
//            }
//            else {
//                int divY = int(container.GetBottom() +
//                    (container.GetTop() - container.GetBottom()) * elements[1]->GetAcreage() / container.GetAcreage());
//                if (abs(divY - container.GetBottom()) < 2) divY = (int)container.GetBottom();
//                if (abs(divY - container.GetTop()) < 2) divY = (int)container.GetTop();
//                elements[1]->SetVertices(container.GetLeft(), container.GetBottom(), container.GetRight(), (float)divY);
//                elements[0]->SetVertices(container.GetLeft(), (float)divY, container.GetRight(), container.GetTop());
//            }
//        }
//
//        while (!elements.empty()) {
//            auto tmp = elements.back();
//            elements.pop_back();
//            if (auto chunk = dynamic_cast<Chunk*>(tmp)) {
//                Quad* rect1 = chunk->r1;
//                Quad* rect2 = chunk->r2;
//
//                if (tmp->GetAcreage() > 0) {
//                    if (tmp->GetSizeX() > tmp->GetSizeY()) {
//                        if (GetRandom(2)) {
//                            int divX = int(tmp->GetLeft() +
//                                tmp->GetSizeX() * rect1->GetAcreage() / tmp->GetAcreage());
//                            if (abs(divX - tmp->GetLeft()) < 2) divX = (int)tmp->GetLeft();
//                            if (abs(divX - tmp->GetRight()) < 2) divX = (int)tmp->GetRight();
//                            rect1->SetVertices(tmp->GetLeft(), tmp->GetBottom(), (float)divX, tmp->GetTop());
//                            rect2->SetVertices((float)divX, tmp->GetBottom(), tmp->GetRight(), tmp->GetTop());
//                        }
//                        else {
//                            int divX = int(tmp->GetLeft() +
//                                tmp->GetSizeX() * rect2->GetAcreage() / tmp->GetAcreage());
//                            if (abs(divX - tmp->GetLeft()) < 2) divX = (int)tmp->GetLeft();
//                            if (abs(divX - tmp->GetRight()) < 2) divX = (int)tmp->GetRight();
//                            rect2->SetVertices(tmp->GetLeft(), tmp->GetBottom(), (float)divX, tmp->GetTop());
//                            rect1->SetVertices((float)divX, tmp->GetBottom(), tmp->GetRight(), tmp->GetTop());
//                        }
//                    }
//                    else {
//                        if (GetRandom(2)) {
//                            int divY = int(tmp->GetBottom() +
//                                tmp->GetSizeY() * rect1->GetAcreage() / tmp->GetAcreage());
//                            if (abs(divY - tmp->GetBottom()) < 2) divY = (int)tmp->GetBottom();
//                            if (abs(divY - tmp->GetTop()) < 2) divY = (int)tmp->GetTop();
//                            rect1->SetVertices(tmp->GetLeft(), tmp->GetBottom(), tmp->GetRight(), (float)divY);
//                            rect2->SetVertices(tmp->GetLeft(), (float)divY, tmp->GetRight(), tmp->GetTop());
//                        }
//                        else {
//                            int divY = int(tmp->GetBottom() +
//                                tmp->GetSizeY() * rect2->GetAcreage() / tmp->GetAcreage());
//                            if (abs(divY - tmp->GetBottom()) < 2) divY = (int)tmp->GetBottom();
//                            if (abs(divY - tmp->GetTop()) < 2) divY = (int)tmp->GetTop();
//                            rect2->SetVertices(tmp->GetLeft(), tmp->GetBottom(), tmp->GetRight(), (float)divY);
//                            rect1->SetVertices(tmp->GetLeft(), (float)divY, tmp->GetRight(), tmp->GetTop());
//                        }
//                    }
//                    if (dynamic_cast<Chunk*>(rect1)) {
//                        elements.push_back(rect1);
//                    }
//                    if (dynamic_cast<Chunk*>(rect2)) {
//                        elements.push_back(rect2);
//                    }
//                }
//
//                delete chunk;
//            }
//        }
//    }
//
//    if (emptyRect) {
//        delete emptyRect;
//    }
//}

// 获取园区中心世界位置
void Zone::GetPosition(float& x, float& y) const {
    auto block = GetParent();
    if (block) {
        auto center = block->GetPosition(mod->GetPosX(), mod->GetPosY());
        x = center.first;
        y = center.second;
    }
}

// 获取完整地址
const char* Zone::GetAddress() {
    if (fullAddress != "") {
        return fullAddress.data();
    }

    auto blockAddress = GetParent()->GetAddress();
    fullAddress = blockAddress + " " + GetName();
    return fullAddress.data();
}