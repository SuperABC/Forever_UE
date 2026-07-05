#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class ProductMod {
public:
	/*
	* 构造产品模组基类
	*/
	ProductMod();

	/*
	* 析构产品模组基类
	*/
	virtual ~ProductMod();

	/*
	* Override
	* 产品静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 产品动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 产品实例名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 设置产品参数
	*/
	COSTOM_INIT virtual void SetProperty() = 0;

	// 所属分类
	std::vector<std::string> catagories;

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
	* 注册产品
	* @id: 产品静态类型标识
	* @creator: 创建函数
	* @deleter: 析构函数
	*/
	void RegisterProduct(const std::string& id,
		std::function<ProductMod* ()> creator, std::function<void(ProductMod*)> deleter);

	/*
	* 清空启用配置
	*/
	void RemoveAll();

	/*
	* 创建产品实例
	* @id: 产品静态类型标识
	*/
	ProductMod* CreateProduct(const std::string& id) const;

	/*
	* 检查是否已注册
	* @id: 产品静态类型标识
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置启用配置
	* @name: 产品静态类型标识
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 析构产品实例
	* @productMod: 待析构的产品模组
	*/
	void DestroyProduct(ProductMod* productMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<ProductMod* ()>, std::function<void(ProductMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;

};
