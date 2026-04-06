#pragma once

#include "traffic/station_mod.h"


// 机场站点
class AirportStation : public StationMod {
public:
	AirportStation();
	virtual ~AirportStation();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

private:
	static int count;

	int id;
	std::string name;
};
