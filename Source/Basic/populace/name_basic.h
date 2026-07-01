#pragma once

#include "populace/name_mod.h"


// 中文姓名
class ChineseName : public NameMod {
public:
	/*
	* 构造中文姓名
	*/
	ChineseName();

	/*
	* 析构中文姓名
	*/
	virtual ~ChineseName();

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
	/*
	* 初始化姓氏库
	*/
	void InitializeSurnames();

	/*
	* 初始化名字库
	*/
	void InitializeNames();

	// 姓氏库
	std::vector<std::string> surnames;

	// 男性名字库
	std::vector<std::string> maleNames;

	// 女性名字库
	std::vector<std::string> femaleNames;

	// 中性名字库
	std::vector<std::string> neutralNames;

	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};
