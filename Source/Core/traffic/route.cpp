#include "route.h"


using namespace std;

// 飞机航线

string AirRoute::GetId() {
	return "air";
}

string AirRoute::GetType() const {
	return "air";
}

string AirRoute::GetName() const {
	return "飞机航线";
}

bool AirRoute::StandAlone() const {
	return true;
}

bool AirRoute::RouteVisible() const {
	return false;
}
