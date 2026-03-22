#pragma once

#include "building_mod.h"

#include "block.h"


// 子类注册函数
typedef void (*RegisterModBuildingsFunc)(BuildingFactory* factory);

// 空建筑
class EmptyBuilding : public BuildingMod {
public:
	EmptyBuilding();
	virtual ~EmptyBuilding();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static std::vector<float> GetPowers();

	static std::function<int(Lot*)> BuildingAssigner;

	virtual float RandomAcreage();
	virtual void LayoutBuilding();
	virtual void PlaceConstruction();

private:
	static int count;

	int id;
	std::string name;
};

// 建筑实体
class Block;
class Zone;
class Building {
public:
	Building() = delete;
	Building(BuildingFactory* factory, std::string building);
	~Building();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 获取几何
	Quad* GetQuad();

	// 采样面积
	float RandomAcreage();

	// 面积下限
	float GetAcreageMin();

	// 面积上限
	float GetAcreageMax();

	// 获取所在地块
	Block* GetParentBlock() const;

	// 设置所在地块
	void SetParent(Block* block);

	// 获取所在园区
	Zone* GetParentZone() const;

	// 设置所在园区
	void SetParent(Zone* zone);

	// 获取是否由政府拥有
	bool GetStated() const;

	// 设置是否由政府拥有
	void SetStated(bool stated);

	// 获取总地面层数
	int GetLayers() const;

	// 设置总地面层数
	void SetLayers(int layers);

	// 获取总地下层数
	int GetBasements() const;

	// 设置总地下层数
	void SetBasements(int basements);

	// 获取层高
	float GetHeight() const;

	// 设置层高
	void SetHeight(float height);

	// 获取建筑楼体范围
	const Quad GetConstruction() const;

	// 获取私人房东ID
	//int GetOwner() const;

	// 设置私人房东ID
	//void SetOwner(int owner);

private:
	BuildingMod* mod;
	BuildingFactory* factory;

	std::string type;
	std::string name;

	Zone* parentZone;
	Block* parentBlock;
	bool stated;
	int layers;
	int basements;
	float height;
	Quad construction;
	//Person* owner;
	//VARIABLE_HOLDER std::vector<Floor*> floors;
	//VARIABLE_HOLDER std::vector<Component*> components;
	//VARIABLE_HOLDER std::vector<Room*> rooms;
};

