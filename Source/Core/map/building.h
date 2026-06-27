#pragma once

#include "map/building_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModBuildingsFunc)(BuildingFactory* factory);

// 楼梯
class Stair : public Quad {
public:
	/*
	* 按矩形参数构造楼梯
	* @params: 矩形相对参数（8个浮点数）
	*/
	Stair(std::vector<float> params);

	/*
	* 析构楼梯
	*/
	~Stair();

	/*
	* 获取朝向
	*/
	FACE_DIRECTION GetDirection() const;

	/*
	* 设置朝向
	* @direction: 朝向
	*/
	void SetDirection(FACE_DIRECTION direction);

	/*
	* 获取某一侧是否有墙
	* @direction: 方向
	*/
	bool GetWall(int direction) const;

	/*
	* 在某一侧添加墙
	* @direction: 方向
	*/
	void AddWall(int direction);

	/*
	* 按楼层尺寸将相对参数实例化为实际矩形
	* @width, height: 楼层尺寸
	*/
	void InstanciateQuad(float width, float height);

private:
	// 朝向
	FACE_DIRECTION direction;

	// 四面是否有墙
	std::vector<bool> walls;

	// 矩形相对参数
	std::vector<float> params;
};

// 电梯
class Elevator : public Quad {
public:
	/*
	* 按矩形参数构造电梯
	* @params: 矩形相对参数（8个浮点数）
	*/
	Elevator(std::vector<float> params);

	/*
	* 析构电梯
	*/
	~Elevator();

	/*
	* 获取朝向
	*/
	FACE_DIRECTION GetDirection() const;

	/*
	* 设置朝向
	* @direction: 朝向
	*/
	void SetDirection(FACE_DIRECTION direction);

	/*
	* 获取某一侧是否有墙
	* @direction: 方向
	*/
	bool GetWall(int direction) const;

	/*
	* 在某一侧添加墙
	* @direction: 方向
	*/
	void AddWall(int direction);

	/*
	* 按楼层尺寸将相对参数实例化为实际矩形
	* @width, height: 楼层尺寸
	*/
	void InstanciateQuad(float width, float height);

private:
	// 朝向
	FACE_DIRECTION direction;

	// 四面是否有墙
	std::vector<bool> walls;

	// 矩形相对参数
	std::vector<float> params;
};

// 斜坡
class Ramp : public Quad {
public:
	/*
	* 按矩形参数构造斜坡
	* @params: 矩形相对参数（8个浮点数）
	*/
	Ramp(std::vector<float> params);

	/*
	* 析构斜坡
	*/
	~Ramp();

	/*
	* 获取朝向
	*/
	FACE_DIRECTION GetDirection() const;

	/*
	* 设置朝向
	* @direction: 朝向
	*/
	void SetDirection(FACE_DIRECTION direction);

	/*
	* 获取某一侧是否有墙
	* @direction: 方向
	*/
	bool GetWall(int direction) const;

	/*
	* 在某一侧添加墙
	* @direction: 方向
	*/
	void AddWall(int direction);

	/*
	* 按楼层尺寸将相对参数实例化为实际矩形
	* @width, height: 楼层尺寸
	*/
	void InstanciateQuad(float width, float height);

private:
	// 朝向
	FACE_DIRECTION direction;

	// 四面是否有墙
	std::vector<bool> walls;

	// 矩形相对参数
	std::vector<float> params;
};

// 天花板
class Ceiling : public Quad {
public:
	/*
	* 按矩形参数构造天花板
	* @params: 矩形相对参数（8个浮点数）
	*/
	Ceiling(std::vector<float> params);

	/*
	* 析构天花板
	*/
	~Ceiling();

	/*
	* 按楼层尺寸将相对参数实例化为实际矩形
	* @width, height: 楼层尺寸
	*/
	void InstanciateQuad(float width, float height);

private:
	// 矩形相对参数
	std::vector<float> params;
};

// 地板
class Ground : public Quad {
public:
	/*
	* 按矩形参数构造地板
	* @params: 矩形相对参数（8个浮点数）
	*/
	Ground(std::vector<float> params);

	/*
	* 析构地板
	*/
	~Ground();

	/*
	* 按楼层尺寸将相对参数实例化为实际矩形
	* @width, height: 楼层尺寸
	*/
	void InstanciateQuad(float width, float height);

private:
	// 矩形相对参数
	std::vector<float> params;
};

// 走廊
class Corridor : public Quad {
public:
	/*
	* 按矩形参数构造走廊
	* @params: 矩形相对参数（8个浮点数）
	*/
	Corridor(std::vector<float> params);

