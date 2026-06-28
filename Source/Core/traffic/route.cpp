#include "route.h"

#include "map/map.h"
#include "map/roadnet.h"
#include "map/block.h"
#include "map/building.h"
#include "traffic/traffic.h"
#include "traffic/station.h"


using namespace std;

Route::Route(RouteFactory* factory, const string& route) :
	mod(factory->CreateRoute(route)),
	factory(factory),
	type(),
	name(),
	stations() {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Route " + route + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Route::~Route() {
	factory->DestroyRoute(mod);

	for (auto& [_, station] : stations) {
		if (station)delete station;
		station = nullptr;
	}
}

string Route::GetType() const {
	return type;
}

string Route::GetName() const {
	return name;
}

void Route::LayoutRoute(Map* map) {
	auto roadnet = map->GetRoadnet();

	vector<Lot*> blocks;
	for(auto block : roadnet->GetBlocks()) {
		blocks.push_back(static_cast<Lot*>(block));
	}
	mod->LayoutRoute(roadnet->GetIntersections(), roadnet->GetRoads(), blocks);
	for (auto& [id, acreage, stationType, buildingType] : mod->stations) {
		auto block = roadnet->GetBlocks()[id];

		auto building = new Building(Map::buildingFactory, buildingType);
		building->SetParent(block);
		building->SetAcreage(acreage);
		block->AddBuilding(building->GetName(), building);
		map->AddBuilding(building);

		auto station = new Station(Traffic::stationFactory, stationType);
		station->SetBuilding(building->GetName());
		stations[station->GetName()] = station;
	}
}

int EmptyRoute::count = 0;

EmptyRoute::EmptyRoute() : id(count++) {

}

EmptyRoute::~EmptyRoute() {

}

const char* EmptyRoute::GetId() {
	return "empty";
}

const char* EmptyRoute::GetType() const {
	return "empty";
}

const char* EmptyRoute::GetName() {
	name = "空线路" + to_string(id);
	return name.data();
}

void EmptyRoute::LayoutRoute(
	const vector<Intersection*>& intersections, const vector<Road*>& roads, const vector<Lot*>& blocks) {

}


