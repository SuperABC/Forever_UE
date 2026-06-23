#include "commute.h"


using namespace std;

Commute::Commute() :
	simulate(true),
	visible(false),
	targetAddress(),
	currentPaths(),
	reversedPaths(),
	currentIdx(0),
	currentEnd() {

}

Commute::~Commute() {

}

string Commute::GetTarget() const {
	return targetAddress;
}

void Commute::SetTarget(const string& target) {
	targetAddress = target;
}

void Commute::SetPaths(const vector<Connection*>& paths) {
	currentPaths = paths;

	// 根据相邻连接共享的端点，推算每段连接的实际通行方向（第一段默认按其自身起点->终点方向通行）
	reversedPaths.assign(paths.size(), false);
	for (size_t i = 1; i < paths.size(); i++) {
		int previousExit = reversedPaths[i - 1] ?
			paths[i - 1]->GetStart().GetId() : paths[i - 1]->GetEnd().GetId();
		if (paths[i]->GetEnd().GetId() == previousExit) {
			reversedPaths[i] = true;
		}
	}
}

void Commute::SetTime(const Time& start) {
	if (currentPaths.size() == 0) {
		debugf("Warning: commute paths is empty when assigning start time.\n");
		return;
	}

	float dist = currentPaths[0]->CalcDistance();
	int seconds = (int)(dist * 10);
	currentEnd = start + Time(0, 1, 1, seconds / 3600, (seconds / 60) % 60, seconds % 60);
}

bool Commute::GetSimulate() {
	return simulate;
}

void Commute::SetSimulate(bool simulate) {
	this->simulate = simulate;
}

bool Commute::GetVisible() {
	return visible;
}

void Commute::StartVisible() {
	if (visible || !simulate)return;

	visible = true;
}

void Commute::EndVisible() {
	if (!visible)return;

	visible = false;
}

bool Commute::Tick(const Time& time) {
	while (time > currentEnd) {
		currentIdx++;
		if (currentIdx >= currentPaths.size()) {
			return true;
		}

		float dist = currentPaths[currentIdx]->CalcDistance();
		int seconds = (int)(dist * 10);
		currentEnd = currentEnd + Time(0, 1, 1, seconds / 3600, (seconds / 60) % 60, seconds % 60);
	}

	return false;
}

Connection* Commute::NextRoad(const Time& time) {
	currentIdx++;
	if (currentIdx >= currentPaths.size()) {
		return nullptr;
	}

	float dist = currentPaths[currentIdx]->CalcDistance();
	int seconds = (int)(dist * 10);
	currentEnd = time + Time(0, 1, 1, seconds / 3600, (seconds / 60) % 60, seconds % 60);

	return currentPaths[currentIdx];
}

pair<Connection*, float> Commute::RealtimeRoad(const Time& time) {
	float dist = currentPaths[currentIdx]->CalcDistance();
	float remaining = (currentEnd - time).GetOnlySecond() * 0.1f / dist;
	if (!reversedPaths[currentIdx]) {
		return { currentPaths[currentIdx], 1.f - remaining };
	}
	else {
		return { currentPaths[currentIdx], remaining };
	}
}
