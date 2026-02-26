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

    // 动态返回组合静态信息
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 可存储类型
	virtual std::vector<std::string> GetCategories() const = 0;

    // 父类实现方法

	// 设置/获取内容
	void SetAccept(bool accept);
	bool GetAccept() const;
	void AddIncome(std::string product, float amount, ProductFactory* factory);
	Product* GetIncome(std::string product) const;
	std::vector<Product*> GetIncomes() const;
	void SetProvide(bool provide);
	bool GetProvide() const;
    void AddOutcome(std::string product, float amount, ProductFactory* factory);
    Product* GetOutcome(std::string product) const;
    std::vector<Product*> GetOutcomes() const;

	// 设置/获取连接
	void ConnectUpstream(Manufacture* manufacture);
	std::vector<Manufacture*> GetUpstreams() const;
	void ConnectDownstream(Manufacture* manufacture);
	std::vector<Manufacture*> GetDownstreams() const;

	// 设置/获取容量
    void SetVolume(float volume);
	float GetVolume() const;

protected:
	bool accept = true;
	bool provide = true;
	std::vector<Product*> incomes;
    std::vector<Product*> outcomes;

    std::vector<Manufacture*> upstreams;
	std::vector<Manufacture*> downstreams;

    float volume;
};

class StorageFactory {
public:
    void RegisterStorage(const std::string& id, std::function<Storage*()> creator);
    Storage* CreateStorage(const std::string& id);
    bool CheckRegistered(const std::string& id);
    void SetConfig(std::string name, bool config);

private:
    std::unordered_map<std::string, std::function<Storage*()>> registries;
    std::unordered_map<std::string, bool> configs;
};