#pragma once

#include "populace/name_mod.h"

#include "class.h"

#include <unordered_set>


// 子类注册函数
typedef void (*RegisterModNamesFunc)(NameFactory* factory);

// 姓名实体
class Name {
public:
	// 禁止默认构造
	Name() = delete;

	/*
	* 通过类型从工厂构造
	* @factory: 姓名工厂
	* @name: 姓名静态类型标识
	*/
	Name(NameFactory* factory, const std::string& name);

	// 析构姓名
	~Name();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	/*
	* 从姓名中获取姓
	* @set: 接收姓的回调
	* @name: 待提取的姓名
	*/
	void GetSurname(std::function<void(const std::string&)> set,
		const std::string& name);

	/*
	* 全随机生成姓名
	* @set: 接收生成结果的回调
	* @male, female, neutral: 是否允许使用男性、女性、中性名
	*/
	void GenerateName(std::function<void(const std::string&)> set,
		bool male = true, bool female = true, bool neutral = true);

	/*
	* 给定姓随机生成名
	* @set: 接收生成结果的回调
	* @surname: 指定的姓
	* @male, female, neutral: 是否允许使用男性、女性、中性名
	*/
	void GenerateName(std::function<void(const std::string&)> set,
		const std::string& surname, bool male = true, bool female = true, bool neutral = true);

	/*
	* 占用名字（不计入已生成名单，仅用于预留）
	* @name: 待占用的名字
	*/
	bool ReserveName(const std::string& name);

	/*
	* 使用名字（计入已生成名单，避免重复生成）
	* @name: 待使用的名字
	*/
	bool RegisterName(const std::string& name);

private:
	// 模组对象
	OBJECT_HOLDER NameMod* mod;

	// 工厂
	NameFactory* factory;

	// 姓名类型
	std::string type;

	// 姓名名称
	std::string name;

	// 预留（占用）的姓名集合，不会被当作已生成名字
	std::unordered_set<std::string> reserve;

	// 已生成过的姓名集合，避免重复
	std::unordered_set<std::string> roll;
};

// 空姓名
class EmptyName : public NameMod {
public:
	// 构造空姓名
	EmptyName();

	// 析构空姓名
	virtual ~EmptyName();

	/*
	* Override
	* 姓名静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 姓名动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 姓名实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 从姓名中获取姓
	* @set: 接收姓的回调
	* @name: 待提取的姓名
	*/
	virtual void GetSurname(std::function<void(const std::string&)> set,
		const std::string& name) override;

	/*
	* Override
	* 全随机生成姓名
	* @set: 接收生成结果的回调
	* @male, female, neutral: 是否允许使用男性、女性、中性名
	*/
	virtual void GenerateName(std::function<void(const std::string&)> set,
		bool male = true, bool female = true, bool neutral = true) override;

	/*
	* Override
	* 给定姓随机生成名
	* @set: 接收生成结果的回调
	* @surname: 指定的姓
	* @male, female, neutral: 是否允许使用男性、女性、中性名
	*/
	virtual void GenerateName(std::function<void(const std::string&)> set,
		const std::string& surname, bool male = true, bool female = true, bool neutral = true) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

