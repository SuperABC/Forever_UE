#pragma once

#include "society/job_mod.h"


class ShopSalerJob : public JobMod {
public:
	/*
	* 构造售货员职业
	*/
	ShopSalerJob();

	/*
	* 析构售货员职业
	*/
	virtual ~ShopSalerJob();

	/*
	* Override
	* 职业静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 职业动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 职业实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 初始化职业
	*/
	virtual void InitJob() override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};
