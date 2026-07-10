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

	virtual void LayoutRoute(
		const std::unordered_map<std::string, std::vector<std::pair<std::vector<Node*>, std::vector<std::pair<Node*, Node*>>>>>& interfaces,
		int sizeX, int sizeY, int nodeStaticCount);

private:
	static int count;

	int id;
	std::string name;
};
