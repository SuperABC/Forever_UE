#include "terrain_mod.h"
#include "roadnet_mod.h"
#include "zone_mod.h"
#include "building_mod.h"
#include "component_mod.h"
#include "room_mod.h"

#pragma comment(lib, "Dependence.lib")


using namespace std;

extern "C" __declspec(dllexport) void RegisterModTerrains(TerrainFactory* factory) {
    factory->RegisterTerrain(ModTerrain::GetId(), []() {
        return new ModTerrain();
        });
}

extern "C" __declspec(dllexport) void RegisterModRoadnets(RoadnetFactory* factory) {
    factory->RegisterRoadnet(ModRoadnet::GetId(), []() {
        return new ModRoadnet();
        });
}

extern "C" __declspec(dllexport) void RegisterModZones(ZoneFactory* factory) {
    factory->RegisterZone(ModZone::GetId(),
        []() { return new ModZone(); }, ModZone::ZoneGenerator);
}

extern "C" __declspec(dllexport) void RegisterModBuildings(BuildingFactory* factory) {
    factory->RegisterBuilding(ModZone::GetId(),
        []() { return new ModBuilding(); }, ModBuilding::GetPower());
}

extern "C" __declspec(dllexport) void RegisterModComponents(ComponentFactory* factory) {
    factory->RegisterComponent(ModComponent::GetId(), []() {
        return new ModComponent();
        });
}

extern "C" __declspec(dllexport) void RegisterModRooms(RoomFactory* factory) {
    factory->RegisterRoom(ModRoom::GetId(), []() {
        return new ModRoom();
        });
}


