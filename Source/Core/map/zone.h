#pragma once

#include "map/zone_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModZonesFunc)(ZoneFactory* factory);

// 园区实体
class Zone : public Quad {
public:
	/*
	* 禁止默认构造
	*/
	Zone() = delete;

	/*
	* 通过园区静态类型标识从工厂构造园区
	* @factory: 园区工厂
	* @zone: 园区静态类型标识
	*/
	Zone(ZoneFactory* factory, const std::string& zone);

	/*
	* 析构园区
	*/
	~Zone();

	/*
	* 获取园区类型
	*/
	std::string GetType() const;

	/*
	* 获取园区名称
	*/
	std::string GetName() const;

	/*
	* 获取所在地块
	*/
	Block* GetParent() const;

	/*
	* 设置所在地块
	* @block: 所在地块
	*/
	void SetParent(Block* block);

	/*
	* 获取完整地址
	*/
	std::string GetAddress();

	/*
	* 获取寻址锚点
	*/
	const std::vector<Node*> GetPivots();

	/*
	* 获取园区内部划分（DivideSpace）产生的导航节点
	*/
	const std::vector<Node*>& GetNodes() const;

	/*
	* 获取园区自身的四角四边（非持有引用）
	*/
	const QuadBoundary& GetBoundary() const;

	/*
	* 追加园区内部划分产生的导航节点（园区持有，负责释放）
	* @newNodes: 新增节点
	*/
	void AddNodes(const std::vector<Node*>& newNodes);

	/*
	* 设置园区自身的四角四边
	* @boundary: 四角四边（非持有引用）
	*/
	void SetBoundary(const QuadBoundary& boundary);

	/*
	* 按名称获取园区内一栋建筑
	* @name: 建筑名称
	*/
	Building* GetBuilding(const std::string& name);

	/*
	* 获取园区内所有建筑
	*/
	const std::unordered_map<std::string, Building*>& GetBuildings();

	/*
	* 获取是否由政府拥有
	*/
	bool GetStated() const;

	/*
	* 设置是否由政府拥有
	* @state: 是否由政府拥有
	*/
	void SetStated(bool state);

	/*
	* 获取私人房东
	*/
	Person* GetOwner() const;

	/*
	* 设置私人房东
	* @owner: 私人房东
	*/
	void SetOwner(Person* owner);

	/*
	* 获取关联剧情
	*/
	Script* GetScript() const;

	/*
	* 获取园区中心世界位置
	* @x, y: 输出的世界坐标
	*/
	void GetPosition(float& x, float& y) const;

	/*
	* 设计园区
	* @block: 所在地块
	*/
	void LayoutZone(const Lot* block);

	/*
	* 自动分布建筑，并将分布过程中产生的导航节点与连接接入地图导航图
	* @map: 所在地图
	*/
	void ArrangeBuildings(Map* map);

	/*
	* 清理面积为零的建筑
	*/
	void ClearZero();

	/*
	* 放置寻址锚点
	* @zone: 园区所占据的矩形区域
	*/
	void PlacePivots(Quad* zone);

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

	// 寻址锚点
	OBJECT_HOLDER std::vector<Node*> pivots;

	// 内部划分产生的导航节点
	OBJECT_HOLDER std::vector<Node*> nodes;

	// 自身的四角四边（非持有引用）
	QuadBoundary boundary;

	// 内部建筑
	OBJECT_HOLDER std::unordered_map<std::string, Building*> buildings;

	// 是否由政府拥有
	bool stated;

	// 私人房东
	Person* owner;

	// 关联剧情
	OBJECT_HOLDER Script* script;
};

// 空园区
class EmptyZone : public ZoneMod {
public:
	/*
	* 构造空园区
	*/
	EmptyZone();

	/*
	* 析构空园区
	*/
	virtual ~EmptyZone();

	/*
	* Override
	* 园区静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 园区动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 园区实例唯一名称
	*/
	virtual const char* GetName() override;

	// 按地块计算应生成的园区数量
	static std::function<int(const Lot*)> ZoneAssigner;

	/*
	* Override
	* 设计园区
	* @block: 所在地块
	*/
	virtual void LayoutZone(const Lot* block) override;

	/*
	* Override
	* 放置寻址锚点
	* @zone: 园区所占据的矩形区域
	*/
	virtual void PlacePivots(Quad* zone) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

