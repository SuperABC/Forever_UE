#pragma once

#include "building_mod.h"


// 住宅建筑
class ResidentialBuilding : public BuildingMod {
public:
	ResidentialBuilding();
	virtual ~ResidentialBuilding();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static std::vector<float> GetPowers();

	static std::function<int(const Lot*)> BuildingAssigner;

	virtual float RandomAcreage() override;
	virtual void LayoutBuilding(const Quad* quad) override;
	virtual void PlaceConstruction() override;

private:
	static int count;

	int id;
	std::string name;
};

// 商店建筑
class ShopBuilding : public BuildingMod {
public:
	ShopBuilding();
	virtual ~ShopBuilding();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static std::vector<float> GetPowers();

	static std::function<int(const Lot*)> BuildingAssigner;

	virtual float RandomAcreage() override;
	virtual void LayoutBuilding(const Quad* quad) override;
	virtual void PlaceConstruction() override;

private:
	static int count;

	int id;
	std::string name;
};

// 工厂建筑
class FactoryBuilding : public BuildingMod {
public:
	FactoryBuilding();
	virtual ~FactoryBuilding();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static std::vector<float> GetPowers();

	static std::function<int(const Lot*)> BuildingAssigner;

	virtual float RandomAcreage() override;
	virtual void LayoutBuilding(const Quad* quad) override;
	virtual void PlaceConstruction() override;

private:
	static int count;

	int id;
	std::string name;
};
