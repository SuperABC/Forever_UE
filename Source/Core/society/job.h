#pragma once

#include "story/story.h"

#include "job_mod.h"

#include "calendar.h"


// 子类注册函数
typedef void (*RegisterModJobsFunc)(JobFactory* factory);

// 职业实体
class JobMod;
class Script;
class JobFactory;
class Job {
public:
	// 禁止默认构造
	Job() = delete;

	// 通过类型从工厂构造
	Job(JobFactory* factory, const std::string& component);

	// 析构职业
	~Job();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 获取日程
	Calendar* GetCalendar() const;

	// 设置日程
	void SetCalendar(Calendar* calendar);

	// 获取剧本
	Script* GetScript() const;

    // 获取工作房间
    Room* GetPosition() const;

    // 设置工作房间
    void SetPosition(Room* room);

	// 初始化职业
	void InitJob(std::string name);

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
	EmptyJob();
	virtual ~EmptyJob();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void InitJob() override;

private:
	static int count;

	int id;
	std::string name;
};

