#pragma once

#include "industry/manufacture_mod.h"


// 经验工坊
class ExperienceManufacture : public ManufactureMod {
public:
	ExperienceManufacture();
	virtual ~ExperienceManufacture();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void SetTargets();

private:
	static int count;

	int id;
	std::string name;
};
