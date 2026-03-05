#include "zone.h"


using namespace std;

int DefaultZone::count = 0;

DefaultZone::DefaultZone() {
    name = count++;
}

string DefaultZone::GetId() {
    return "default";
}

string DefaultZone::GetType() const {
    return "default";
}

string DefaultZone::GetName() const {
    return "默认园区" + to_string(name);
}

void DefaultZone::SetZone(Plot* plot, BuildingFactory* buildingFactory) {
    Quad::SetAcreage(40000.f);

    vector<pair<string, float>> buildings;
    int num = 40;
    for (int i = 0; i < num; i++) {
        buildings.push_back({ "default_residential", 1.f });
        buildings.push_back({ "default_working", 1.f });
    }
    AddBuildings(buildingFactory, buildings);
}

function<int(Plot*)> DefaultZone::ZoneGenerator = [](Plot* plot) { return 1; };