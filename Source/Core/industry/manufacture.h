#pragma once

#include "industry/manufacture_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModManufacturesFunc)(ManufactureFactory* factory);

// 子类注册收尾函数
typedef void (*FinishModManufacturesFunc)(ManufactureFactory* factory);

// 工坊实体
class Manufacture {
public:
	/*
	* 禁止默认构造
	*/
	Manufacture() = delete;

	/*
	* 通过类型从工厂构造
	* @factory: 工坊工厂
	* @manufacture: 工坊静态类型标识
	*/
	Manufacture(ManufactureFactory* factory, const std::string& manufacture);

	/*
	* 析构工坊
	*/
	~Manufacture();

	/*
	* 获取类型
	*/
	std::string GetType() const;

	/*
	* 获取名称
	*/
	std::string GetName() const;

	/*
	* 设置参数
	* @room: 所在房间
	*/
	void SetProperty(Room* room);

	/*
	* 获取所在房间
	*/
	Room* GetRoom() const;

	/*
	* 获取输入暂存仓库
	*/
	Storage* GetInput() const;

	/*
	* 获取输出暂存仓库
	*/
	Storage* GetOutput() const;

	/*
	* 获取上游仓库
	*/
	std::unordered_map<std::string, Storage*> GetUpstreams() const;

	/*
	* 连接上游仓库
	* @type: 产品类型标识
	* @storage: 上游仓库
	*/
	void ConnectUpstream(std::string type, Storage* storage);

	/*
	* 获取下游仓库
	*/
	std::unordered_map<std::string, Storage*> GetDownstreams() const;

	/*
	* 连接下游仓库
	* @type: 产品类型标识
	* @storage: 下游仓库
	*/
	void ConnectDownstream(std::string type, Storage* storage);

	/*
	* 获取日原料消耗量
	*/
	std::unordered_map<std::string, float> GetIngredients() const;

	/*
	* 获取日产品产量
	*/
	std::unordered_map<std::string, float> GetTargets() const;

	/*
	* 获取日副产物产量
	*/
	std::unordered_map<std::string, float> GetByproducts() const;

	/*
	* 前一日生产结算
	*/
	void WorkAccount();

	/*
	* 每日生产前物流
	*/
	void InitDelivery();

	/*
	* 每日启动生产
	*/
	void StartProduce();

private:
	// 模组对象
	OBJECT_HOLDER ManufactureMod* mod;

	// 工厂
	ManufactureFactory* factory;

	// 所在房间
	Room* room;

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
	/*
	* 构造空工坊
	*/
	EmptyManufacture();

	/*
	* 析构空工坊
	*/
	virtual ~EmptyManufacture();

	/*
	* Override
	* 工坊静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 工坊动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 工坊实例名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 设置生产目标
	*/
	virtual void SetTargets() override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;

};
