#include "route_base.h"
#include "../common/error.h"
#include "../common/utility.h"

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

Route::Route() {
}

Route::~Route() {
    for (auto connection : connections) {
        delete connection;
    }
}

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

vector<BezierCurve*> Route::GetConnections() {
    return connections;
}

void RouteFactory::RegisterRoute(const string& id,
	function<Route*()> creator, function<void(Route*)> deleter) {
    registries[id] = {creator, deleter};
}

Route* RouteFactory::CreateRoute(const string& id) {
    if (configs.find(id) == configs.end() || !configs.find(id)->second)
        return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first();
    }
    return nullptr;
}

bool RouteFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void RouteFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}

void RouteFactory::DestroyRoute(Route* route) const {
    if (!route) return;
    auto it = registries.find(route->GetType());
    if (it != registries.end()) {
        it->second.second(route);
    } else {
        debugf(("Deleter not found for " + route->GetType() + ".\n").data());
    }
}