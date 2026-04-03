#pragma once

#include "product_mod.h"


// 子类注册函数
typedef void (*RegisterModProductsFunc)(ProductFactory* factory);

// 产品实体
class Product {
public:
	// 禁止默认构造
	Product() = delete;

	// 通过类型从工厂构造
	Product(ProductFactory* factory, const std::string& component);

	// 析构产品
	~Product();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 设置参数
	void SetProperty();

	// 获取数量
	float GetAmount() const;

	// 设置数量
	void SetAmount(float amount);

	// 增加数量
	void IncreaseAmount(float amount);

	// 减少数量
	void DecreaseAmount(float amount);

	// 获取单价
	float GetPrice() const;

	// 设置单价
	void SetPrice(float price);

private:
	// 模组对象
	OBJECT_HOLDER ProductMod* mod;

	// 工厂
	ProductFactory* factory;

	// 产品类型
	std::string type;

	// 产品名称
	std::string name;

	// 所属分类
	std::vector<std::string> catagories;

	// 指导价
	float price;

	// 单位原材料
	std::unordered_map<std::string, float> ingredients;

	// 单位副产物
	std::unordered_map<std::string, float> byproducts;

	// 数量
	float amount;
};

// 空产品
class EmptyProduct : public ProductMod {
public:
	EmptyProduct();
	virtual ~EmptyProduct();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void SetProperty() override;

private:
	static int count;

	int id;
	std::string name;
};

