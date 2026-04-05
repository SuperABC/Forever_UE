#pragma once

#include "manufacture_mod.h"

#include "storage.h"


// 子类注册函数
typedef void (*RegisterModManufacturesFunc)(ManufactureFactory* factory);

// 工坊实体
class Storage;
class Manufacture {
public:
	// 禁止默认构造
	Manufacture() = delete;

	// 通过类型从工厂构造
	Manufacture(ManufactureFactory* factory, const std::string& manufacture);

	// 析构工坊
	~Manufacture();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 设置参数
	void SetProperty();

	// 获取输入暂存仓库
	OBJECT_HOLDER Storage* GetInput() const;

	// 获取输出暂存仓库
	OBJECT_HOLDER Storage* GetOutput() const;

	// 获取上游仓库
	std::unordered_map<std::string, Storage*> GetUpstreams() const;

	// 设置上游仓库
	void ConnectUpstream(std::string type, Storage* storage);

	// 获取下游仓库
	std::unordered_map<std::string, Storage*> GetDownstreams() const;

	// 设置下游仓库
	void ConnectDownstream(std::string type, Storage* storage);

	// 获取日原料消耗量
	std::unordered_map<std::string, float> GetIngredients() const;

	// 获取日产品产量
	std::unordered_map<std::string, float> GetTargets() const;

	// 获取日副产物产量
	std::unordered_map<std::string, float> GetByproducts() const;

	// 每日启动生产
	void StartProduce();

	// 每日完成生产
	void FinishProduce();

private:
	// 模组对象
	OBJECT_HOLDER ManufactureMod* mod;

	// 工厂
	ManufactureFactory* factory;

	// 工坊类型
	std::string type;

	// 工坊名称
	std::string name;

	// 内置输入暂存仓库
	OBJECT_HOLDER Storage* inputCache;

	// 内置输出暂存仓库
	OBJECT_HOLDER Storage* outputCache;

	// 日原料消耗量
	std::unordered_map<std::string, float> ingredients;

	// 日产品产量
	std::unordered_map<std::string, float> targets;

	// 日副产物产量
	std::unordered_map<std::string, float> byproducts;

	// 当日生产效率
	float currentWorkload;
};

// 空工坊
class EmptyManufacture : public ManufactureMod {
public:
	EmptyManufacture();
	virtual ~EmptyManufacture();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void SetTargets();

private:
	static int count;

	int id;
	std::string name;
};

