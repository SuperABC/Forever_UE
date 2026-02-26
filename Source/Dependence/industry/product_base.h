#pragma once

#include <string>
#include <functional>
#include <memory>
#include <vector>


class Product {
public:
    Product() = default;
    virtual ~Product() = default;

    // 子类实现方法

    // 动态返回组合静态信息
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 所有适配仓库分类
    virtual std::vector<std::string> GetCategories() const = 0;

    // 单位体积
    virtual float UnitVolume() const = 0;

    // 指导价
    virtual float StandardPrice() const = 0;

    // 材料/产出列表
    virtual std::unordered_map<std::string, float> GetIngredients() const = 0;
    virtual std::unordered_map<std::string, float> GetByproducts() const = 0;

    // 父类实现方法

	// 获取/设置数量/单价
    void SetAmount(float amount);
	void IncreaseAmount(float amount);
	void DecreaseAmount(float amount);
	float GetAmount() const;
	void SetPrice(float price);
	float GetPriceValue() const;

protected:
    float amount;
    float price;
};

class ProductFactory {
public:
    void RegisterProduct(const std::string& id, std::function<Product*()> creator);
    Product* CreateProduct(const std::string& id);
    bool CheckRegistered(const std::string& id);
    void SetConfig(std::string name, bool config);

private:
    std::unordered_map<std::string, std::function<Product*()>> registries;
    std::unordered_map<std::string, bool> configs;
};