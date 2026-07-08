#include "route_basic.h"


using namespace std;

int AirRoute::count = 0;

AirRoute::AirRoute() : id(count++) {

}

AirRoute::~AirRoute() {

}

const char* AirRoute::GetId() {
	return "air";
}

const char* AirRoute::GetType() const {
	return "air";
}

const char* AirRoute::GetName() {
	name = "飞机线路" + to_string(id);
	return name.data();
}

void AirRoute::LayoutRoute(
	const unordered_map<string, vector<vector<vector<float>>>>& interfaces,
	int nodeStaticCount) {
	Node::SetCount(nodeStaticCount);
}
