#pragma once

#include "../common/utility.h"
#include "../common/error.h"
#include "../common/handle.h"

#include <string>
#include <functional>


// 姓名Mod基类
class NameMod {
public:
	/*
	* 构造姓名
	*/
	NameMod();

	/*
	* 析构姓名
	*/
	virtual ~NameMod();

	/*
	* Override
	* 姓名静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 姓名动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 姓名实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 从姓名中获取姓
	* @set: 接收姓的回调
	* @name: 待提取的姓名
	* @post: 向Core发起查询的句柄
	*/
	COSTOM_RUNTIME virtual void GetSurname(std::function<void(const std::string&)> set,
		const std::string& name, PostHandle* post) = 0;

	/*
	* Override
	* 全随机生成姓名
	* @set: 接收生成结果的回调
	* @male, female, neutral: 是否允许使用男性、女性、中性名
	* @post: 向Core发起查询的句柄
	*/
	COSTOM_RUNTIME virtual void GenerateName(std::function<void(const std::string&)> set,
		bool male, bool female, bool neutral, PostHandle* post) = 0;

	/*
	* Override
	* 给定姓随机生成名
	* @set: 接收生成结果的回调
	* @surname: 指定的姓
	* @male, female, neutral: 是否允许使用男性、女性、中性名
	* @post: 向Core发起查询的句柄
	*/
	COSTOM_RUNTIME virtual void GenerateName(std::function<void(const std::string&)> set,
		const std::string& surname, bool male, bool female, bool neutral, PostHandle* post) = 0;
};

class NameFactory {
public:
	/*
	* 构造姓名工厂,预留暂存区容量
	*/
	NameFactory();

	/*
	* 注册姓名
	* @id: 姓名类型
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterName(const std::string& id,
		std::function<NameMod* ()> creator, std::function<void(NameMod*)> deleter);

	/*
	* 将暂存数据合并进正式注册表
	*/
	void MergeTemp();

	/*
	* 清空暂存数据
	*/
	void CleanTemp();

	/*
	* 清空所有注册
	*/
	void RemoveAll();

	/*
	* 创建姓名
	* @id: 姓名类型
	*/
	NameMod* CreateName(const std::string& id) const;

	/*
	* 检查姓名是否已注册
	* @id: 姓名类型
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置姓名启用配置
	* @name: 姓名类型
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 获取启用姓名
	*/
	std::string GetName() const;

	/*
	* 析构姓名
	* @nameMod: 待析构的姓名
	*/
	void DestroyName(NameMod* nameMod) const;

private:
	// 暂存本轮注册写入的数据,避免跨模块直接写正式成员
	struct Temp {
		// 注册表
		std::unordered_map<
			std::string,
			std::pair<std::function<NameMod* ()>, std::function<void(NameMod*)>>
		> registries;
	};

	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<NameMod* ()>, std::function<void(NameMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;

	// 暂存数据
	Temp temp;
};

