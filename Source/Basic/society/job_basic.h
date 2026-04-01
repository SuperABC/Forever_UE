#pragma once

#include "job_mod.h"


class ShopSalerJob : public JobMod {
public:
	ShopSalerJob();
	virtual ~ShopSalerJob();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void InitJob() override;

private:
	static int count;

	int id;
	std::string name;
};
