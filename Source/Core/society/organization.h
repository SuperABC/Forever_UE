#pragma once

#include "society/organization_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModOrganizationsFunc)(OrganizationFactory* factory);

// 子类注册收尾函数
typedef void (*FinishModOrganizationsFunc)(OrganizationFactory* factory);

// 组织实体
class Organization {
public:
	/*
	* 禁止默认构造
	*/
	Organization() = delete;

	/*
	* 通过类型从工厂构造
	* @factory: 组织工厂
	* @organization: 组织静态类型标识
	*/
	Organization(OrganizationFactory* factory, const std::string& organization);

	/*
	* 析构组织
	*/
	~Organization();

	/*
	* 获取类型
	*/
	std::string GetType() const;

	/*
	* 获取名称
	*/
	std::string GetName() const;

	/*
	* 返回每个需要的组合类型与组合数量上下限
	*/
	std::unordered_map<std::string, std::pair<int, int>> ComponentRequirements() const;

	/*
	* 根据实际组合数量安排工作岗位
	* @components: 各组合类型的实际数量
	*/
	std::unordered_map<std::string, std::vector<std::vector<std::pair<std::string, std::string>>>> ArrageVacancies(
		const std::unordered_map<std::string, int>& components);

	/*
	* 添加职缺
	* @component: 所属组合
	* @vacancies: 职位列表
	*/
	void AddVacancy(Component* component, const std::vector<Job*>& vacancies);

	/*
	* 员工入职
	* @candidates: 候选人列表
	*/
	std::vector<Job*> EnrollEmployee(const std::vector<Person*>& candidates);

	/*
	* 为所有职位分配工位
	*/
	void ArrangeRooms();

	/*
	* 获取全部职位（组合 -> [(工作, 员工)]）
	*/
	std::vector<std::pair<Component*, std::vector<std::pair<Job*, Person*>>>> GetJobs() const;

	/*
	* 获取关联剧情
	*/
	Script* GetScript() const;

	/*
	* 初始化组织，创建关联剧情对象
	*/
	void InitOrganization();

	/*
	* 每日规划，委托模组填充plans字段
	* @time: 当前游戏时间
	* @post: 向Core发起查询的句柄
	*/
	void DailyPlan(const Time& time, PostHandle* post);

	/*
	* 规划节点调用，返回节点产出的变化列表（由调用方负责释放）
	* @node: 节点名称
	* @storyScript: 主线剧情脚本
	* @post: 向Core发起查询的句柄
	*/
	std::vector<Change*> ExecNode(const std::string& node, Script* storyScript, PostHandle* post);

	/*
	* 获取今日计划（节点名 -> 触发时刻）
	*/
	const std::unordered_map<std::string, Time>& GetPlans() const;

private:
	// 模组对象
	OBJECT_HOLDER OrganizationMod* mod;

	// 工厂
	OrganizationFactory* factory;

	// 组织类型
	std::string type;

	// 组织名称
	std::string name;

	// 工号上限
	int count;

	// 员工
	OBJECT_HOLDER std::vector<std::pair<Component*, std::vector<std::pair<Job*, Person*>>>> jobs;

	// 关联剧情
	OBJECT_HOLDER Script* script;
};

// 空组织
class EmptyOrganization : public OrganizationMod {
public:
	/*
	* 构造空组织
	*/
	EmptyOrganization();

	/*
	* 析构空组织
	*/
	virtual ~EmptyOrganization();

	/*
	* Override
	* 组织静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 组织动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 组织实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 组织分配权重
	*/
	static float GetPower();

	/*
	* Override
	* 设置所需组合及数量范围
	*/
	virtual void ComponentRequirements() override;

	/*
	* Override
	* 根据实际组合数量安排工作岗位与日程
	* @components: 各组合类型的实际数量
	*/
	virtual void ArrageVacancies(const std::unordered_map<std::string, int>& components) override;

	/*
	* Override
	* 为工作岗位分配工位
	* @arrangements: 各岗位类型与分配到的工位序号
	* @rooms: 可用工位类型列表
	*/
	virtual void ArrageRoom(std::vector<std::pair<std::string, int>>& arrangements,
		const std::vector<std::string>& rooms) override;

	/*
	* Override
	* 初始化组织
	*/
	virtual void InitOrganization() override;

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
	* @storyScript, orgScript: 主线剧情脚本、组织自身剧情脚本
	* @post: 向Core发起查询的句柄
	*/
	virtual void ExecNode(const std::string& name,
		Container* storyScript, Container* orgScript, PostHandle* post) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

