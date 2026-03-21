#pragma once

#include "terrain_mod.h"


// 海洋地形
class OceanTerrain : public TerrainMod {
public:
    OceanTerrain();
    virtual ~OceanTerrain();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual float GetPriority() const override;

    virtual void DistributeTerrain(int width, int height,
        std::function<bool(int, int, std::string, float)> setElement,
        std::function<std::string(int, int)> getTerrain, std::function<float(int, int)> getHeight) const override;
};

// 山区地形
class MountainTerrain : public TerrainMod {
public:
    MountainTerrain();
    virtual ~MountainTerrain();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual float GetPriority() const override;

    virtual void DistributeTerrain(int width, int height,
        std::function<bool(int, int, std::string, float)> setElement,
        std::function<std::string(int, int)> getTerrain, std::function<float(int, int)> getHeight) const override;
};

