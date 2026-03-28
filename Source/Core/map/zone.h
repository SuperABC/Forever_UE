#pragma once

#include "story/script.h"

#include "zone_mod.h"

#include "block.h"


// 子类注册函数
typedef void (*RegisterModZonesFunc)(ZoneFactory* factory);

// 园区实体
class Block;
class Building;
class BuildingFactory;
class Zone : public Quad {
public:
    // 禁止默认构造
	Zone() = delete;

    // 通过类型从工厂构造
	Zone(ZoneFactory* factory, const std::string& zone);
	
    // 析构园区
	~Zone();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 获取所在地块
	Block* GetParent() const;

	// 设置所在地块
	void SetParent(Block* block);

	// 获取地址
	std::string GetAddress();

	// 获取一栋建筑
	Building* GetBuilding(const std::string& name);

	// 获取园区内所有建筑
	const std::unordered_map<std::string, Building*>& GetBuildings();

	// 获取剧情
	Script* GetScript();

	// 获取园区中心世界位置
	void GetPosition(float& x, float& y) const;

	// 设计园区
	void LayoutZone(Lot* block, BuildingFactory* factory);

	// 自动分布建筑
	void ArrangeBuildings();

	// 清理空建筑
	void ClearZero();

	// 读取剧情
	void ReadScript();

private:
    // 模组对象
	OBJECT_HOLDER ZoneMod* mod;

    // 工厂
	ZoneFactory* factory;

    // 地形类型
	std::string type;

    // 地形名称
	std::string name;

    // 所在街区
	Block* parentBlock;
	
	// 完整地址
	std::string address;

	// 内部建筑
	OBJECT_HOLDER std::unordered_map<std::string, Building*> buildings;

	// 关联剧情
	Script* script;
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

