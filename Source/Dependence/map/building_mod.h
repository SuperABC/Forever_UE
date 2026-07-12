#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include "geometry.h"

#include <string>
#include <tuple>
#include <functional>
#include <unordered_map>
#include <unordered_set>


// 点位相对参数的固定长度（xRatio, xOffset, yRatio, yOffset）
#define POINT_PARAM_COUNT 4

// 规定pair哈希以作为map键
struct PairHash {
	/*
	* 计算pair哈希值
	* @p: 待计算哈希的pair
	*/
	std::size_t operator()(const std::pair<std::string, int>& p) const {
		return std::hash<std::string>()(p.first) ^ (std::hash<int>()(p.second) << 1);
	}
};

// 建筑Mod基类
class BuildingMod {
public:
	/*
	* 构造建筑
	*/
	BuildingMod();

	/*
	* 析构建筑
	*/
	virtual ~BuildingMod();

	/*
	* Override
	* 建筑静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 建筑动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 建筑实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 建筑在各功能分区的生成权重
	*/
	COSTOM_INIT static std::vector<float> GetPowers();

	/*
	* Override
	* 按地块计算应生成的建筑数量
	* @lot: 待生成建筑的地块
	* @index: 当前建筑在地块内的序号
	* @total: 地块内建筑总数
	*/
	COSTOM_INIT static std::function<int(const Lot*, int, int)> BuildingAssigner;

	/*
	* Override
	* 采样建筑面积
	*/
	COSTOM_INIT virtual float RandomAcreage() = 0;

	/*
	* Override
	* 布局建筑
	* @quad: 建筑所占据的矩形区域
	*/
	COSTOM_INIT virtual void LayoutBuilding(const Quad* quad) = 0;

	/*
	* Override
	* 放置楼栋
	*/
	COSTOM_INIT virtual void PlaceConstruction() = 0;

	/*
	* Override
	* 放置寻址锚点
	* @building: 建筑所占据的矩形区域
	*/
	COSTOM_INIT virtual void PlacePivots(Quad* building) = 0;

	/*
	* Tool
	* 根据地上地下层数分配楼层模板空间
	*/
	void AllocateFloors();

	/*
	* Tool
	* 添加电梯
	* @name: 电梯名称
	* @temp: 电梯温度
	* @idx: 电梯编号
	* @from, to: 可达楼层范围
	* @script: 关联脚本
	* @paths: 脚本里程碑路径
	*/
	void AddElevator(std::string name, int temp, int idx, int from, int to, std::string script, std::vector<std::string> paths);

	/*
	* Tool
	* 根据模板生成一层楼层
	* @level: 楼层编号
	* @face: 模板朝向
	* @layout: 楼层模板名称
	*/
	void AssignFloor(int level, int face, std::string layout);

	/*
	* Tool
	* 按照单一模板生成所有楼层
	* @face: 模板朝向
	* @layout: 楼层模板名称
	*/
	void AssignFloors(int face, std::string layout);

	/*
	* Tool
	* 按照层数个模板生成所有楼层
	* @face: 模板朝向
	* @layouts: 每层对应的楼层模板名称
	*/
	void AssignFloors(int face, std::vector<std::string> layouts);

	/*
	* Tool
	* 为模板中第slot个独立房间生成房间，按id并入component
	* @level: 楼层编号
	* @slot: 独立房间在模板中的序号
	* @room: 房间类型名称
	* @component: 所属组合名称
	* @id: 组合编号
	*/
	void AssignRoom(int level, int slot, std::string room, std::string component, int id);

	/*
	* Tool
	* 为模板中第slot个联排房间生成房间，按id并入component
	* @level: 楼层编号
	* @slot: 联排房间在模板中的序号
	* @room: 房间类型名称
	* @acreage: 单间面积
	* @component: 所属组合名称
	* @id: 组合编号
	*/
	void ArrangeRow(int level, int slot, std::string room, float acreage, std::string component, int id);

	/*
	* Tool
	* 添加装饰
	* @path: 装饰资源路径
	* @px, py, pz: 装饰位置
	* @sx, sy, sz: 装饰缩放
	* @rx, ry, rz: 装饰旋转
	*/
	void AddDecoration(std::string path,
		float px, float py, float pz, float sx, float sy, float sz, float rx, float ry, float rz);

	/*
	* Tool
	* 将正北朝向下定义的模板锚点按建筑当前朝向旋转后加入锚点列表
	* @point: 锚点相对参数（4个浮点数：x比例、x偏移、y比例、y偏移），按正北朝向定义
	* @face: 建筑当前朝向
	*/
	void AddPivot(std::vector<float> point, int face);

	// 最大面积限制
	float maxAcreage;

	// 最小面积限制
	float minAcreage;

	// 地面层数
	int layers;

	// 地下层数
	int basements;

	// 层高
	float height;

	// 楼栋位置
	Quad construction;

	// 锚点
	std::vector<std::vector<float>> pivots;

	// 外立面材质
	std::string wallTexture;

	// 天花板材质
	std::string ceilingTexture;

	// 地板材质
	std::string groundTexture;

	// 所有楼层布局及方向（包含地下及地上）
	std::vector<std::pair<std::string, int>> templates;

	//										name		template	idx	from		to		script paths
	// 电梯设置
	std::vector<std::tuple<std::string, int, int, int, int, std::string, std::vector<std::string>>> elevators;

	//							component		id		每个room		floor	slot	type
	// 单间模板房间
	std::unordered_map<std::pair<std::string, int>, std::vector<std::tuple<int, int, std::string>>, PairHash> singles;

	//							component		id		每个floor	每个room			slot	type	面积
	// 联排模板房间
	std::unordered_map<std::pair<std::string, int>, std::vector<std::vector<std::tuple<int, std::string, float>>>, PairHash> rows;

	// 装饰物
	std::vector<std::pair<std::string, std::vector<float>>> decorations;

	// 关联剧情与脚本
	std::pair<std::string, std::vector<std::string>> script;
};

class BuildingFactory {
public:
	/*
	* 注册建筑
	* @id: 建筑类型
	* @power: 建筑在各功能分区的生成权重
	* @assigner: 按地块计算应生成的建筑数量的方法
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterBuilding(const std::string& id,
		const std::vector<float>& power, std::function<int(const Lot*, int, int)> assigner,
		std::function<BuildingMod* ()> creator, std::function<void(BuildingMod*)> deleter);

	/*
	* 清空所有注册
	*/
	void RemoveAll();

	/*
	* 创建建筑
	* @id: 建筑类型
	*/
	BuildingMod* CreateBuilding(const std::string& id) const;

	/*
	* 检查建筑是否已注册
	* @id: 建筑类型
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置建筑启用配置
	* @name: 建筑类型
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 获取所有已启用建筑在各功能分区的生成权重
	*/
	std::unordered_map<std::string, std::vector<float>> GetPowers() const;

	/*
	* 在地块内按数量生成器生成一类建筑的多个实例
	* @type: 建筑静态类型标识
	* @lot: 待生成建筑的地块
	* @idx: 当前建筑在地块内的序号
	* @total: 地块内建筑总数
	*/
	std::vector<std::string> CreateBuildings(const std::string& type, const Lot* lot, int idx, int total) const;

	/*
	* 析构建筑
	* @buildingMod: 待析构的建筑
	*/
	void DestroyBuilding(BuildingMod* buildingMod) const;

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
	std::unordered_map<std::string, std::function<int(const Lot*, int, int)>> assigners;
};

