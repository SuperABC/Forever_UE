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
		[](RoadnetMod* roadnet) { delete roadnet; }
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
	static vector<string> mods = { "residential", "shop", "factory" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModBuildings(BuildingFactory* factory) {
	factory->RegisterBuilding(ResidentialBuilding::GetId(),
		ResidentialBuilding::GetPowers(), ResidentialBuilding::BuildingAssigner,
		[]() { return new ResidentialBuilding(); },
		[](BuildingMod* building) { delete building; }
	);
	factory->RegisterBuilding(ShopBuilding::GetId(),
		ShopBuilding::GetPowers(), ShopBuilding::BuildingAssigner,
		[]() { return new ShopBuilding(); },
		[](BuildingMod* building) { delete building; }
	);
	factory->RegisterBuilding(FactoryBuilding::GetId(),
		FactoryBuilding::GetPowers(), FactoryBuilding::BuildingAssigner,
		[]() { return new FactoryBuilding(); },
		[](BuildingMod* building) { delete building; }
	);
}

extern "C" __declspec(dllexport) void* GetModComponents() {
	static vector<string> mods = { "residential", "shop", "factory" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModComponents(ComponentFactory* factory) {
	factory->RegisterComponent(ResidentialComponent::GetId(),
		[]() { return new ResidentialComponent(); },
		[](ComponentMod* component) { delete component; }
	);
	factory->RegisterComponent(ShopComponent::GetId(),
		[]() { return new ShopComponent(); },
		[](ComponentMod* component) { delete component; }
	);
	factory->RegisterComponent(FactoryComponent::GetId(),
		[]() { return new FactoryComponent(); },
		[](ComponentMod* component) { delete component; }
	);
}

extern "C" __declspec(dllexport) void* GetModRooms() {
	static vector<string> mods = { "residential", "shop", "warehouse", "factory" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModRooms(RoomFactory* factory) {
	factory->RegisterRoom(ResidentialRoom::GetId(),
		[]() { return new ResidentialRoom(); },
		[](RoomMod* room) { delete room; }
	);
	factory->RegisterRoom(ShopRoom::GetId(),
		[]() { return new ShopRoom(); },
		[](RoomMod* room) { delete room; }
	);
	factory->RegisterRoom(WarehouseRoom::GetId(),
		[]() { return new WarehouseRoom(); },
		[](RoomMod* room) { delete room; }
	);
	factory->RegisterRoom(FactoryRoom::GetId(),
		[]() { return new FactoryRoom(); },
		[](RoomMod* room) { delete room; }
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

extern "C" __declspec(dllexport) void* GetModCalendars() {
	static vector<string> mods = { "standard", "full"};
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModCalendars(CalendarFactory* factory) {
	factory->RegisterCalendar(StandardCalendar::GetId(),
		[]() { return new StandardCalendar(); },
		[](CalendarMod* calendar) { delete calendar; }
	);
	factory->RegisterCalendar(FullCalendar::GetId(),
		[]() { return new FullCalendar(); },
		[](CalendarMod* calendar) { delete calendar; }
	);
}

extern "C" __declspec(dllexport) void* GetModJobs() {
	static vector<string> mods = { "shop_saler" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModJobs(JobFactory* factory) {
	factory->RegisterJob(ShopSalerJob::GetId(),
		[]() { return new ShopSalerJob(); },
		[](JobMod* job) { delete job; }
	);
}

extern "C" __declspec(dllexport) void* GetModOrganizations() {
	static vector<string> mods = { "shop" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModOrganizations(OrganizationFactory* factory) {
	factory->RegisterOrganization(ShopOrganization::GetId(), ShopOrganization::GetPower(),
		[]() { return new ShopOrganization(); },
		[](OrganizationMod* organization) { delete organization; }
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

extern "C" __declspec(dllexport) void* GetModProducts() {
	static vector<string> mods = { "experience" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModProducts(ProductFactory* factory) {
	factory->RegisterProduct(ExperienceProduct::GetId(),
		[]() { return new ExperienceProduct(); },
		[](ProductMod* product) { delete product; }
	);
}

extern "C" __declspec(dllexport) void* GetModStorages() {
	static vector<string> mods = { "shop" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModStorages(StorageFactory* factory) {
	factory->RegisterStorage(ShopStorage::GetId(),
		[]() { return new ShopStorage(); },
		[](StorageMod* storage) { delete storage; }
	);
}

extern "C" __declspec(dllexport) void* GetModManufactures() {
	static vector<string> mods = { "experience" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModManufactures(ManufactureFactory* factory) {
	factory->RegisterManufacture(ExperienceManufacture::GetId(),
		[]() { return new ExperienceManufacture(); },
		[](ManufactureMod* manufacture) { delete manufacture; }
	);
}

