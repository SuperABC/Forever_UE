#pragma once

#include "product_base.h"
#include "manufacture_base.h"

#include <string>
#include <functional>
#include <memory>


class Manufacture;

class Storage {
public:
    Storage();
    virtual ~Storage();

    // 子类实现方法

    // 统一类型定义
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 可存储类型
	virtual std::vector<std::string> GetCategories() const = 0;

    // 父类实现方法

	// 获取是否接收产品
	bool GetAccept() const;
    
	// 设置是否接收产品
	void SetAccept(bool accept);
    
	// 获取一种接收产品
	Product* GetIncome(const std::string& product) const;
    
	// 获取全部接收产品
	std::vector<Product*> GetIncomes() const;
    
	// 新增接收产品种类
	void AddIncome(const std::string& product, float amount, ProductFactory* factory);
    
	// 获取是否提供产品
	bool GetProvide() const;

	// 设置是否提供产品
	void SetProvide(bool provide);
    
	// 获取一种提供产品
    Product* GetOutcome(const std::string& product) const;
    
	// 获取全部提供产品
    std::vector<Product*> GetOutcomes() const;
    
	// 新增提供产品种类
    void AddOutcome(const std::string& product, float amount, ProductFactory* factory);

	// 获取上游工坊
	std::vector<Manufacture*> GetUpstreams() const;
    
	// 设置上游工坊
	void ConnectUpstream(Manufacture* manufacture);
    
	// 获取下游工坊
	std::vector<Manufacture*> GetDownstreams() const;

	// 设置下游工坊
	void ConnectDownstream(Manufacture* manufacture);

	// 获取容量
	float GetVolume() const;

	// 设置容量
    void SetVolume(float volume);
    
private:
	bool accept;
	bool provide;
	std::vector<Product*> incomes;
    std::vector<Product*> outcomes;

    std::vector<Manufacture*> upstreams;
	std::vector<Manufacture*> downstreams;

    float volume;
};

class StorageFactory {
public:
    // 注册存储
    void RegisterStorage(const std::string& id,
        std::function<Storage*()> creator, std::function<void(Storage*)> deleter);

    // 创建存储（包含new操作）
    Storage* CreateStorage(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 析构存储（包含delete操作）
    void DestroyStorage(Storage* storage) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Storage*()>, std::function<void(Storage*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
};

