#pragma once

#include "../common/error.h"

#include "storage_base.h"

#include <string>
#include <functional>
#include <memory>


class Storage;

class Manufacture {
public:
    Manufacture() = default;
    virtual ~Manufacture() = default;

    // 子类实现方法

    // 动态返回组合静态信息
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 产品线
    virtual std::unordered_map<std::string, float> ProductTargets() const = 0;

    // 父类实现方法
    
    // 计算全部输入输出
    void CalculateTargets(ProductFactory* factory);

	// 设置/获取输入/输出
    void SetInput(std::string product, Storage* input, ProductFactory* factory);
    std::unordered_map<std::string, Storage*> GetInputs() const;
    void SetOutput(std::string product, Storage* output, ProductFactory* factory);
    std::unordered_map<std::string, Storage*> GetOutputs() const;

	// 获取单位产品
	std::unordered_map<std::string, float> GetIngredients() const;
	std::unordered_map<std::string, float> GetTargets() const;
	std::unordered_map<std::string, float> GetByproducts() const;

protected:
    std::unordered_map<std::string, Storage*> inputs;
    std::unordered_map<std::string, Storage*> outputs;
	float currentInput = 0.f;
    float currentOutput = 0.f;
	float currentWorkload = 0.f;

    std::unordered_map<std::string, float> ingredients;
    std::unordered_map<std::string, float> targets;
    std::unordered_map<std::string, float> byproducts;

    static std::unordered_map<std::string, std::unordered_map<std::string, float>> ingredientsCache;
    static std::unordered_map<std::string, std::unordered_map<std::string, float>> byproductsCache;
};

class ManufactureFactory {
public:
    void RegisterManufacture(const std::string& id, std::function<Manufacture*()> creator);
    Manufacture* CreateManufacture(const std::string& id);
    bool CheckRegistered(const std::string& id);
    void SetConfig(std::string name, bool config);

private:
    std::unordered_map<std::string, std::function<Manufacture*()>> registries;
    std::unordered_map<std::string, bool> configs;
};