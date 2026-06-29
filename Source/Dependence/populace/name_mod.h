#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class NameMod {
public:
	// 无构造
	NameMod();

	// 无析构
	virtual ~NameMod();

	/*
	* Override
	* 姓名静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 姓名动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 姓名实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 从姓名中获取姓
	* @set: 接收姓的回调
	* @name: 待提取的姓名
	*/
	COSTOM_RUNTIME virtual void GetSurname(std::function<void(const std::string&)> set,
		const std::string& name) = 0;

	/*
	* Override
	* 全随机生成姓名
	* @set: 接收生成结果的回调
	* @male, female, neutral: 是否允许使用男性、女性、中性名
	*/
	COSTOM_RUNTIME virtual void GenerateName(std::function<void(const std::string&)> set,
		bool male = true, bool female = true, bool neutral = true) = 0;

	/*
	* Override
	* 给定姓随机生成名
	* @set: 接收生成结果的回调
	* @surname: 指定的姓
	* @male, female, neutral: 是否允许使用男性、女性、中性名
	*/
	COSTOM_RUNTIME virtual void GenerateName(std::function<void(const std::string&)> set,
		const std::string& surname, bool male = true, bool female = true, bool neutral = true) = 0;
};

class NameFactory {
public:
	/*
	* 注册姓名
	* @id: 姓名静态类型标识
	* @creator, deleter: 构造与析构方法
	*/
	void RegisterName(const std::string& id,
		std::function<NameMod* ()> creator, std::function<void(NameMod*)> deleter);

	// 清空注册
	void RemoveAll();

	/*
	* 创建姓名
	* @id: 姓名静态类型标识
	*/
	NameMod* CreateName(const std::string& id) const;

	/*
	* 检查是否注册
	* @id: 姓名静态类型标识
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置启用配置
	* @name: 姓名静态类型标识
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	// 获取启用姓名
	std::string GetName() const;

	/*
	* 析构姓名
	* @nameMod: 待析构的姓名模组对象
	*/
	void DestroyName(NameMod* nameMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<NameMod* ()>, std::function<void(NameMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

