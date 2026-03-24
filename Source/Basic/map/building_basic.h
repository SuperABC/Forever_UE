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

	static std::function<int(Lot*)> BuildingAssigner;

	virtual float RandomAcreage();
	virtual void LayoutBuilding(const Quad* quad);
	virtual void PlaceConstruction();

private:
	static int count;

	int id;
	std::string name;
};
