#pragma once

#include "map/room_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModRoomsFunc)(RoomFactory* factory);

// 房间实体
class Room : public Quad {
public:
	// 门窗类
	using WallHole = std::unordered_map<
		FACE_DIRECTION,
		std::vector<std::pair<std::vector<float>, Quad>>
	>;

	/*
	* 禁止默认构造
	*/
	Room() = delete;

	/*
	* 通过房间静态类型标识从工厂构造房间
	* @factory: 房间工厂
	* @component: 房间静态类型标识
	*/
	Room(RoomFactory* factory, const std::string& component);

	/*
	* 析构房间
	*/
	~Room();

	/*
	* 获取房间类型
	*/
	std::string GetType() const;

	/*
	* 获取房间名称
	*/
	std::string GetName() const;

	/*
	* 获取所在建筑
	*/
	Building* GetParentBuilding() const;

	/*
	* 设置所在建筑
	* @building: 所在建筑
	*/
	void SetParent(Building* building);

	/*
	* 获取所在组合
	*/
	Component* GetParentComponent() const;

	/*
	* 设置所在组合
	* @component: 所在组合
	*/
	void SetParent(Component* component);

	/*
	* 获取所在层数
	*/
	int GetLayer() const;

	/*
	* 设置所在层数
	* @layer: 所在层数
	*/
	void SetLayer(int layer);

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
	* 获取门框
	*/
	const WallHole& GetDoors() const;

	/*
	* 设置门框
	* @doors: 门框
	*/
	void SetDoors(const WallHole& doors);

	/*
	* 获取窗框
	*/
	const WallHole& GetWindows() const;

	/*
	* 设置窗框
	* @windows: 窗框
	*/
	void SetWindows(const WallHole& windows);

	/*
	* 获取门牌号
	*/
	std::string GetNumber() const;

	/*
	* 设置门牌号
	* @floor: 楼层编号
	* @number: 楼层内房间序号
	*/
	void SetNumber(int floor, int number);

	/*
	* 读取房间配置
	*/
	void ConfigRoom();

	/*
	* 获取完整地址
	*/
	std::string GetAddress() const;

	/*
	* 获取寻址锚点
	*/
	const std::vector<Node*> GetPivots();

	/*
	* 放置寻址锚点
	* @room: 房间所占据的矩形区域
	*/
	void PlacePivots(Quad* room);

	/*
	* 获取导航图节点（房间中心点）
	*/
	Node* GetNavigationNode() const;

	/*
	* 设置导航图节点（房间持有，负责释放）
	* @node: 导航图节点
	*/
	void SetNavigationNode(Node* node);

	/*
	* 获取世界坐标
	* @x, y: 房间局部坐标
	*/
	std::pair<float, float> GetPosition(float x, float y) const;

	/*
	* 是否可作为住宅
	*/
	bool IsResidential() const;

	/*
	* 是否可作为工位
	*/
	bool IsWorkspace() const;

	/*
	* 是否可作为仓库
	*/
	bool IsStorage() const;

	/*
	* 是否可作为工坊
	*/
	bool IsManufacture() const;

	/*
	* 是否可作为停车位
	*/
	bool IsParking() const;

	/*
	* 作为住宅容纳人数
	*/
	int ResidentialCapacity() const;

	/*
	* 作为工位容纳人数
	*/
	int WorkspaceCapacity() const;

	/*
	* 作为仓库各分区容量
	*/
	std::unordered_map<std::string, float> StorageConfig() const;

	/*
	* 作为工坊包含生产线种类
	*/
	std::vector<std::string> ManufactureTypes() const;

	/*
	* 获取停车位信息
	*/
	std::vector<std::vector<float>> ParkingSpaces() const;

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
	* 获取住户
	*/
	const std::vector<Person*>& GetTenants() const;

	/*
	* 添加住户
	* @person: 住户
	*/
	void AddTenant(Person* person);

	/*
	* 移除住户
	* @name: 住户名称
	*/
	bool RemoveTenant(const std::string& name);

	/*
	* 获取工人
	*/
	const std::vector<Person*>& GetWorkers() const;

	/*
	* 添加工人
	* @person: 工人
	*/
	void AddWorker(Person* person);

	/*
	* 移除工人
	* @name: 工人名称
	*/
	bool RemoveWorker(const std::string& name);

	/*
	* 获取仓库
	*/
	std::vector<Storage*> GetStorage() const;

	/*
	* 添加仓库
	* @storage: 仓库
	*/
	void AddStorage(Storage* storage);

	/*
	* 清空仓库
	*/
	void ClearStorages();

	/*
	* 获取工坊
	*/
	std::vector<Manufacture*> GetManufactures() const;

	/*
	* 添加工坊
	* @manufacture: 工坊
	*/
	void AddManufacture(Manufacture* manufacture);

	/*
	* 清空工坊
	*/
	void ClearManufactures();

	/*
	* 获取停车位
	*/
	std::vector<Quad> GetParkings() const;

	/*
	* 获取屋内车辆
	*/
	std::vector<Vehicle*> GetVehicles() const;

	/*
	* 车辆进入
	* @vehicle: 车辆
	*/
	void AddVehicle(Vehicle* vehicle);

	/*
	* 车辆离开
	* @vehicle: 车辆
	*/
	void RemoveVehicle(Vehicle* vehicle);

	/*
	* 清空车辆
	*/
	void ClearVehicles();

private:
	// 模组对象
	OBJECT_HOLDER RoomMod* mod;

	// 工厂
	RoomFactory* factory;

	// 房间类型
	std::string type;

	// 房间名称
	std::string name;

	// 所在建筑
	Building* parentBuilding;

	// 所在组合
	Component* parentComponent;

	// 所在楼层
	int layer;

	// 朝向
	FACE_DIRECTION direction;

	// 门
	WallHole doors;

	// 窗
	WallHole windows;

	// 门牌号
	std::string number;

	// 寻址锚点
	OBJECT_HOLDER std::vector<Node*> pivots;

	// 导航图节点（房间中心点）
	OBJECT_HOLDER Node* navigationNode;

	// 是否由政府拥有
	bool stated;

	// 私人房东
	Person* owner;

	// 屋内住户
	std::vector<Person*> tenants;

	// 屋内工人
	std::vector<Person*> workers;

	// 屋内仓库
	OBJECT_HOLDER std::vector<Storage*> storages;

	// 屋内工坊
	OBJECT_HOLDER std::vector<Manufacture*> manufactures;

	// 屋内停车位
	std::vector<Quad> parkings;

	// 屋内车辆
	std::vector<Vehicle*> vehicles;
};

// 空房间
class EmptyRoom : public RoomMod {
public:
	/*
	* 构造空房间
	*/
	EmptyRoom();

	/*
	* 析构空房间
	*/
	virtual ~EmptyRoom();

	/*
	* Override
	* 房间静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 房间动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 房间实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 配置房间属性
	* @quad: 房间所占据的矩形区域
	* @direction: 房间朝向
	*/
	virtual void ConfigRoom(Quad* quad, FACE_DIRECTION direction) override;

	/*
	* Override
	* 放置寻址锚点
	* @room: 房间所占据的矩形区域
	*/
	virtual void PlacePivots(Quad* room) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

