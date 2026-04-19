#pragma once

#include "map/component_mod.h"


// 住宅组合
class ResidentialComponent : public ComponentMod {
public:
	ResidentialComponent();
	virtual ~ResidentialComponent();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void InitComponent();

private:
	static int count;

	int id;
	std::string name;
};

// 商店组合
class ShopComponent : public ComponentMod {
public:
	ShopComponent();
	virtual ~ShopComponent();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void InitComponent();

private:
	static int count;

	int id;
	std::string name;
};

// 工厂组合
class FactoryComponent : public ComponentMod {
public:
	FactoryComponent();
	virtual ~FactoryComponent();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void InitComponent();

private:
	static int count;

	int id;
	std::string name;
};
