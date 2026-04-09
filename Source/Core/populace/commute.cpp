#include "commute.h"


using namespace std;

Commute::Commute() :
	simulate(true),
	visible(false),
	targetAddress(),
	currentPaths(),
	currentIdx(-1),
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

void Commute::SetPaths(const vector<pair<Connection*, pair<float, float>>>& paths) {
	currentPaths = paths;
}

void Commute::SetTime(const Time& start) {
	if (currentPaths.size() == 0) {
		debugf("Warning: commute paths is empty when assigning start time.\n");
		return;
	}

	float dist = currentPaths[0].first->CalcDistance() *
		abs(currentPaths[0].second.first - currentPaths[0].second.second);
	currentEnd = start + Time(0, 0, 0, 0, 0, (int)(dist * 10));
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

		float dist = currentPaths[currentIdx].first->CalcDistance() *
			abs(currentPaths[currentIdx].second.first - currentPaths[currentIdx].second.second);
		currentEnd = currentEnd + Time(0, 0, 0, 0, 0, (int)(dist * 10));
	}

	return false;
}

pair<Connection*, pair<float, float>> Commute::NextConnection(const Time& time) {
	currentIdx++;
	if (currentIdx >= currentPaths.size()) {
		return { nullptr, {0.f, 0.f} };
	}

	float dist = currentPaths[currentIdx].first->CalcDistance() *
		abs(currentPaths[currentIdx].second.first - currentPaths[currentIdx].second.second);
	currentEnd = time + Time(0, 0, 0, 0, 0, (int)(dist * 10));

	return currentPaths[currentIdx];
}

pair<Connection*, float> Commute::RealtimeConnection(const Time& time) {
	float dist = currentPaths[currentIdx].first->CalcDistance();
	float way = (currentEnd - time).GetOnlySecond() * 0.1f / dist;
	if (currentPaths[currentIdx].second.second > currentPaths[currentIdx].second.first) {
		return { currentPaths[currentIdx].first,
			currentPaths[currentIdx].second.second - (currentEnd - time).GetOnlySecond() * 0.1f / dist };
	}
	else {
		return { currentPaths[currentIdx].first,
			currentPaths[currentIdx].second.second + (currentEnd - time).GetOnlySecond() * 0.1f / dist };
	}
}


