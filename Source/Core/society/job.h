#pragma once

#include "society/job_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModJobsFunc)(JobFactory* factory);

// 职业实体
class Job {
public:
	/*
	* 禁止默认构造
	*/
	Job() = delete;

	/*
	* 通过类型从工厂构造
	* @factory: 职业工厂
	* @job: 职业静态类型标识
	*/
	Job(JobFactory* factory, const std::string& job);

	/*
	* 析构职业
	*/
	~Job();

	/*
	* 获取类型
	*/
	std::string GetType() const;

	/*
	* 获取名称
	*/
	std::string GetName() const;

	/*
	* 获取日程
	*/
	Calendar* GetCalendar() const;

	/*
	* 设置日程
	* @calendar: 日程对象
	*/
	void SetCalendar(Calendar* calendar);

	/*
	* 获取剧本
	*/
	Script* GetScript() const;

	/*
	* 获取工作房间
	*/
	Room* GetPosition() const;

	/*
	* 设置工作房间
	* @room: 工作房间对象
	*/
	void SetPosition(Room* room);

	/*
	* 初始化职业
	* @name: 持有该职业的市民姓名
	* @idx: 职业在市民工作列表中的序号
	*/
	void InitJob(std::string name, int idx);

private:
	// 模组对象
	OBJECT_HOLDER JobMod* mod;

	// 工厂
	JobFactory* factory;

	// 职业类型
	std::string type;

	// 职业名称
	std::string name;

	// 职业日程
	OBJECT_HOLDER Calendar* calendar;

	// 关联剧情
	OBJECT_HOLDER Script* script;

	// 工作房间
	Room* position;
};

// 空职业
class EmptyJob : public JobMod {
public:
	/*
	* 构造空职业
	*/
	EmptyJob();

	/*
	* 析构空职业
	*/
	virtual ~EmptyJob();

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

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

