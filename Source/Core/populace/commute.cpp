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

void Commute::SetPaths(const vector<Connection*>& paths) {
	currentPaths.clear();
	for (auto path : paths) {
		currentPaths.emplace_back(path, false);
	}

	// 根据相邻连接共享的端点，推算每段连接的实际通行方向（第一段默认按其自身起点->终点方向通行）
	for (size_t i = 1; i < currentPaths.size(); i++) {
		if (!currentPaths[i - 1].first || !currentPaths[i].first) continue;
		int previousExit = currentPaths[i - 1].second ?
			currentPaths[i - 1].first->GetStart().GetId() : currentPaths[i - 1].first->GetEnd().GetId();
		if (currentPaths[i].first->GetEnd().GetId() == previousExit) {
			currentPaths[i].second = true;
		}
	}
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
