#pragma once

#include "industry/manufacture_mod.h"


// 经验工坊
class ExperienceManufacture : public ManufactureMod {
public:
	/*
	* 构造经验工坊
	*/
	ExperienceManufacture();

	/*
	* 析构经验工坊
	*/
	virtual ~ExperienceManufacture();

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
