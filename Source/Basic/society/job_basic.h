#pragma once

#include "society/job_mod.h"


class ShopSalerJob : public JobMod {
public:
	/*
	* 构造售货员职业
	*/
	ShopSalerJob();

	/*
	* 析构售货员职业
	*/
	virtual ~ShopSalerJob();

	/*
	* Override
	* 职业静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 职业动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 职业实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 初始化职业
	*/
	virtual void InitJob() override;

	/*
	* Override
	* 每日规划
	* @time: 当前游戏时间
	* @post: 向Core发起查询的句柄
	*/
	virtual void DailyPlan(const Time& time, PostHandle* post) override;

	/*
	* Override
	* 规划节点调用
	* @name: 节点名称
	* @storyScript, schedulerScript, jobScript: 主线剧情脚本、市民调度脚本、职业自身剧情脚本
	* @post: 向Core发起查询的句柄
	*/
	virtual void ExecNode(const std::string& name,
		Container* storyScript, Container* schedulerScript, Container* jobScript,
		PostHandle* post) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

// 医院前台职业
class HospitalReceptionJob : public JobMod {
public:
	/*
	* 构造医院前台职业
	*/
	HospitalReceptionJob();

	/*
	* 析构医院前台职业
	*/
	virtual ~HospitalReceptionJob();

	/*
	* Override
	* 职业静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 职业动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 职业实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 初始化职业
	*/
	virtual void InitJob() override;

	/*
	* Override
	* 每日规划
	* @time: 当前游戏时间
	* @post: 向Core发起查询的句柄
	*/
	virtual void DailyPlan(const Time& time, PostHandle* post) override;

	/*
	* Override
	* 规划节点调用
	* @name: 节点名称
	* @storyScript, schedulerScript, jobScript: 主线剧情脚本、市民调度脚本、职业自身剧情脚本
	* @post: 向Core发起查询的句柄
	*/
	virtual void ExecNode(const std::string& name,
		Container* storyScript, Container* schedulerScript, Container* jobScript,
		PostHandle* post) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

// 门诊医生职业
class OutpatientDoctorJob : public JobMod {
public:
	/*
	* 构造门诊医生职业
	*/
	OutpatientDoctorJob();

	/*
	* 析构门诊医生职业
	*/
	virtual ~OutpatientDoctorJob();

	/*
	* Override
	* 职业静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 职业动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 职业实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 初始化职业
	*/
	virtual void InitJob() override;

	/*
	* Override
	* 每日规划
	* @time: 当前游戏时间
	* @post: 向Core发起查询的句柄
	*/
	virtual void DailyPlan(const Time& time, PostHandle* post) override;

	/*
	* Override
	* 规划节点调用
	* @name: 节点名称
	* @storyScript, schedulerScript, jobScript: 主线剧情脚本、市民调度脚本、职业自身剧情脚本
	* @post: 向Core发起查询的句柄
	*/
	virtual void ExecNode(const std::string& name,
		Container* storyScript, Container* schedulerScript, Container* jobScript,
		PostHandle* post) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};
