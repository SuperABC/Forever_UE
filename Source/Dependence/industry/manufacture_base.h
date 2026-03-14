#pragma once

#include "storage_base.h"

#include <string>
#include <functional>
#include <memory>


class Storage;

class Manufacture {
public:
    Manufacture();
    virtual ~Manufacture();

    // 子类实现方法

    // 统一类型定义
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 产品线
    virtual std::unordered_map<std::string, float> ProductTargets() const = 0;

    // 父类实现方法
    
    // 计算全部输入输出
    void CalculateTargets(ProductFactory* factory);

	// 获取输入
    std::unordered_map<std::string, Storage*> GetInputs() const;

	// 设置输入
    void SetInput(const std::string& product, Storage* input, ProductFactory* factory);
    
	// 获取输出
    std::unordered_map<std::string, Storage*> GetOutputs() const;

	// 设置输出
    void SetOutput(const std::string& product, Storage* output, ProductFactory* factory);

	// 获取原料配比
	std::unordered_map<std::string, float> GetIngredients() const;
    
	// 获取产出配比
	std::unordered_map<std::string, float> GetTargets() const;
    
	// 获取副产物配比
	std::unordered_map<std::string, float> GetByproducts() const;

private:
    std::unordered_map<std::string, Storage*> inputs;
    std::unordered_map<std::string, Storage*> outputs;
	float currentInput;
    float currentOutput;
	float currentWorkload;

    std::unordered_map<std::string, float> ingredients;
    std::unordered_map<std::string, float> targets;
    std::unordered_map<std::string, float> byproducts;

    static std::unordered_map<std::string, std::unordered_map<std::string, float>> ingredientsCache;
    static std::unordered_map<std::string, std::unordered_map<std::string, float>> byproductsCache;
};

class ManufactureFactory {
public:
    // 注册制造
    void RegisterManufacture(const std::string& id,
        std::function<Manufacture*()> creator, std::function<void(Manufacture*)> deleter);

    // 创建制造（包含new操作）
    Manufacture* CreateManufacture(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 析构制造（包含delete操作）
    void DestroyManufacture(Manufacture* manufacture) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Manufacture*()>, std::function<void(Manufacture*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
};