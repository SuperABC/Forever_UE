#pragma once

#include "industry/storage_mod.h"


// 商店仓库
class ShopStorage : public StorageMod {
public:
	/*
	* 构造商店仓库
	*/
	ShopStorage();

	/*
	* 析构商店仓库
	*/
	virtual ~ShopStorage();

	/*
	* Override
	* 仓库静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 仓库动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 仓库实例名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 设置仓库参数
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