	/*
	* 析构走廊
	*/
	~Corridor();

	/*
	* 获取某一侧是否有墙
	* @direction: 方向
	*/
	bool GetWall(int direction) const;

	/*
	* 在某一侧添加墙
	* @direction: 方向
	*/
	void AddWall(int direction);

	/*
	* 获取所有门
	*/
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetDoors() const;

	/*
	* 在某一侧添加门
	* @direction: 方向
	* @positions: 门的相对位置参数列表
	*/
	void AddDoor(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);

	/*
	* 获取所有窗
	*/
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetWindows() const;

	/*
	* 在某一侧添加窗
	* @direction: 方向
	* @positions: 窗的相对位置参数列表
	*/
	void AddWindow(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);

	/*
	* 按楼层尺寸将相对参数实例化为实际矩形
	* @width, height: 楼层尺寸
	*/
	void InstanciateQuad(float width, float height);

private:
	// 四面是否有墙
	std::vector<bool> walls;

	// 各方向的门
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> doors;

	// 各方向的窗
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> windows;

	// 矩形相对参数
	std::vector<float> params;
};

// 地板洞口
class Hatch : public Quad {
public:
	/*
	* 按矩形参数构造地板洞口
	* @params: 矩形相对参数（8个浮点数）
	*/
	Hatch(std::vector<float> params);

	/*
	* 析构地板洞口
	*/
	~Hatch();

	/*
	* 按楼层尺寸将相对参数实例化为实际矩形
	* @width, height: 楼层尺寸
	*/
	void InstanciateQuad(float width, float height);

private:
	// 矩形相对参数
	std::vector<float> params;
};

// 独立房间
class Single : public Quad {
public:
	/*
	* 按矩形参数构造独立房间
	* @params: 矩形相对参数（8个浮点数）
	*/
	Single(std::vector<float> params);

	/*
	* 析构独立房间
	*/
	~Single();

	/*
	* 获取朝向
	*/
	FACE_DIRECTION GetDirection() const;

	/*
	* 设置朝向
	* @direction: 朝向
	*/
	void SetDirection(FACE_DIRECTION direction);

	/*
	* 获取所有门
	*/
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetDoors() const;

	/*
	* 在某一侧添加门
	* @direction: 方向
	* @positions: 门的相对位置参数列表
	*/
	void AddDoor(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);

	/*
	* 获取所有窗
	*/
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetWindows() const;

	/*
	* 在某一侧添加窗
	* @direction: 方向
	* @positions: 窗的相对位置参数列表
	*/
	void AddWindow(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);

	/*
	* 按楼层尺寸将相对参数实例化为实际矩形
	* @width, height: 楼层尺寸
	*/
	void InstanciateQuad(float width, float height);

private:
	// 朝向
	FACE_DIRECTION direction;

	// 各方向的门
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> doors;

	// 各方向的窗
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> windows;

	// 矩形相对参数
	std::vector<float> params;
};

// 联排房间
class Row : public Quad {
public:
	/*
	* 按矩形参数构造联排房间
	* @params: 矩形相对参数（8个浮点数）
	*/
	Row(std::vector<float> params);

	/*
	* 析构联排房间
	*/
	~Row();

	/*
	* 获取朝向
	*/
	FACE_DIRECTION GetDirection() const;

	/*
	* 设置朝向
	* @direction: 朝向
	*/
	void SetDirection(FACE_DIRECTION direction);

	/*
	* 获取所有门
	*/
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetDoors() const;

	/*
	* 在某一侧添加门
	* @direction: 方向
	* @positions: 门的相对位置参数列表
	*/
	void AddDoor(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);

	/*
	* 获取所有窗
	*/
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> GetWindows() const;

	/*
	* 在某一侧添加窗
	* @direction: 方向
	* @positions: 窗的相对位置参数列表
	*/
	void AddWindow(FACE_DIRECTION direction, std::vector<std::vector<float>> positions);

	/*
	* 按楼层尺寸将相对参数实例化为实际矩形
	* @width, height: 楼层尺寸
	*/
	void InstanciateQuad(float width, float height);

private:
	// 朝向
	FACE_DIRECTION direction;

	// 各方向的门
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> doors;

	// 各方向的窗
	std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> windows;

	// 矩形相对参数
	std::vector<float> params;
};

// 楼层
class Floor : public Quad {
public:
	/*
	* 按楼层编号及尺寸构造楼层
	* @level: 楼层编号
	* @width, height: 楼层尺寸
	*/
	Floor(int level, float width, float height);

