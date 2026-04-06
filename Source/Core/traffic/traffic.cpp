#include "traffic.h"

#include "traffic/vehicle.h"
#include "traffic/station.h"
#include "traffic/route.h"


using namespace std;

VehicleFactory* Traffic::vehicleFactory = nullptr;
StationFactory* Traffic::stationFactory = nullptr;
RouteFactory* Traffic::routeFactory = nullptr;

Traffic::Traffic() :
	stations(),
	routes() {
	if (!vehicleFactory) {
		vehicleFactory = new VehicleFactory();
	}
	if (!stationFactory) {
		stationFactory = new StationFactory();
	}
	if (!routeFactory) {
		routeFactory = new RouteFactory();
	}
}

Traffic::~Traffic() {
	Destroy();
}

void Traffic::LoadConfigs() const {
	routeFactory->RemoveAll();
	stationFactory->RemoveAll();
	vehicleFactory->RemoveAll();

	auto routes = Config::GetEnables("route");
	for (auto route : routes) {
		routeFactory->SetConfig(route, true);
	}
	auto stations = Config::GetEnables("station");
	for (auto station : stations) {
		stationFactory->SetConfig(station, true);
	}
	auto vehicles = Config::GetEnables("vehicle");
	for (auto vehicle : vehicles) {
		vehicleFactory->SetConfig(vehicle, true);
	}
}

void Traffic::InitVehicles(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {
	vehicleFactory->RegisterVehicle(EmptyVehicle::GetId(),
		[]() { return new EmptyVehicle(); },
		[](VehicleMod* vehicle) { delete vehicle; }
	);

	for (auto dll : dlls) {
		HMODULE modHandle;
		if (modHandles.find(dll) != modHandles.end()) {
			modHandle = modHandles[dll];
		}
		else {
			modHandle = LoadLibraryA(dll.data());
			modHandles[dll] = modHandle;
		}
		if (modHandle) {
			debugf("Log: %s loaded successfully.\n", dll.data());

			auto registerFunc = (RegisterModVehiclesFunc)GetProcAddress(modHandle, "RegisterModVehicles");
			if (registerFunc) {
				registerFunc(vehicleFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}
}

void Traffic::InitStations(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {
	stationFactory->RegisterStation(EmptyStation::GetId(),
		[]() { return new EmptyStation(); },
		[](StationMod* station) { delete station; }
	);

	for (auto dll : dlls) {
		HMODULE modHandle;
		if (modHandles.find(dll) != modHandles.end()) {
			modHandle = modHandles[dll];
		}
		else {
			modHandle = LoadLibraryA(dll.data());
			modHandles[dll] = modHandle;
		}
		if (modHandle) {
			debugf("Log: %s loaded successfully.\n", dll.data());

			auto registerFunc = (RegisterModStationsFunc)GetProcAddress(modHandle, "RegisterModStations");
			if (registerFunc) {
				registerFunc(stationFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}
}

void Traffic::InitRoutes(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {
	routeFactory->RegisterRoute(EmptyRoute::GetId(),
		[]() { return new EmptyRoute(); },
		[](RouteMod* route) { delete route; }
	);

	for (auto dll : dlls) {
		HMODULE modHandle;
		if (modHandles.find(dll) != modHandles.end()) {
			modHandle = modHandles[dll];
		}
		else {
			modHandle = LoadLibraryA(dll.data());
			modHandles[dll] = modHandle;
		}
		if (modHandle) {
			debugf("Log: %s loaded successfully.\n", dll.data());

			auto registerFunc = (RegisterModRoutesFunc)GetProcAddress(modHandle, "RegisterModRoutes");
			if (registerFunc) {
				registerFunc(routeFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}
}

void Traffic::Init(Map* map) {
	Destroy();
}

void Traffic::Destroy() {
	for (auto& station : stations) {
		if (station)delete station;
		station = nullptr;
	}
	stations.clear();
	for (auto& route : routes) {
		if (route)delete route;
		route = nullptr;
	}
	routes.clear();
}

void Traffic::Tick(Player* player) {

}

void Traffic::ApplyChange(Change* change,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {

}
