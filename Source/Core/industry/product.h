#pragma once

#include "industry/product_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModProductsFunc)(ProductFactory* factory);

// 产品实体
class Product {
public:
	/*
	* 禁止默认构造
	*/
	Product() = delete;

	/*
	* 通过类型从工厂构造
	* @factory: 产品工厂
	* @product: 产品静态类型标识
	*/
	Product(ProductFactory* factory, const std::string& product);

	/*
	* 析构产品
	*/
	~Product();

	/*
	* 获取类型
	*/
	std::string GetType() const;

	/*
	* 获取名称
	*/
	std::string GetName() const;

	/*
	* 设置参数
	*/
	void SetProperty();

	/*
	* 获取产品分类
	*/
	std::vector<std::string> GetCategories() const;

	/*
	* 获取单价
	*/
	float GetPrice() const;

	/*
	* 设置单价
	* @price: 新单价
	*/
	void SetPrice(float price);

	/*
	* 获取原材料列表
	*/
	std::unordered_map<std::string, float> GetIngredients() const;

	/*
	* 获取副产物列表
	*/
	std::unordered_map<std::string, float> GetByproducts() const;

	/*
	* 获取数量
	*/
	float GetAmount() const;

	/*
	* 设置数量
	* @amount: 新数量
	*/
	void SetAmount(float amount);

	/*
	* 增加数量
	* @amount: 增加量
	*/
	void IncreaseAmount(float amount);

	/*
	* 减少数量
	* @amount: 减少量
	*/
	void DecreaseAmount(float amount);

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
	std::vector<std::string> categories;

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
	/*
	* 构造空产品
	*/
	EmptyProduct();

	/*
	* 析构空产品
	*/
	virtual ~EmptyProduct();

	/*
	* Override
	* 产品静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 产品动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 产品实例名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 设置产品参数
	*/
	virtual void SetProperty() override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;

};
