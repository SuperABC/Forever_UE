#include "map/terrain_basic.h"
#include "map/roadnet_basic.h"
#include "map/zone_basic.h"
#include "map/building_basic.h"
#include "map/component_basic.h"
#include "map/room_basic.h"
#include "populace/asset_basic.h"
#include "populace/name_basic.h"
#include "populace/scheduler_basic.h"
#include "society/calendar_basic.h"
#include "society/job_basic.h"
#include "society/organization_basic.h"
#include "story/script_basic.h"
#include "industry/product_basic.h"
#include "industry/storage_basic.h"
#include "industry/manufacture_basic.h"
#include "traffic/route_basic.h"
#include "traffic/station_basic.h"
#include "traffic/vehicle_basic.h"
#include "player/skill_basic.h"

#pragma comment(lib, "Dependence.lib")


using namespace std;

extern "C" __declspec(dllexport) void* GetModTerrains() {
	static vector<string> mods = { "ocean", "mountain" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModTerrains(TerrainFactory* factory) {
	factory->RegisterTerrain(OceanTerrain::GetId(),
		[]() { return new OceanTerrain(); },
		[](TerrainMod* terrain) { delete terrain; }
	);
	factory->RegisterTerrain(MountainTerrain::GetId(),
		[]() { return new MountainTerrain(); },
		[](TerrainMod* terrain) { delete terrain; }
	);
}

extern "C" __declspec(dllexport) void* GetModRoadnets() {
	static vector<string> mods = { "jing" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModRoadnets(RoadnetFactory* factory) {
	factory->RegisterRoadnet(JingRoadnet::GetId(),
		[]() { return new JingRoadnet(); },
		[](RoadnetMod* terrain) { delete terrain; }
	);
}

extern "C" __declspec(dllexport) void* GetModZones() {
	static vector<string> mods = { "residential" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModZones(ZoneFactory* factory) {
	factory->RegisterZone(ResidentialZone::GetId(), ResidentialZone::ZoneAssigner,
		[]() { return new ResidentialZone(); },
		[](ZoneMod* zone) { delete zone; }
	);
}

extern "C" __declspec(dllexport) void* GetModBuildings() {
	static vector<string> mods = { "residential" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModBuildings(BuildingFactory* factory) {
	factory->RegisterBuilding(ResidentialBuilding::GetId(),
		ResidentialBuilding::GetPowers(), ResidentialBuilding::BuildingAssigner,
		[]() { return new ResidentialBuilding(); },
		[](BuildingMod* zone) { delete zone; }
	);
}

extern "C" __declspec(dllexport) void* GetModComponents() {
	static vector<string> mods = { "residential" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModComponents(ComponentFactory* factory) {
	factory->RegisterComponent(ResidentialComponent::GetId(),
		[]() { return new ResidentialComponent(); },
		[](ComponentMod* zone) { delete zone; }
	);
}

extern "C" __declspec(dllexport) void* GetModRooms() {
	static vector<string> mods = { "residential" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModRooms(RoomFactory* factory) {
	factory->RegisterRoom(ResidentialRoom::GetId(),
		[]() { return new ResidentialRoom(); },
		[](RoomMod* zone) { delete zone; }
	);
}

extern "C" __declspec(dllexport) void* GetModAssets() {
	static vector<string> mods = { "zone", "building", "room" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModAssets(AssetFactory* factory) {
	factory->RegisterAsset(ZoneAsset::GetId(),
		[]() { return new ZoneAsset(); },
		[](AssetMod* asset) { delete asset; }
	);
	factory->RegisterAsset(BuildingAsset::GetId(),
		[]() { return new BuildingAsset(); },
		[](AssetMod* asset) { delete asset; }
	);
	factory->RegisterAsset(RoomAsset::GetId(),
		[]() { return new RoomAsset(); },
		[](AssetMod* asset) { delete asset; }
	);
}

extern "C" __declspec(dllexport) void* GetModNames() {
	static vector<string> mods = { "chinese" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModNames(NameFactory* factory) {
	factory->RegisterName(ChineseName::GetId(),
		[]() { return new ChineseName(); },
		[](NameMod* name) { delete name; }
	);
}

extern "C" __declspec(dllexport) void* GetModSchedulers() {
	static vector<string> mods = { "workonly" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModSchedulers(SchedulerFactory* factory) {
	factory->RegisterScheduler(WorkonlyScheduler::GetId(), WorkonlyScheduler::GetPower(),
		[]() { return new WorkonlyScheduler(); },
		[](SchedulerMod* scheduler) { delete scheduler; }
	);
}

extern "C" __declspec(dllexport) void* GetModScripts() {
	static vector<string> mods = { "npc" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModScripts(ScriptFactory* factory) {
	factory->RegisterScript(NPCScript::GetId(), NPCScript::MainStory(),
		[]() { return new NPCScript(); },
		[](ScriptMod* script) { delete script; }
	);
}