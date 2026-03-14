#pragma once

#include "../map/roadnet_base.h"

#include "utility.h"

#include <vector>


class Commute {
public:
	Commute();
	~Commute();

	// 通勤有效性
	void SetNull();
	bool IsNull() const;

	// 通勤目标
	Room* GetTarget() const;
	void SetTarget(Room* target);

	// 通勤开始
	void SetPaths(const std::vector<Connection>& paths);
	void StartCommute(Time time);

	// 通勤结束
	void FinishCommute();

	// 实时位置
	std::pair<Connection, float> RealtimeConnection(const Time& time);
	bool AtConnection(const Connection& connection, const Time& time) const;

private:
	std::vector<Connection> currentPaths;
	Time startTime;
	Room* targetRoom;
};
