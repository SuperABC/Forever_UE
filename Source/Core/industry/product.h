#pragma once

#include "product_base.h"

#include <string>
#include <vector>


// 子类注册函数
typedef void (*RegisterModProductsFunc)(ProductFactory* factory);

// 标准双休日程
class DefaultProduct : public Product {
public:
	static std::string GetId();
	virtual std::string GetType() const override;
	virtual std::string GetName() const override;

    virtual std::vector<std::string> GetCategories() const override;

    virtual float UnitVolume() const override;

    virtual float StandardPrice() const override;

    virtual std::unordered_map<std::string, float> GetIngredients() const override;
    virtual std::unordered_map<std::string, float> GetByproducts() const override;

};