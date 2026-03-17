#include "../common/error.h"
#include "../common/utility.h"

#include "route_base.h"


using namespace std;

BezierCurve::BezierCurve(string station1, int slot1, glm::vec3 pos1, glm::vec3 dir1,
                         string station2, int slot2, glm::vec3 pos2, glm::vec3 dir2) {
    vertices = {station1, station2};
    slots = {slot1, slot2};
    controlPoints.push_back({pos1, dir1});
    controlPoints.push_back({pos2, dir2});
}

BezierCurve::~BezierCurve() {
}

// 全成员默认构造
Route::Route() :
    stations(),
    connections(){

}

// Station在traffic中统一创建和析构
Route::~Route() {
    for (auto connection : connections) {
        delete connection;
    }
}

// 获取路线连接图
vector<BezierCurve*> Route::GetConnections() {
    return connections;
}

// 路线图中加入车站
bool Route::AddStation(string connectStation, int connectSlot, Station* newStation, int newSlot, string name) {
    if (stations.find(name) != stations.end()) return false;
    if (stations.find(connectStation) == stations.end()) return false;

    if (connectSlot < 0 || connectSlot >= stations[connectStation]->GetSlots().size()) return false;
    if (newSlot < 0 || newSlot >= newStation->GetSlots().size()) return false;

    stations[name] = newStation;
    if (connectStation.size() == 0) {
        return true;
    }

    connections.push_back(new BezierCurve(connectStation, connectSlot,
        stations[connectStation]->GetSlot(connectSlot).first + glm::vec3(stations[connectStation]->GetPosX(), stations[connectStation]->GetPosY(), 0.f),
        stations[connectStation]->GetSlot(connectSlot).second,
        name, newSlot,
        newStation->GetSlot(newSlot).first + glm::vec3(newStation->GetPosX(), newStation->GetPosY(), 0.f),
        newStation->GetSlot(newSlot).second));
    return true;
}

// 连接两个已有车站
bool Route::AddConnection(string connect1, int slot1, string connect2, int slot2) {
    if (stations.find(connect1) == stations.end() || stations.find(connect2) == stations.end()) {
        return false;
    }
    connections.push_back(new BezierCurve(connect1, slot1,
        stations[connect1]->GetSlot(slot1).first + glm::vec3(stations[connect1]->GetPosX(), stations[connect1]->GetPosY(), 0.f),
        stations[connect1]->GetSlot(slot1).second,
        connect2, slot2,
        stations[connect2]->GetSlot(slot2).first + glm::vec3(stations[connect2]->GetPosX(), stations[connect2]->GetPosY(), 0.f),
        stations[connect2]->GetSlot(slot2).second));
    return true;
}

// 清空注册
void RouteFactory::RemoveAll() {
    for(auto &config : configs) {
        config.second = false;
    }
}

// 注册线路
void RouteFactory::RegisterRoute(const string& id,
    function<Route*()> creator, function<void(Route*)> deleter) {
    registries[id] = {creator, deleter};
}

// 创建线路
Route* RouteFactory::CreateRoute(const string& id) const {
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Route %s not enabled when creating.\n", id.data());
        return nullptr;
    }

    auto it = registries.find(id);
    if (it == registries.end()) {
        debugf("Warning: Route %s not registered when creating.\n", id.data());
        return nullptr;
    }

    if (it->second.first) {
        return it->second.first();
    } else {
        THROW_EXCEPTION(NullPointerException, "Route " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool RouteFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void RouteFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 析构线路
void RouteFactory::DestroyRoute(Route* route) const {
    if (!route) {
        debugf("Warning: Route is null when deleting.\n");
        return;
    }

    auto it = registries.find(route->GetType());
    if (it == registries.end()) {
        debugf("Warning: Route %s not registered when deleting.\n", route->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(route);
    } else {
        THROW_EXCEPTION(NullPointerException, "Route " + route->GetType() + " deleter is null.\n");
    }
}

