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
	name() {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Route " + route + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Route::~Route() {
	factory->DestroyRoute(mod);

	for(auto ext : externs) {
		delete ext;
	}
	externs.clear();
	for(auto node : nodes) {
		delete node;
	}
	nodes.clear();
	for(auto connection : connections) {
		delete connection;
	}
	connections.clear();
}

string Route::GetType() const {
	return type;
}

string Route::GetName() const {
	return name;
}

void Route::LayoutRoute(
	const unordered_map<string, vector<vector<vector<float>>>>& interfaces) {
	mod->LayoutRoute(interfaces);

	for(auto& ext : mod->externs) {
		externs.push_back(new Node(ext));
	}
	for(auto& node : mod->nodes) {
		nodes.push_back(new Node(node));
	}
	for(auto& connection : mod->connections) {
		connections.push_back(new Connection(connection));
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
	const unordered_map<string, vector<vector<vector<float>>>>& interfaces) {

}


