#pragma once

#include "product_base.h"

#include <memory>
#include <string>


// 模组检测子类
class ModProduct : public Product {
public:
    static std::string GetId() { return "mod"; }
    virtual std::string GetType() const override { return "mod"; }
    virtual std::string GetName() const override { return "模组产品"; }

    virtual std::vector<std::string> GetCategories() const override {
        return { "模组分类" };
    }

    virtual float UnitVolume() const override {
		return 1.f;
    }

    virtual float StandardPrice() const override {
        return 1.f;
    }

    virtual std::unordered_map<std::string, float> GetIngredients() const override {
        return {};
    }
    virtual std::unordered_map<std::string, float> GetByproducts() const override {
        return {};
    }

};

