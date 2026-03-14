#include "commute.h"


using namespace std;

Commute::Commute() :
	currentPaths(),
	startTime(),
	targetRoom(nullptr) {
}

Commute::~Commute() {

}

void Commute::SetNull() {
	currentPaths.clear();
}

bool Commute::IsNull() const {
	return currentPaths.empty();
}

Room* Commute::GetTarget() const {
	return targetRoom;
}

void Commute::SetTarget(Room* target) {
	if (target == nullptr) {
		debugf("Warning: commute target is nullptr.\n");
	}
	targetRoom = target;
}

void Commute::SetPaths(const vector<Connection>& paths) {
	currentPaths = paths;
}

void Commute::StartCommute(Time time) {
	startTime = time;
}

void Commute::FinishCommute() {
	currentPaths.clear();
}

pair<Connection, float> Commute::RealtimeConnection(const Time& time) {
	float dist = (time - startTime).GetMinute() * 600.f;
	for (const auto& connection : currentPaths) {
		auto distance = connection.distance();
		if (dist < distance) {
			return {connection, dist / distance};
		}
		dist -= connection.distance();
	}

	FinishCommute();

	return {Connection("", nullptr, -1, -1), 0.f};
}

bool Commute::AtConnection(const Connection& connection, const Time& time) const {
	float dist = (time - startTime).GetMinute() * 600.f;
	for (const auto& conn : currentPaths) {
		if (dist < conn.distance()) {
			return conn == connection;
		}
		dist -= conn.distance();
	}
	return false;
}
