#pragma once

#include "map/geometry.h"


class Commute {
public:
	// 构造通勤
	Commute();

	// 析构通勤
	~Commute();

	// 获取通勤目标
	std::string GetTarget() const;

	// 设置通勤目标
	void SetTarget(const std::string& target);

	// 设置通勤路线
	void SetPaths(const std::vector<std::pair<Connection*, std::pair<float, float>>>& paths);

	// 设置开始通勤时间
	void SetTime(const Time& start);

	// 获取是否接入路线模拟
	bool GetSimulate();

	// 设置是否接入路线模拟
	void SetSimulate(bool simulate);

	// 获取是否已在视线范围内
	bool GetVisible();

	// 进入视线范围
	void StartVisible();

	// 离开视线范围
	void EndVisible();

	// 自动前进，并返回是否完成全部路线
	bool Tick(const Time& time);

	// 模拟且可见状态下手动完成当前连接并进入下一连接
	std::pair<Connection*, std::pair<float, float>> NextConnection(const Time& time);

	// 获取实时位置
	std::pair<Connection*, float> RealtimeConnection(const Time& time);

private:
	// 是否接入路线模拟，如果不接入则所有连接定时到达且不可见，如果接入则在可见时按实际模拟结果到达
	bool simulate;

	// 是否已在视线范围内，如果不在则定时进入下一连接，如果在则按实际模拟结果进入下一连接
	bool visible;

	// 目标地址
	std::string targetAddress;

	// 路线
	std::vector<std::pair<Connection*, std::pair<float, float>>> currentPaths;

	// 当前所在连接索引
	int currentIdx;

	// 当前所在连接计划完成时间
	Time currentEnd;
};
