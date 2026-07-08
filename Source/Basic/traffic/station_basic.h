#pragma once

#include "map/building_mod.h"
#include "traffic/station_mod.h"


// 机场站点
class AirportStation : public StationMod {
public:
	AirportStation();
	virtual ~AirportStation();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static std::function<std::vector<int>& (const std::vector<Lot*>& blocks)> StationAssigner;

	virtual void LayoutStation(const Lot* block);

	virtual void PlaceInterface(const Quad* building);

private:
	static int count;

	int id;
	std::string name;
};

// 机场建筑
class AirportBuilding : public BuildingMod {
public:
	AirportBuilding();
	virtual ~AirportBuilding();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static std::vector<float> GetPowers();

	static std::function<int(const Lot*, int, int)> BuildingAssigner;

	virtual float RandomAcreage() override;
	virtual void LayoutBuilding(const Quad* quad) override;
	virtual void PlaceConstruction() override;
	virtual void PlacePivots(Quad* building) override;

private:
	static int count;

	int id;
	std::string name;
};
