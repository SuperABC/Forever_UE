#include "traffic.h"
#include "utility.h"
#include "error.h"
#include "json.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <random>


using namespace std;

RouteFactory* Traffic::routeFactory = nullptr;
StationFactory* Traffic::stationFactory = nullptr;
VehicleFactory* Traffic::vehicleFactory = nullptr;

Traffic::Traffic() :
	resourcePath() {
	if (!routeFactory) {
		routeFactory = new RouteFactory();
	}
	if (!stationFactory) {
		stationFactory = new StationFactory();
	}
	if (!vehicleFactory) {
		vehicleFactory = new VehicleFactory();
	}
}

Traffic::~Traffic() {
	// 保留空实现，工厂由静态指针管理，无需在此释放
}

void Traffic::SetResourcePath(const string& path) {
	resourcePath = path;
}

void Traffic::InitRoutes(unordered_map<string, HMODULE>& modHandles) {
	routeFactory->RegisterRoute(AirRoute::GetId(),
		[]() { return new AirRoute(); },
		[](Route* route) { delete route; }
	);

	string modPath = "Mod.dll";
	HMODULE modHandle;
	if (modHandles.find(modPath) != modHandles.end()) {
		modHandle = modHandles[modPath];
	}
	else {
		modHandle = LoadLibraryA(modPath.data());
		modHandles[modPath] = modHandle;
	}
	if (modHandle) {
		debugf("Log: Mod dll loaded successfully.\n");
		RegisterModRoutesFunc registerFunc =
			(RegisterModRoutesFunc)GetProcAddress(modHandle, "RegisterModRoutes");
		if (registerFunc) {
			registerFunc(routeFactory);
		}
		else {
			debugf("Warning: Incorrect dll content.\n");
		}
	}
	else {
		debugf("Warning: Failed to load mod dll.\n");
	}

#ifdef MOD_TEST
	auto routeList = { "mod" };
	for (const auto& routeId : routeList) {
		if (routeFactory->CheckRegistered(routeId)) {
			auto route = routeFactory->CreateRoute(routeId);
			debugf("Log: Created test route %s.\n", routeId);
			routeFactory->DestroyRoute(route);
		}
		else {
			debugf("Warning: Route %s not registered.\n", routeId);
		}
	}
#endif // MOD_TEST
}

void Traffic::InitStations(unordered_map<string, HMODULE>& modHandles) {
	stationFactory->RegisterStation(AirportStation::GetId(),
		[]() { return new AirportStation(); },
		[](Station* station) { delete station; }
	);

	string modPath = "Mod.dll";
	HMODULE modHandle;
	if (modHandles.find(modPath) != modHandles.end()) {
		modHandle = modHandles[modPath];
	}
	else {
		modHandle = LoadLibraryA(modPath.data());
		modHandles[modPath] = modHandle;
	}
	if (modHandle) {
		debugf("Log: Mod dll loaded successfully.\n");
		RegisterModStationsFunc registerFunc =
			(RegisterModStationsFunc)GetProcAddress(modHandle, "RegisterModStations");
		if (registerFunc) {
			registerFunc(stationFactory);
		}
		else {
			debugf("Warning: Incorrect dll content.\n");
		}
	}
	else {
		debugf("Warning: Failed to load mod dll.\n");
	}

#ifdef MOD_TEST
	auto stationList = { "mod" };
	for (const auto& stationId : stationList) {
		if (stationFactory->CheckRegistered(stationId)) {
			auto station = stationFactory->CreateStation(stationId);
			debugf("Log: Created test station %s.\n", stationId);
			stationFactory->DestroyStation(station);
		}
		else {
			debugf("Warning: Station %s not registered.\n", stationId);
		}
	}
#endif // MOD_TEST
}

void Traffic::InitVehicles(unordered_map<string, HMODULE>& modHandles) {
	vehicleFactory->RegisterVehicle(PlaneVehicle::GetId(),
		[]() { return new PlaneVehicle(); },
		[](Vehicle* vehicle) { delete vehicle; }
	);

	string modPath = "Mod.dll";
	HMODULE modHandle;
	if (modHandles.find(modPath) != modHandles.end()) {
		modHandle = modHandles[modPath];
	}
	else {
		modHandle = LoadLibraryA(modPath.data());
		modHandles[modPath] = modHandle;
	}
	if (modHandle) {
		debugf("Log: Mod dll loaded successfully.\n");
		RegisterModVehiclesFunc registerFunc =
			(RegisterModVehiclesFunc)GetProcAddress(modHandle, "RegisterModVehicles");
		if (registerFunc) {
			registerFunc(vehicleFactory);
		}
		else {
			debugf("Warning: Incorrect dll content.\n");
		}
	}
	else {
		debugf("Warning: Failed to load mod dll.\n");
	}

#ifdef MOD_TEST
	auto vehicleList = { "mod" };
	for (const auto& vehicleId : vehicleList) {
		if (vehicleFactory->CheckRegistered(vehicleId)) {
			auto vehicle = vehicleFactory->CreateVehicle(vehicleId);
			debugf("Log: Created test vehicle %s.\n", vehicleId);
			vehicleFactory->DestroyVehicle(vehicle);
		}
		else {
			debugf("Warning: Vehicle %s not registered.\n", vehicleId);
		}
	}
#endif // MOD_TEST
}

void Traffic::Init(Map* map) {
	if (map == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Map is null.\n");
	}

}

void Traffic::ReadConfigs(const string& path) const {
	string fullPath = resourcePath + path;
	if (!filesystem::exists(fullPath)) {
		THROW_EXCEPTION(IOException, "Path does not exist: " + fullPath + ".\n");
	}

	JsonReader reader;
	JsonValue root;

	ifstream fin(fullPath);
	if (!fin.is_open()) {
		THROW_EXCEPTION(IOException, "Failed to open file: " + fullPath + ".\n");
	}
	if (reader.Parse(fin, root)) {
		for (auto route : root["mods"]["route"]) {
			routeFactory->SetConfig(route.AsString(), true);
		}
		for (auto station : root["mods"]["station"]) {
			stationFactory->SetConfig(station.AsString(), true);
		}
		for (auto vehicle : root["mods"]["vehicle"]) {
			vehicleFactory->SetConfig(vehicle.AsString(), true);
		}
	}
	else {
		fin.close();
		THROW_EXCEPTION(JsonFormatException, "Json syntax error: " + reader.GetErrorMessages() + ".\n");
	}
	fin.close();
}

void Traffic::Destroy() {

}

void Traffic::Tick(int day, int hour, int min, int sec) {

}

void Traffic::Print() const {

}

void Traffic::Load(const string& path) {

}

void Traffic::Save(const string& path) const {

}

void Traffic::ApplyChange(Change* change, Story* story,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	if (change == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Change is null.\n");
	}
	if (story == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Story is null.\n");
	}

}

RouteFactory* Traffic::GetRouteFactory() const {
	return routeFactory;
}

StationFactory* Traffic::GetStationFactory() const {
	return stationFactory;
}

