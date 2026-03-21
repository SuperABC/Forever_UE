#pragma once

#include "roadnet_mod.h"


// 井字路网
class JingRoadnet : public RoadnetMod {
public:
	JingRoadnet();
	virtual ~JingRoadnet();

	static std::string GetId();
	virtual std::string GetType() const override;
	virtual std::string GetName() const override;

	virtual void DistributeRoadnet(int width, int height,
		std::function<std::string(int, int)> get) override;
};
