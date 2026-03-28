#pragma once

#include "terrain_mod.h"


// 子类注册函数
typedef void (*RegisterModTerrainsFunc)(TerrainFactory* factory);

// 地形实体
class Terrain {
public:
    // 禁止默认构造
	Terrain() = delete;

    // 通过类型从工厂构造
	Terrain(TerrainFactory* factory, const std::string& terrain);

    // 析构地形
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
    // 模组对象
	OBJECT_HOLDER TerrainMod* mod;

    // 工厂
	TerrainFactory* factory;

    // 地形类型
	std::string type;

    // 地形名称
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

