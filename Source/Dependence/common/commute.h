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
	bool IsNull();

	// 通勤目标
	void SetTarget(Room* target);
	Room* GetTarget();

	// 通勤开始
	void SetPaths(std::vector<Connection> paths);
	void StartCommute(Time time);

	// 通勤结束
	void FinishCommute();

	// 实时位置
	std::pair<Connection, float> RealtimeConnection(Time time);
	bool AtConnection(Connection connection, Time time) const;

private:
	std::vector<Connection> currentPaths;
	Time startTime;
	Room* targetRoom;
};