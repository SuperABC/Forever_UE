#pragma once

#include "terrain_mod.h"


// 海洋地形
class OceanTerrain : public TerrainMod {
public:
	OceanTerrain();
	virtual ~OceanTerrain();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual float GetPriority() const override;

	virtual void DistributeTerrain(int width, int height,
		std::function<bool(int, int, std::string, float)> setElement,
		std::function<std::string(int, int)> getTerrain, std::function<float(int, int)> getHeight) const override;

private:
	static int count;

	int id;
	std::string name;
};

// 山区地形
class MountainTerrain : public TerrainMod {
public:
	MountainTerrain();
	virtual ~MountainTerrain();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual float GetPriority() const override;

	virtual void DistributeTerrain(int width, int height,
		std::function<bool(int, int, std::string, float)> setElement,
		std::function<std::string(int, int)> getTerrain, std::function<float(int, int)> getHeight) const override;

private:
	static int count;

	int id;
	std::string name;
};

