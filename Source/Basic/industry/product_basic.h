#pragma once

#include "industry/product_mod.h"


// 经验产品
class ExperienceProduct : public ProductMod {
public:
	/*
	* 构造经验产品
	*/
	ExperienceProduct();

	/*
	* 析构经验产品
	*/
	virtual ~ExperienceProduct();

	/*
	* Override
	* 产品静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 产品动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 产品实例名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 设置产品参数
	*/
	virtual void SetProperty() override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;

};
