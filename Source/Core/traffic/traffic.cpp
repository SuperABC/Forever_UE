#include "traffic.h"

#include "story/change.h"
#include "story/condition.h"

#include "map/map.h"
#include "map/block.h"
#include "map/roadnet.h"
#include "map/building.h"
#include "map/room.h"
#include "populace/populace.h"
#include "populace/person.h"
#include "traffic/vehicle.h"
#include "traffic/station.h"
#include "traffic/route.h"


using namespace std;

VehicleFactory* Traffic::vehicleFactory = nullptr;
StationFactory* Traffic::stationFactory = nullptr;
RouteFactory* Traffic::routeFactory = nullptr;

Traffic::Traffic() :
	routes(),
	stations(),
	vehicles() {
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
	vehicleFactory->RegisterVehicle(EmptyVehicle::GetId(), EmptyVehicle::GetLabel(),
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

			auto registerFunc = reinterpret_cast<RegisterModVehiclesFunc>(GetProcAddress(modHandle, "RegisterModVehicles"));
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
	stationFactory->RegisterStation(EmptyStation::GetId(), EmptyStation::StationAssigner,
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

			auto registerFunc = reinterpret_cast<RegisterModStationsFunc>(GetProcAddress(modHandle, "RegisterModStations"));
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

			auto registerFunc = reinterpret_cast<RegisterModRoutesFunc>(GetProcAddress(modHandle, "RegisterModRoutes"));
			if (registerFunc) {
				registerFunc(routeFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}
}

void Traffic::InitBuildings(Map* map) {
	Destroy();

	auto roadnet = map->GetRoadnet();
	vector<Lot*> blocks;
	for (auto block : roadnet->GetBlocks()) {
		blocks.emplace_back(block);
	}
	auto types = stationFactory->GetTypes();
	for(auto type : types) {
		auto counts = stationFactory->AssignStations(type, blocks);
		for(int i = 0; i < counts.size(); ++i) {
			if(counts[i] <= 0) {
				continue;
			}
			auto block = static_cast<Block*>(blocks[i]);
			for(int j = 0; j < counts[i]; ++j) {
				auto station = new Station(Traffic::stationFactory, type);
				station->LayoutStation(block);

				auto building = new Building(Map::buildingFactory, station->GetBuildingType());
				building->SetParent(block);
				building->SetAcreage(station->GetBuildingAcreage());
				block->AddBuilding(building->GetName(), building);
				map->AddBuilding(building);
				station->SetBuilding(building->GetName());
				stations[station->GetName()] = station;
			}
		}
	}
}

void Traffic::InitTraffic(Map* map, Populace* populace) {
	unordered_map<string, vector<pair<vector<Node*>, vector<pair<Node*, Node*>>>>> stationInterfaces;
	for(auto& [_, station] : stations) {
		unordered_map<string, vector<pair<Node*, Node*>>> interfaces;
		auto building = map->GetBuilding(station->GetBuilding());
		station->PlaceInterface(building, building->GetPivots());
		for(auto& [type, ray] : station->GetInterfaces()) {
			interfaces[type].push_back(ray);
		}
		for(auto & [type, rays] : interfaces) {
			stationInterfaces[type].push_back({ station->GetNodes(), rays });
		}
	}

	auto types = routeFactory->GetRoutes();
	for (auto type : types) {
		auto route = new Route(routeFactory, type);
		route->LayoutRoute(stationInterfaces, map->GetSize().first, map->GetSize().second, Node::GetCount());
		routes[route->GetName()] = route;
	}

	vector<pair<Quad, Room*>> parkings;
	for (auto& room : map->GetRooms()) {
		if (room->IsParking()) {
			for (auto& parking : room->GetParkings()) {
				parkings.emplace_back(parking, room);
			}
		}
	}

	for(auto citizen : populace->GetCitizens()) {
		if(parkings.empty()) {
			break;
		}

		auto vehicle = new Vehicle(vehicleFactory, "");
		if(!vehicle) {
			continue;
		}
		vehicle->SetupVehicle();
		int idx = GetRandom((int)parkings.size());
		auto &[quad, room] = parkings[idx];
		vehicle->EnterRoom(room);
		vehicle->SetPosition(quad.GetPosX(), quad.GetPosY(), 0.f, 0.f);
		parkings[idx] = parkings.back();
		parkings.pop_back();

		AddVehicle(vehicle->GetName(), vehicle);
	}
}

void Traffic::Destroy() {
	for (auto& [_, route] : routes) {
		if (route) delete route;
		route = nullptr;
	}
	routes.clear();

	for (auto& [_, vehicle] : vehicles) {
		if (vehicle) delete vehicle;
		vehicle = nullptr;
	}
	vehicles.clear();
}

void Traffic::Tick(Player* player) {

}

void Traffic::ApplyChange(Change* change,
	const vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	if (!change) {
		THROW_EXCEPTION(NullPointerException, "Change is null.\n");
	}

	auto type = change->GetType();

	if (type == "add_option") {
		auto obj = dynamic_cast<AddOptionChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to AddOptionChange.\n");
		}

		Condition conditionTarget;
		conditionTarget.ParseCondition(obj->GetName());
		string targetName = ToString(conditionTarget.EvaluateValue(getValues));
		Vehicle* target = GetVehicle(targetName);
		if (!target) {
			return;
		}

		Condition conditionOption;
		conditionOption.ParseCondition(obj->GetOption());
		string option = ToString(conditionOption.EvaluateValue(getValues));
		target->AddOption(option);
	}
	else if (type == "remove_option") {
		auto obj = dynamic_cast<RemoveOptionChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to RemoveOptionChange.\n");
		}

		Condition conditionTarget;
		conditionTarget.ParseCondition(obj->GetName());
		string targetName = ToString(conditionTarget.EvaluateValue(getValues));
		Vehicle* target = GetVehicle(targetName);
		if (!target) {
			return;
		}

		Condition conditionOption;
		conditionOption.ParseCondition(obj->GetOption());
		string option = ToString(conditionOption.EvaluateValue(getValues));
		target->RemoveOption(option);
	}
}

Vehicle* Traffic::GetVehicle(const string& name) const {
	auto it = vehicles.find(name);
	if (it == vehicles.end()) return nullptr;
	return it->second;
}

vector<Vehicle*> Traffic::GetVehicles() const {
	vector<Vehicle*> result;
	for (auto& [_, vehicle] : vehicles) {
		if (vehicle) {
			result.push_back(vehicle);
		}
	}
	return result;
}

void Traffic::AddVehicle(string name, Vehicle* vehicle) {
	if (vehicles.find(name) != vehicles.end()) {
		debugf("Warning: Vehicle with name %s already exists.\n", name.data());
		return;
	}
	vehicles[name] = vehicle;
}

void Traffic::RemoveVehicle(string name) {
	auto it = vehicles.find(name);
	if (it == vehicles.end()) {
		debugf("Warning: Vehicle with name %s does not exist.\n", name.data());
		return;
	}
	if (it->second) delete it->second;
	it->second = nullptr;
	vehicles.erase(it);
}