	/*
	* 析构楼层
	*/
	~Floor();

	/*
	* 获取楼层编号
	*/
	int GetLevel() const;

	/*
	* 获取所有楼梯
	*/
	std::vector<Stair>& GetStairs();

	/*
	* 获取所有电梯
	*/
	std::vector<Elevator>& GetElevators();

	/*
	* 获取所有斜坡
	*/
	std::vector<Ramp>& GetRamps();

	/*
	* 获取所有天花板
	*/
	std::vector<Ceiling>& GetCeilings();

	/*
	* 获取所有地板
	*/
	std::vector<Ground>& GetGrounds();

	/*
	* 获取所有走廊
	*/
	std::vector<Corridor>& GetCorridors();

	/*
	* 获取所有独立房间
	*/
	std::vector<Single>& GetSingles();

	/*
	* 获取所有联排房间
	*/
	std::vector<Row>& GetRows();

	/*
	* 获取所有地板洞口
	*/
	std::vector<Hatch>& GetHatches();

	/*
	* 添加楼梯
	* @stair: 楼梯
	*/
	void AddStair(Stair stair);

	/*
	* 添加电梯
	* @elevator: 电梯
	*/
	void AddElevator(Elevator elevator);

	/*
	* 添加斜坡
	* @ramp: 斜坡
	*/
	void AddRamp(Ramp ramp);

	/*
	* 添加天花板
	* @ceiling: 天花板
	*/
	void AddCeiling(Ceiling ceiling);

	/*
	* 添加地板
	* @ground: 地板
	*/
	void AddGround(Ground ground);

	/*
	* 添加走廊
	* @corridor: 走廊
	*/
	void AddCorridor(Corridor corridor);

	/*
	* 添加独立房间
	* @single: 独立房间
	*/
	void AddSingle(Single single);

	/*
	* 添加联排房间
	* @row: 联排房间
	*/
	void AddRow(Row row);

	/*
	* 添加地板洞口
	* @hatch: 地板洞口
	*/
	void AddHatch(Hatch hatch);

	/*
	* 分配并返回下一个房间编号
	*/
	int AssignNumber();

private:
	// 楼层编号
	int level;

	// 下一个待分配的房间编号
	int number;

	// 楼梯
	std::vector<Stair> stairs;

	// 电梯
	std::vector<Elevator> elevators;

	// 斜坡
	std::vector<Ramp> ramps;

	// 天花板
	std::vector<Ceiling> ceilings;

	// 地板
	std::vector<Ground> grounds;

	// 走廊
	std::vector<Corridor> corridors;

	// 独立房间
	std::vector<Single> singles;

	// 联排房间
	std::vector<Row> rows;

	// 地板洞口
	std::vector<Hatch> hatches;
};

// 轿厢
class Cabin {
public:
	/*
	* 构造轿厢
	* @name: 名称
	* @temp: 温度
	* @idx: 编号
	* @minFloor, maxFloor: 可达楼层范围
	* @script: 关联剧情
	*/
	Cabin(std::string name, int temp, int idx, int minFloor, int maxFloor, Script* script);

	/*
	* 析构轿厢
	*/
	~Cabin();

	/*
	* 获取名称
	*/
	std::string GetName() const;

	/*
	* 获取温度
	*/
	int GetTemp() const;

	/*
	* 获取编号
	*/
	int GetIdx() const;

	/*
	* 获取可达楼层下限
	*/
	int GetMinFloor() const;

	/*
	* 获取可达楼层上限
	*/
	int GetMaxFloor() const;

	/*
	* 获取当前所在楼层
	*/
	int GetCurrentFloor() const;

	/*
	* 获取关联剧情
	*/
	Script* GetScript() const;

	/*
	* 添加可选项
	* @option: 选项
	*/
	void AddOption(const std::string& option);

	/*
	* 移除可选项
	* @option: 选项
	*/
	void RemoveOption(const std::string& option);

	/*
	* 获取所有可选项
	*/
	std::vector<std::string> GetOptions() const;

private:
	// 名称
	std::string name;

	// 温度
	int temp;

	// 编号
	int idx;

	// 可达楼层下限
	int minFloor;

	// 可达楼层上限
	int maxFloor;

	// 当前所在楼层
	int currentFloor;

	// 关联剧情
	OBJECT_HOLDER Script* script;

	// 可选项
	std::vector<std::string> options;
};

// 楼层布局模板集合
class Layout {
public:
	// 楼梯模板
	std::unordered_map<std::string, std::vector<std::vector<Stair>>> templateStairs;

