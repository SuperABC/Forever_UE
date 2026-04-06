#pragma once

#include "traffic/vehicle_mod.h"


// 飞机载具
class PlaneVehicle : public VehicleMod {
public:
	PlaneVehicle();
	virtual ~PlaneVehicle();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

private:
	static int count;

	int id;
	std::string name;
};

