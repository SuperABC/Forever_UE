#include "commute.h"


using namespace std;

Commute::Commute() :
	simulate(true),
	visible(false),
	targetAddress(),
	currentPaths(),
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

void Commute::SetPaths(const vector<pair<Connection*, bool>>& paths) {
	currentPaths = paths;
}

const vector<pair<Connection*, bool>>& Commute::GetPaths() const {
	return currentPaths;
}

void Commute::SetTime(const Time& start) {
	if (currentPaths.size() == 0) {
		debugf("Warning: commute paths is empty when assigning start time.\n");
		return;
	}
	if (!currentPaths[0].first) {
		debugf("Warning: commute first path connection is null.\n");
		return;
	}

	float dist = currentPaths[0].first->CalcDistance();
	int seconds = (int)(dist * 10);
	currentEnd = start + Time(0, 1, 1, seconds / 3600, (seconds / 60) % 60, seconds % 60);
}

bool Commute::GetSimulate() const {
	return simulate;
}

void Commute::SetSimulate(bool simulate) {
	this->simulate = simulate;
}

bool Commute::GetVisible() const {
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
		if (!currentPaths[currentIdx].first) {
			return true;
		}

		float dist = currentPaths[currentIdx].first->CalcDistance();
		int seconds = (int)(dist * 1); //为测试方便，几乎瞬间完成通勤
		currentEnd = currentEnd + Time(0, 1, 1, seconds / 3600, (seconds / 60) % 60, seconds % 60);
	}

	return false;
}

Connection* Commute::NextRoad(const Time& time) {
	currentIdx++;
	if (currentIdx >= currentPaths.size()) {
		return nullptr;
	}
	if (!currentPaths[currentIdx].first) {
		return nullptr;
	}

	float dist = currentPaths[currentIdx].first->CalcDistance();
	int seconds = (int)(dist * 10);
	currentEnd = time + Time(0, 1, 1, seconds / 3600, (seconds / 60) % 60, seconds % 60);

	return currentPaths[currentIdx].first;
}

pair<Connection*, float> Commute::RealtimeRoad(const Time& time) {
	if (!currentPaths[currentIdx].first) {
		return { nullptr, 0.f };
	}
	float dist = currentPaths[currentIdx].first->CalcDistance();
	float remaining = (currentEnd - time).GetOnlySecond() * 0.1f / dist;
	if (!currentPaths[currentIdx].second) {
		return { currentPaths[currentIdx].first, 1.f - remaining };
	}
	else {
		return { currentPaths[currentIdx].first, remaining };
	}
}
