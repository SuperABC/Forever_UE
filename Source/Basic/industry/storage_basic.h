#pragma once

#include "storage_mod.h"


// 商店仓库
class ShopStorage : public StorageMod {
public:
	ShopStorage();
	virtual ~ShopStorage();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void SetProperty();

private:
	static int count;

	int id;
	std::string name;
};
