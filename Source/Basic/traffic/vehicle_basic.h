#pragma once

#include "traffic/vehicle_mod.h"


// 城市载具
class CityVehicle : public VehicleMod {
public:
	CityVehicle();
	virtual ~CityVehicle();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static std::string GetLabel();

	virtual void SetupVehicle() override;

private:
	static int count;

	int id;
	std::string name;
};

// 飞机载具
class PlaneVehicle : public VehicleMod {
public:
	PlaneVehicle();
	virtual ~PlaneVehicle();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static std::string GetLabel();

	virtual void SetupVehicle()override;

private:
	static int count;

	int id;
	std::string name;
};

