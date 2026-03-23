#pragma once

#include "component_mod.h"


// 住宅组合
class ResidentialComponent : public ComponentMod {
public:
	ResidentialComponent();
	virtual ~ResidentialComponent();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

private:
	static int count;

	int id;
	std::string name;
};
