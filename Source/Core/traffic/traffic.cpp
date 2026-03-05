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

Traffic::Traffic() {
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

}

void Traffic::SetResourcePath(string path) {
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
		debugf("Mod dll loaded successfully.\n");
		RegisterModRoutesFunc registerFunc = (RegisterModRoutesFunc)GetProcAddress(modHandle, "RegisterModRoutes");
		if (registerFunc) {
			registerFunc(routeFactory);
		}
		else {
			debugf("Incorrect dll content.\n");
		}
	}
	else {
		debugf("Failed to load mod dll.\n");
	}

#ifdef MOD_TEST
	auto routeList = { "mod" };
	for (const auto& routeId : routeList) {
		if (routeFactory->CheckRegistered(routeId)) {
			auto route = routeFactory->CreateRoute(routeId);
			debugf(("Created route: " + route->GetName() + " (ID: " + routeId + ").\n").data());
			delete route;
		}
		else {
			debugf("Route not registered: %s.\n", routeId);
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
		debugf("Mod dll loaded successfully.\n");
		RegisterModStationsFunc registerFunc = (RegisterModStationsFunc)GetProcAddress(modHandle, "RegisterModStations");
		if (registerFunc) {
			registerFunc(stationFactory);
		}
		else {
			debugf("Incorrect dll content.\n");
		}
	}
	else {
		debugf("Failed to load mod dll.\n");
	}

#ifdef MOD_TEST
	auto stationList = { "mod" };
	for (const auto& stationId : stationList) {
		if (stationFactory->CheckRegistered(stationId)) {
			auto station = stationFactory->CreateStation(stationId);
			debugf(("Created station: " + station->GetName() + " (ID: " + stationId + ").\n").data());
			delete station;
		}
		else {
			debugf("Station not registered: %s.\n", stationId);
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
		debugf("Mod dll loaded successfully.\n");
		RegisterModVehiclesFunc registerFunc = (RegisterModVehiclesFunc)GetProcAddress(modHandle, "RegisterModVehicles");
		if (registerFunc) {
			registerFunc(vehicleFactory);
		}
		else {
			debugf("Incorrect dll content.\n");
		}
	}
	else {
		debugf("Failed to load mod dll.\n");
	}

#ifdef MOD_TEST
	auto vehicleList = { "mod" };
	for (const auto& vehicleId : vehicleList) {
		if (vehicleFactory->CheckRegistered(vehicleId)) {
			auto vehicle = vehicleFactory->CreateVehicle(vehicleId);
			debugf(("Created vehicle: " + vehicle->GetName() + " (ID: " + vehicleId + ").\n").data());
			delete vehicle;
		}
		else {
			debugf("Vehicle not registered: %s.\n", vehicleId);
		}
	}
#endif // MOD_TEST

}

void Traffic::Init(Map* map) {

}

void Traffic::ReadConfigs(string path) const {
	path = resourcePath + path;
	if (!filesystem::exists(path)) {
		THROW_EXCEPTION(IOException, "Path does not exist: " + path + ".\n");
	}

	JsonReader reader;
	JsonValue root;

	ifstream fin(path);
	if (!fin.is_open()) {
		THROW_EXCEPTION(IOException, "Failed to open file: " + path + ".\n");
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

void Traffic::Load(string path) {

}

void Traffic::Save(string path) const {

}

void Traffic::ApplyChange(Change* change, Story* story,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {

}

RouteFactory* Traffic::GetRouteFactory() {
	return routeFactory;
}

StationFactory* Traffic::GetStationFactory() {
	return stationFactory;
}

