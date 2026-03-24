#pragma once

#include "zone_mod.h"

#include "block.h"


// 子类注册函数
typedef void (*RegisterModZonesFunc)(ZoneFactory* factory);

// 园区实体
class Block;
class Building;
class BuildingFactory;
class Zone {
public:
	Zone() = delete;
	Zone(ZoneFactory* factory, std::string zone);
	~Zone();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 获取几何
	Quad* GetQuad();

	// 设计园区
	void LayoutZone(Lot* block, BuildingFactory* factory);

	// 获取所在地块
	Block* GetParent() const;

	// 设置所在地块
	void SetParent(Block* block);

	// 获取是否由政府拥有
	bool GetStated() const;

	// 设置是否由政府拥有
	void SetStated(bool state);

	// 获取私人房东ID
	//int GetOwner() const;

	// 设置私人房东ID
	//void SetOwner(int owner);

	// 获取一栋建筑
	Building* GetBuilding(std::string name);

	// 获取园区内所有建筑
	const std::unordered_map<std::string, Building*>& GetBuildings();

	// 自动分布建筑
	void ArrangeBuildings();

	// 清理空建筑
	void ClearZero();

	// 获取园区中心世界位置
	void GetPosition(float& x, float& y) const;

	// 获取地址
	std::string GetAddress();

private:
	ZoneMod* mod;
	ZoneFactory* factory;

	std::string type;
	std::string name;

	Block* parentBlock;
	bool stated;
	//Person* owner;
	std::string fullAddress;
	OBJECT_HOLDER std::unordered_map<std::string, Building*> buildings;
};

// 空园区
class EmptyZone : public ZoneMod {
public:
	EmptyZone();
	virtual ~EmptyZone();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static std::function<int(Lot*)> ZoneAssigner;

	virtual void LayoutZone(Lot* block);

private:
	static int count;

	int id;
	std::string name;
};

