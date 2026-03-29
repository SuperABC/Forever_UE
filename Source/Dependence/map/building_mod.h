#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include "geometry.h"

#include <string>
#include <tuple>
#include <functional>


// 建筑方向
enum FACE_DIRECTION : int {
	FACE_WEST,
	FACE_EAST,
	FACE_NORTH,
	FACE_SOUTH
};
static char faceAbbr[4] = { 'w', 'e', 'n', 's' };

// 规定pair哈希以作为map键
struct PairHash {
	std::size_t operator()(const std::pair<std::string, int>& p) const {
		return std::hash<std::string>()(p.first) ^ (std::hash<int>()(p.second) << 1);
	}
};

class BuildingMod {
public:
	// 无构造
	BuildingMod();

	// 无析构
	virtual ~BuildingMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 建筑生成权重
	COMSTOM_INIT static std::vector<float> GetPowers();

	// 指定地块内建筑数量
	COMSTOM_INIT static std::function<int(const Lot*)> BuildingAssigner;

	// 采样建筑面积
	COMSTOM_INIT virtual float RandomAcreage() = 0;

	// 布局建筑内部
	COMSTOM_INIT virtual void LayoutBuilding(const Quad* quad) = 0;

	// 放置楼栋
	COMSTOM_INIT virtual void PlaceConstruction() = 0;

	// 根据楼层分配空间
	void AllocateFloors();

	// 根据模板生成一层楼层
	void AssignFloor(int level, int face, std::string layout);

	// 按照单一模板生成所有楼层
	void AssignFloors(int face, std::string layout);

	// 按照层数个模板生成所有楼层
	void AssignFloors(int face, std::vector<std::string> layouts);

	// 为模板中第slot个独立房间生成房间，按id并入component
	void AssignRoom(int level, int slot, std::string room, std::string component, int id);

	// 为模板中第slot个联排房间生成房间，按id并入component
	void ArrangeRow(int level, int slot, std::string room, float acreage, std::string component, int id);

	// 最大面积限制
	float maxAcreage;

	// 最小面积限制
	float minAcreage;

	// 地面层数
	int layers;

	// 地下限制
	int basements;

	// 层高
	float height;

	// 楼栋位置
	Quad construction;

	// 所有楼层布局及方向（包含地下及地上）
	std::vector<std::pair<std::string, int>> templates;

	//							component		id		每个room		floor	slot	type
	// 单间模板房间
	std::unordered_map<std::pair<std::string, int>, std::vector<std::tuple<int, int, std::string>>, PairHash> singles;

	//							component		id		每个floor	每个room			slot	type	面积
	// 联排模板房间
	std::unordered_map<std::pair<std::string, int>, std::vector<std::vector<std::tuple<int, std::string, float>>>, PairHash> rows;
	
	// 关联剧情与脚本
	std::pair<std::string, std::vector<std::string>> script;
};

class BuildingFactory {
public:
	// 注册建筑
	void RegisterBuilding(const std::string& id,
		const std::vector<float>& power, std::function<int(const Lot*)> assigner,
		std::function<BuildingMod* ()> creator, std::function<void(BuildingMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建建筑
	BuildingMod* CreateBuilding(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 获取所有建筑全地块权重
	std::unordered_map<std::string, std::vector<float>> GetPowers() const;

	// 在地块内生成一类建筑
	std::vector<std::string> CreateBuildings(const std::string& type, const Lot* lot) const;

	// 析构建筑
	void DestroyBuilding(BuildingMod* zone) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<BuildingMod* ()>, std::function<void(BuildingMod*)>>
	> registries;
	
	// 启用配置
	std::unordered_map<std::string, bool> configs;
	
	// 权重
	std::unordered_map<std::string, std::vector<float>> powers;
	
	// 建筑生成器
	std::unordered_map<std::string, std::function<int(const Lot*)>> assigners;
};

