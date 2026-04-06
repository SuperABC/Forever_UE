#pragma once

#include "traffic/route_mod.h"


// 飞机线路
class AirRoute : public RouteMod {
public:
	AirRoute();
	virtual ~AirRoute();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

private:
	static int count;

	int id;
	std::string name;
};
