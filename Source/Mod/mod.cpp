#include "terrain_mod.h"
#include "roadnet_mod.h"
#include "zone_mod.h"
#include "building_mod.h"
#include "component_mod.h"
#include "room_mod.h"
#include "asset_mod.h"
#include "job_mod.h"
#include "name_mod.h"
#include "scheduler_mod.h"
#include "calendar_mod.h"
#include "organization_mod.h"
#include "event_mod.h"
#include "change_mod.h"

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

extern "C" __declspec(dllexport) void RegisterModAssets(AssetFactory* factory) {
    factory->RegisterAsset(ModAsset::GetId(), []() {
        return new ModAsset();
        });
}

extern "C" __declspec(dllexport) void RegisterModJobs(JobFactory* factory) {
    factory->RegisterJob(ModJob::GetId(), []() {
        return new ModJob();
        });
}

extern "C" __declspec(dllexport) void RegisterModNames(NameFactory* factory) {
    factory->RegisterName(ModName::GetId(), []() {
        return new ModName();
        });
}

extern "C" __declspec(dllexport) void RegisterModSchedulers(SchedulerFactory* factory) {
    factory->RegisterScheduler(ModScheduler::GetId(), []() {
        return new ModScheduler();
        }, ModScheduler::GetPower());
}

extern "C" __declspec(dllexport) void RegisterModCalendars(CalendarFactory* factory) {
    factory->RegisterCalendar(ModCalendar::GetId(), []() {
        return new ModCalendar();
        });
}

extern "C" __declspec(dllexport) void RegisterModOrganizations(OrganizationFactory* factory) {
    factory->RegisterOrganization(ModOrganization::GetId(), []() {
        return new ModOrganization();
        }, ModOrganization::GetPower());
}

extern "C" __declspec(dllexport) void RegisterModEvents(EventFactory* factory) {
    factory->RegisterEvent(ModEvent::GetId(), []() {
        return new ModEvent();
        });
}

extern "C" __declspec(dllexport) void RegisterModChanges(ChangeFactory* factory) {
    factory->RegisterChange(ModChange::GetId(), []() {
        return new ModChange();
        });
}

