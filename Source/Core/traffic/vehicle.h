#pragma once

#include "vehicle_base.h"

#include <string>
#include <vector>


// 子类注册函数
typedef void (*RegisterModVehiclesFunc)(VehicleFactory* factory);

// 飞机载具
class PlaneVehicle : public Vehicle {
public:
	static std::string GetId();
	virtual std::string GetType() const override;
	virtual std::string GetName() const override;
};