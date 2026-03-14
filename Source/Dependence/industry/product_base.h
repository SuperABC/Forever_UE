#pragma once

#include <string>
#include <functional>
#include <memory>
#include <vector>


class Product {
public:
    Product();
    virtual ~Product();

    // 子类实现方法

    // 统一类型定义
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 所有适配仓库分类
    virtual std::vector<std::string> GetCategories() const = 0;

    // 单位体积
    virtual float UnitVolume() const = 0;

    // 指导价
    virtual float StandardPrice() const = 0;

    // 材料列表
    virtual std::unordered_map<std::string, float> GetIngredients() const = 0;
    
    // 副产物列表
    virtual std::unordered_map<std::string, float> GetByproducts() const = 0;

    // 父类实现方法

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
    float amount;
    float price;
};

class ProductFactory {
public:
    // 注册产品
    void RegisterProduct(const std::string& id,
        std::function<Product*()> creator, std::function<void(Product*)> deleter);

    // 创建产品（包含new操作）
    Product* CreateProduct(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 析构产品（包含delete操作）
    void DestroyProduct(Product* product) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Product*()>, std::function<void(Product*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
};

