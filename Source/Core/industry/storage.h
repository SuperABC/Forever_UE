#pragma once

#include "storage_mod.h"

#include "product.h"
#include "manufacture.h"


// 子类注册函数
typedef void (*RegisterModStoragesFunc)(StorageFactory* factory);

// 仓库实体
class Storage {
public:
	// 禁止默认构造
	Storage() = delete;

	// 通过类型从工厂构造
	Storage(StorageFactory* factory, const std::string& component);

	// 析构仓库
	~Storage();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 设置参数
	void SetProperty(float acreage);

	// 获取支持产品类型
	std::vector<std::string> GetCategories() const;

	// 获取是否接收产品
	bool GetAccept() const;

	// 设置是否接收产品
	void SetAccept(bool accept);

	// 获取是否提供产品
	bool GetProvide() const;

	// 设置是否提供产品
	void SetProvide(bool provide);

	// 获取一种提供产品
	Product* GetProduct(const std::string& type) const;

	// 获取全部提供产品
	std::unordered_map<std::string, Product*> GetProducts() const;

	// 输入产品种类
	float InputProduct(const std::string& product, float amount);

	// 输入产品种类
	float OutputProduct(const std::string& product, float amount);

	// 获取上游仓库
	std::unordered_map<std::string, Storage*> GetUpstreams() const;

	// 设置上游仓库
	void ConnectUpstream(std::string type, Storage* storage);

	// 获取下游仓库
	std::unordered_map<std::string, Storage*> GetDownstreams() const;

	// 设置下游仓库
	void ConnectDownstream(std::string type, Storage* storage);

	// 获取容量
	float GetVolume() const;

	// 设置容量
	void SetVolume(float volume);

	// 获取剩余空间
	float GetSpace() const;

	// 获取占用空间
	float GetOccupied() const;

private:
	// 模组对象
	OBJECT_HOLDER StorageMod* mod;

	// 工厂
	StorageFactory* factory;

	// 仓库类型
	std::string type;

	// 仓库名称
	std::string name;

	// 支持产品类型
	std::vector<std::string> catagories;

	// 是否接入系统物流输入
	bool accept;

	// 是否接入系统物流输出
	bool provide;

	// 存放产品
	OBJECT_HOLDER std::unordered_map<std::string, Product*> products;

	// 系统物流输入方
	std::unordered_map<std::string, Storage*> upstreams;

	// 系统物流输出方
	std::unordered_map<std::string, Storage*> downstreams;

	// 仓库容量
	float volume;
};

// 空仓库
class EmptyStorage : public StorageMod {
public:
	EmptyStorage();
	virtual ~EmptyStorage();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void SetProperty();

private:
	static int count;

	int id;
	std::string name;
};

