#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class ProductMod {
public:
	// 无构造
	ProductMod();

	// 无析构
	virtual ~ProductMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 设置参数
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
	// 注册产品
	void RegisterProduct(const std::string& id,
		std::function<ProductMod* ()> creator, std::function<void(ProductMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建产品
	ProductMod* CreateProduct(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 析构产品
	void DestroyProduct(ProductMod* terrain) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<ProductMod* ()>, std::function<void(ProductMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

