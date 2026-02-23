#include "commute.h"


using namespace std;

Commute::Commute() {

}

Commute::~Commute() {

}

void Commute::SetNull() {
	currentPaths.clear();
}

bool Commute::IsNull() {
	return currentPaths.size() == 0;
}

void Commute::SetTarget(Room* target) {
	targetRoom = target;
}

Room* Commute::GetTarget() {
	return targetRoom;
}

void Commute::SetPaths(vector<Connection> paths) {
	currentPaths = paths;
}

void Commute::StartCommute(Time time) {
	startTime = time;
}

void Commute::FinishCommute() {
	currentPaths.clear();
}

pair<Connection, float> Commute::RealtimeConnection(Time time) {
	float dist = (time - startTime).GetMinute() * 600.f;
	for(auto connection : currentPaths) {
		auto distance = connection.distance();
		if(dist < distance) {
			return make_pair(connection, dist / distance);
		}
		else {
			dist -= connection.distance();
		}
	}

	FinishCommute();

	return make_pair(Connection("", nullptr, -1, -1), 0.f);
}

bool Commute::AtConnection(Connection connection, Time time) const {
	float dist = (time - startTime).GetMinute() * 600.f;
	for (auto conn : currentPaths) {
		if (dist < conn.distance()) {
			return conn == connection;
		}
		else {
			dist -= conn.distance();
		}
	}
	return false;
}
