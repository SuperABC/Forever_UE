#include "terrain_mod.h"
#include "roadnet_mod.h"
#include "zone_mod.h"
#include "building_mod.h"
#include "component_mod.h"
#include "room_mod.h"
#include "asset_mod.h"
#include "name_mod.h"
#include "scheduler_mod.h"
#include "calendar_mod.h"
#include "job_mod.h"
#include "organization_mod.h"
#include "event_mod.h"
#include "change_mod.h"
#include "product_mod.h"
#include "storage_mod.h"
#include "manufacture_mod.h"
#include "route_mod.h"
#include "station_mod.h"
#include "vehicle_mod.h"
#include "skill_mod.h"

#pragma comment(lib, "Dependence.lib")


using namespace std;

extern "C" __declspec(dllexport) void* GetModTerrains() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModTerrains(TerrainFactory* factory) {
	factory->RegisterTerrain(ModTerrain::GetId(),
		[]() {
			return new ModTerrain();
		}, [](Terrain* terrain) {
			delete terrain;
		});
}

extern "C" __declspec(dllexport) void* GetModRoadnets() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModRoadnets(RoadnetFactory* factory) {
	factory->RegisterRoadnet(ModRoadnet::GetId(),
		[]() {
			return new ModRoadnet();
		}, [](Roadnet* roadnet) {
			delete roadnet;
		});
}

extern "C" __declspec(dllexport) void* GetModZones() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModZones(ZoneFactory* factory) {
	factory->RegisterZone(ModZone::GetId(), ModZone::ZoneGenerator,
		[]() {
			return new ModZone();
		}, [](Zone* zone) {
			delete zone;
		});
}

extern "C" __declspec(dllexport) void* GetModBuildings() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModBuildings(BuildingFactory* factory) {
	factory->RegisterBuilding(ModBuilding::GetId(), ModBuilding::GetPower(),
		[]() {
			return new ModBuilding();
		}, [](Building* building) {
			delete building;
		});
}

extern "C" __declspec(dllexport) void* GetModComponents() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModComponents(ComponentFactory* factory) {
	factory->RegisterComponent(ModComponent::GetId(),
		[]() {
			return new ModComponent();
		}, [](Component* component) {
			delete component;
		});
}

extern "C" __declspec(dllexport) void* GetModRooms() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModRooms(RoomFactory* factory) {
	factory->RegisterRoom(ModRoom::GetId(),
		[]() {
			return new ModRoom();
		}, [](Room* room) {
			delete room;
		});
}

extern "C" __declspec(dllexport) void* GetModAssets() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModAssets(AssetFactory* factory) {
	factory->RegisterAsset(ModAsset::GetId(),
		[]() {
			return new ModAsset();
		}, [](Asset* asset) {
			delete asset;
		});
}

extern "C" __declspec(dllexport) void* GetModNames() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModNames(NameFactory* factory) {
	factory->RegisterName(ModName::GetId(),
		[]() {
			return new ModName();
		}, [](Name* name) {
			delete name;
		});
}

extern "C" __declspec(dllexport) void* GetModSchedulers() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModSchedulers(SchedulerFactory* factory) {
	factory->RegisterScheduler(ModScheduler::GetId(), ModScheduler::GetPower(),
		[]() {
			return new ModScheduler();
		}, [](Scheduler* scheduler) {
			delete scheduler;
		});
}

extern "C" __declspec(dllexport) void* GetModJobs() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModJobs(JobFactory* factory) {
	factory->RegisterJob(ModJob::GetId(),
		[]() {
			return new ModJob();
		}, [](Job* job) {
			delete job;
		});
}

extern "C" __declspec(dllexport) void* GetModCalendars() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModCalendars(CalendarFactory* factory) {
	factory->RegisterCalendar(ModCalendar::GetId(),
		[]() {
			return new ModCalendar();
		}, [](Calendar* calendar) {
			delete calendar;
		});
}

extern "C" __declspec(dllexport) void* GetModOrganizations() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModOrganizations(OrganizationFactory* factory) {
	factory->RegisterOrganization(ModOrganization::GetId(), ModOrganization::GetPower(),
		[]() {
			return new ModOrganization();
		}, [](Organization* organization) {
			delete organization;
		});
}

extern "C" __declspec(dllexport) void* GetModEvents() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModEvents(EventFactory* factory) {
	factory->RegisterEvent(ModEvent::GetId(),
		[]() {
			return new ModEvent();
		}, [](Event* event) {
			delete event;
		});
}

extern "C" __declspec(dllexport) void* GetModChanges() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModChanges(ChangeFactory* factory) {
	factory->RegisterChange(ModChange::GetId(),
		[]() {
			return new ModChange();
		}, [](Change* change) {
			delete change;
		});
}

extern "C" __declspec(dllexport) void* GetModProducts() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModProducts(ProductFactory* factory) {
	factory->RegisterProduct(ModProduct::GetId(),
		[]() {
			return new ModProduct();
		}, [](Product* product) {
			delete product;
		});
}

extern "C" __declspec(dllexport) void* GetModStorages() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModStorages(StorageFactory* factory) {
	factory->RegisterStorage(ModStorage::GetId(),
		[]() {
			return new ModStorage();
		}, [](Storage* storage) {
			delete storage;
		});
}

extern "C" __declspec(dllexport) void* GetModManufactures() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModManufactures(ManufactureFactory* factory) {
	factory->RegisterManufacture(ModManufacture::GetId(),
		[]() {
			return new ModManufacture();
		}, [](Manufacture* manufacture) {
			delete manufacture;
		});
}

extern "C" __declspec(dllexport) void* GetModRoutes() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModRoutes(RouteFactory* factory) {
	factory->RegisterRoute(ModRoute::GetId(),
		[]() {
			return new ModRoute();
		}, [](Route* route) {
			delete route;
		});
}

extern "C" __declspec(dllexport) void* GetModStations() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModStations(StationFactory* factory) {
	factory->RegisterStation(ModStation::GetId(),
		[]() {
			return new ModStation();
		}, [](Station* station) {
			delete station;
		});
}

extern "C" __declspec(dllexport) void* GetModVehicles() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModVehicles(VehicleFactory* factory) {
	factory->RegisterVehicle(ModVehicle::GetId(),
		[]() {
			return new ModVehicle();
		}, [](Vehicle* vehicle) {
			delete vehicle;
		});
}

extern "C" __declspec(dllexport) void* GetModSkills() {
	static vector<string> mods = { "mod" };
	return (void*)&mods;
}

extern "C" __declspec(dllexport) void RegisterModSkills(SkillFactory* factory) {
	factory->RegisterSkill(ModSkill::GetId(),
		[]() {
			return new ModSkill();
		}, [](Skill* skill) {
			delete skill;
		});
}