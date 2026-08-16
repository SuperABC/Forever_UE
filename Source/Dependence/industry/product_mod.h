#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


// 产品Mod基类
class ProductMod {
public:
	/*
	* 构造产品
	*/
	ProductMod();

	/*
	* 析构产品
	*/
	virtual ~ProductMod();

	/*
	* Override
	* 产品静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 产品动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 产品实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 设置产品参数
	*/
	COSTOM_INIT virtual void SetProperty() = 0;

	// 所属分类
	std::vector<std::string> categories;

	// 指导价
	float price;

	// 单位原材料
	std::unordered_map<std::string, float> ingredients;

	// 单位副产物
	std::unordered_map<std::string, float> byproducts;

};

class ProductFactory {
public:
	/*
	* 构造产品工厂,预留暂存区容量
	*/
	ProductFactory();

	/*
	* 注册产品
	* @id: 产品类型
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterProduct(const std::string& id,
		std::function<ProductMod* ()> creator, std::function<void(ProductMod*)> deleter);

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
	* 创建产品
	* @id: 产品类型
	*/
	ProductMod* CreateProduct(const std::string& id) const;

	/*
	* 检查产品是否已注册
	* @id: 产品类型
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置产品启用配置
	* @name: 产品类型
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 析构产品
	* @productMod: 待析构的产品
	*/
	void DestroyProduct(ProductMod* productMod) const;

private:
	// 暂存本轮注册写入的数据,避免跨模块直接写正式成员
	struct Temp {
		// 注册表
		std::unordered_map<
			std::string,
			std::pair<std::function<ProductMod* ()>, std::function<void(ProductMod*)>>
		> registries;
	};

	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<ProductMod* ()>, std::function<void(ProductMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;

	// 暂存数据
	Temp temp;

};
