#pragma once

#include "map/roadnet_mod.h"


// 井字路网
class JingRoadnet : public RoadnetMod {
public:
	JingRoadnet();
	virtual ~JingRoadnet();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void DistributeRoadnet(int width, int height,
		std::function<std::string(int, int)> get) override;

private:
	static int count;

	int id;
	std::string name;
};