	// 电梯模板
	std::unordered_map<std::string, std::vector<std::vector<Elevator>>> templateElevators;

	// 斜坡模板
	std::unordered_map<std::string, std::vector<std::vector<Ramp>>> templateRamps;

	// 天花板模板
	std::unordered_map<std::string, std::vector<std::vector<Ceiling>>> templateCeilings;

	// 地板模板
	std::unordered_map<std::string, std::vector<std::vector<Ground>>> templateGrounds;

	// 走廊模板
	std::unordered_map<std::string, std::vector<std::vector<Corridor>>> templateCorridors;

	// 独立房间模板
	std::unordered_map<std::string, std::vector<std::vector<Single>>> templateSingles;

	// 联排房间模板
	std::unordered_map<std::string, std::vector<std::vector<Row>>> templateRows;

	// 地板洞口模板
	std::unordered_map<std::string, std::vector<std::vector<Hatch>>> templateHatches;
};

// 建筑实体
class Building : public Quad {
public:
	/*
	* 禁止默认构造
	*/
	Building() = delete;

	/*
	* 通过建筑静态类型标识从工厂构造建筑
	* @factory: 建筑工厂
	* @building: 建筑静态类型标识
	*/
	Building(BuildingFactory* factory, const std::string& building);

	/*
	* 析构建筑
	*/
	~Building();

	/*
	* 获取建筑类型
	*/
	std::string GetType() const;

	/*
	* 获取建筑名称
	*/
	std::string GetName() const;

	/*
	* 获取所在地块
	*/
	Block* GetParentBlock() const;

	/*
	* 设置所在地块
	* @block: 所在地块
	*/
	void SetParent(Block* block);

	/*
	* 获取所在园区
	*/
	Zone* GetParentZone() const;

	/*
	* 设置所在园区
	* @zone: 所在园区
	*/
	void SetParent(Zone* zone);

	/*
	* 获取总地面层数
	*/
	int GetLayers() const;

	/*
	* 设置总地面层数
	* @layers: 总地面层数
	*/
	void SetLayers(int layers);

	/*
	* 获取总地下层数
	*/
	int GetBasements() const;

	/*
	* 设置总地下层数
	* @basements: 总地下层数
	*/
	void SetBasements(int basements);

	/*
	* 获取层高
	*/
	float GetHeight() const;

	/*
	* 设置层高
	* @height: 层高
	*/
	void SetHeight(float height);

	/*
	* 获取建筑楼体范围
	*/
	const Quad GetConstruction() const;

	/*
	* 放置建筑楼体
	*/
	void PlaceConstruction();

	/*
	* 获取外立面材质
	*/
	std::string GetWallTexture() const;

	/*
	* 获取天花板材质
	*/
	std::string GetCeilingTexture() const;

	/*
	* 获取地板材质
	*/
	std::string GetGroundTexture() const;

	/*
	* 获取装饰
	*/
	std::vector<std::pair<std::string, std::vector<float>>>& GetDecorations();

	/*
	* 获取完整地址
	*/
	std::string GetAddress();

	/*
	* 获取寻址锚点
	*/
	const std::vector<Node*> GetPivots();

	/*
	* 获取建筑自身的四角四边（非持有引用）
	*/
	const QuadBoundary& GetBoundary() const;

	/*
	* 设置建筑自身的四角四边
	* @boundary: 四角四边（非持有引用）
	*/
	void SetBoundary(const QuadBoundary& boundary);

	/*
	* 获取楼内全部组合
	*/
	std::vector<Component*>& GetComponents();

	/*
	* 获取楼内全部房间
	*/
	std::vector<Room*>& GetRooms();

	/*
	* 按楼层编号获取楼层
	* @level: 楼层编号
	*/
	Floor* GetFloor(int level) const;

	/*
	* 获取轿厢
	*/
	std::vector<Cabin*>& GetCabins();

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
	* 采样建筑面积
	*/
	float RandomAcreage();

	/*
	* 获取面积下限
	*/
	float GetAcreageMin();

	/*
	* 获取面积上限
	*/
	float GetAcreageMax();

	/*
	* 获取建筑中心世界位置
	* @x, y: 输出的世界坐标
	*/
	void GetPosition(float& x, float& y) const;

	/*
	* 布局内部对象
	* @layout: 楼层布局模板集合
	*/
	void LayoutBuilding(Layout* layout);

	/*
	* 放置寻址锚点
	* @building: 建筑所占据的矩形区域
	*/
	void PlacePivots(Quad* building);

