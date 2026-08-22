#pragma once

#include "map/component_mod.h"


// 住宅组合
class ResidentialComponent : public ComponentMod {
public:
	/*
	* 构造住宅组合
	*/
	ResidentialComponent();

	/*
	* 析构住宅组合
	*/
	virtual ~ResidentialComponent();

	/*
	* Override
	* 组合静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 组合动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 组合实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 初始化组合
	*/
	virtual void InitComponent() override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

// 商店组合
class ShopComponent : public ComponentMod {
public:
	/*
	* 构造商店组合
	*/
	ShopComponent();

	/*
	* 析构商店组合
	*/
	virtual ~ShopComponent();

	/*
	* Override
	* 组合静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 组合动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 组合实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 初始化组合
	*/
	virtual void InitComponent() override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

// 工厂组合
class FactoryComponent : public ComponentMod {
public:
	/*
	* 构造工厂组合
	*/
	FactoryComponent();

	/*
	* 析构工厂组合
	*/
	virtual ~FactoryComponent();

	/*
	* Override
	* 组合静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 组合动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 组合实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 初始化组合
	*/
	virtual void InitComponent() override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

// 医院组合
class HospitalComponent : public ComponentMod {
public:
	/*
	* 构造医院组合
	*/
	HospitalComponent();

	/*
	* 析构医院组合
	*/
	virtual ~HospitalComponent();

	/*
	* Override
	* 组合静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 组合动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 组合实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 初始化组合
	*/
	virtual void InitComponent() override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};
