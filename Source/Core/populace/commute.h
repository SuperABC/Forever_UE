#pragma once

#include "map/geometry.h"


class Commute {
public:
	/*
	* 构造通勤
	*/
	Commute();

	/*
	* 析构通勤
	*/
	~Commute();

	/*
	* 获取通勤目标
	*/
	std::string GetTarget() const;

	/*
	* 设置通勤目标
	* @target: 目标地址
	*/
	void SetTarget(const std::string& target);

	/*
	* 设置通勤路线（每段均从起点完整通行至终点）
	* @paths: 路线连接列表
	*/
	void SetPaths(const std::vector<Connection*>& paths);

	/*
	* 获取通勤路线，每段为连接及其实际通行方向是否与其自身起点->终点相反
	*/
	const std::vector<std::pair<Connection*, bool>>& GetPaths() const;

	/*
	* 设置开始通勤时间
	* @start: 开始时间
	*/
	void SetTime(const Time& start);

	/*
	* 获取是否接入路线模拟
	*/
	bool GetSimulate() const;

	/*
	* 设置是否接入路线模拟
	* @simulate: 是否接入
	*/
	void SetSimulate(bool simulate);

	/*
	* 获取是否已在视线范围内
	*/
	bool GetVisible() const;

	/*
	* 进入视线范围
	*/
	void StartVisible();

	/*
	* 离开视线范围
	*/
	void EndVisible();

	/*
	* 自动前进，并返回是否完成全部路线
	* @time: 当前时间
	*/
	bool Tick(const Time& time);

	/*
	* 模拟且可见状态下手动完成当前连接并进入下一连接
	* @time: 当前时间
	*/
	Connection* NextRoad(const Time& time);

	/*
	* 获取实时位置
	* @time: 当前时间
	*/
	std::pair<Connection*, float> RealtimeRoad(const Time& time);

private:
	// 是否接入路线模拟，如果不接入则所有连接定时到达且不可见，如果接入则在可见时按实际模拟结果到达
	bool simulate;

	// 是否已在视线范围内，如果不在则定时进入下一连接，如果在则按实际模拟结果进入下一连接
	bool visible;

	// 目标地址
	std::string targetAddress;

	// 路线，每段为连接及其实际通行方向是否与其自身起点->终点相反
	std::vector<std::pair<Connection*, bool>> currentPaths;

	// 当前所在连接索引
	int currentIdx;

	// 当前所在连接计划完成时间
	Time currentEnd;
};