	/*
	* 读取所有布局模板
	* @paths: 模板文件路径列表
	*/
	static Layout* ReadTemplates(const std::vector<std::string>& paths);

private:
	/*
	* 添加轿厢
	* @cabin: 轿厢
	*/
	void AddCabin(Cabin* cabin);

	/*
	* 根据模板生成一层楼层
	* @level: 楼层编号
	* @face: 模板朝向
	* @name: 模板名称
	* @layout: 楼层布局模板集合
	*/
	void ReadFloor(int level, int face, const std::string& name, Layout* layout);

	/*
	* 按照单一模板生成所有楼层
	* @face: 模板朝向
	* @name: 模板名称
	* @layout: 楼层布局模板集合
	*/
	void ReadFloors(int face, const std::string& name, Layout* layout);

	/*
	* 按照层数个模板生成所有楼层
	* @face: 模板朝向
	* @names: 每层对应的模板名称
	* @layout: 楼层布局模板集合
	*/
	void ReadFloors(int face, const std::vector<std::string>& names, Layout* layout);

	/*
	* 为模板中第slot个独立房间生成房间
	* @level: 楼层编号
	* @slot: 独立房间在模板中的序号
	* @name: 房间类型名称
	* @component, factory: 所属组合及房间工厂
	*/
	void AssignRoom(int level, int slot, const std::string& name,
		Component* component, RoomFactory* factory);

	/*
	* 为模板中第slot个联排房间生成房间
	* @level: 楼层编号
	* @slot: 联排房间在模板中的序号
	* @name: 房间类型名称
	* @acreage: 单间面积
	* @component, factory: 所属组合及房间工厂
	*/
	void ArrangeRow(int level, int slot, const std::string& name, float acreage,
		Component* component, RoomFactory* factory);

	/*
	* Tool
	* 根据转向修改矩形参数
	* @params: 矩形相对参数（8个浮点数）
	* @face: 转向
	*/
	static std::vector<float> InverseParams(const std::vector<float>& params, int face);

	/*
	* Tool
	* 根据转向修改朝向参数
	* @direction: 朝向
	* @face: 转向
	*/
	static int InverseDirection(int direction, int face);

	// 模组对象
	OBJECT_HOLDER BuildingMod* mod;

	// 工厂
	BuildingFactory* factory;

	// 建筑类型
	std::string type;

	// 建筑名称
	std::string name;

	// 所在园区
	Zone* parentZone;

	// 所在街区
	Block* parentBlock;

	// 地面层数
	int layers;

	// 地下层数
	int basements;

	// 层高
	float height;

	// 建筑楼体范围
	Quad construction;

	// 外立面材质
	std::string wallTexture;

	// 天花板材质
	std::string ceilingTexture;

	// 地板材质
	std::string groundTexture;

	// 装饰
	std::vector<std::pair<std::string, std::vector<float>>> decorations;

	// 完整地址
	std::string address;

	// 寻址锚点
	OBJECT_HOLDER std::vector<Node*> pivots;

	// 自身的四角四边（非持有引用）
	QuadBoundary boundary;

	// 是否由政府拥有
	bool stated;

	// 私人房东
	Person* owner;

	// 关联剧情
	OBJECT_HOLDER Script* script;

	// 轿厢
	OBJECT_HOLDER std::vector<Cabin*> cabins;

	// 楼层
	OBJECT_HOLDER std::vector<Floor*> floors;

	// 组合
	OBJECT_HOLDER std::vector<Component*> components;

	// 房间
	OBJECT_HOLDER std::vector<Room*> rooms;
};

// 空建筑
class EmptyBuilding : public BuildingMod {
public:
	/*
	* 构造空建筑
	*/
	EmptyBuilding();

	/*
	* 析构空建筑
	*/
	virtual ~EmptyBuilding();

	/*
	* Override
	* 建筑静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 建筑动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 建筑实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 建筑在各功能分区的生成权重
	*/
	static std::vector<float> GetPowers();

	// 按地块计算应生成的建筑数量
	static std::function<int(const Lot*, int, int)> BuildingAssigner;

	/*
	* Override
	* 采样建筑面积
	*/
	virtual float RandomAcreage() override;

	/*
	* Override
	* 布局建筑
	* @quad: 建筑所占据的矩形区域
	*/
	virtual void LayoutBuilding(const Quad* quad) override;

	/*
	* Override
	* 放置楼栋
	*/
	virtual void PlaceConstruction() override;

	/*
	* Override
	* 放置寻址锚点
	* @building: 建筑所占据的矩形区域
	*/
	virtual void PlacePivots(Quad* building) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

