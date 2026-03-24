#pragma once

#include "terrain_mod.h"


// 子类注册函数
typedef void (*RegisterModTerrainsFunc)(TerrainFactory* factory);

// 地形实体
class Terrain {
public:
	Terrain() = delete;
	Terrain(TerrainFactory* factory, std::string terrain);
	~Terrain();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 构建优先级
	float GetPriority() const;

	// 构建地形
	void DistributeTerrain(int width, int height,
		std::function<std::string(int, int)> getTerrain, 
		std::function<bool(int, int, std::string)> setTerrain,
        std::function<float(int, int)> getHeight, 
		std::function<bool(int, int, float)> setHeight) const;

private:
	TerrainMod* mod;
	TerrainFactory* factory;

	std::string type;
	std::string name;
};

// 空地形
class EmptyTerrain : public TerrainMod {
public:
	EmptyTerrain();
	virtual ~EmptyTerrain();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual float GetPriority() const override;

	virtual void DistributeTerrain(int width, int height,
		std::function<std::string(int, int)> getTerrain, 
		std::function<bool(int, int, std::string)> setTerrain,
        std::function<float(int, int)> getHeight, 
		std::function<bool(int, int, float)> setHeight) const override;

private:
	static int count;

	int id;
	std::string name;
};

