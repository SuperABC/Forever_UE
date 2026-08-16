#pragma once

#include "traffic/vehicle_mod.h"

#include "class.h"

#include <unordered_set>


// 子类注册函数
typedef void (*RegisterModVehiclesFunc)(VehicleFactory* factory);

// 子类注册收尾函数
typedef void (*FinishModVehiclesFunc)(VehicleFactory* factory);

// 载具实体
class Vehicle {
public:
	/*
	* 禁止默认构造
	*/
	Vehicle() = delete;

	// 通过标签从工厂构造
	/*
	* 构造载具实体
	* @factory: 载具工厂
	* @label: 载具类型
	*/
	Vehicle(VehicleFactory* factory, const std::string& label);

	/*
	* 析构载具实体
	*/
	~Vehicle();

	/*
	* 载具动态类型标识
	* @return: 动态类型标识
	*/
	std::string GetType() const;

	/*
	* 载具实例唯一名称
	* @return: 实例唯一名称
	*/
	std::string GetName() const;

	/*
	* 获取载具资产路径
	* @return: 资产路径
	*/
	std::string GetPath() const;

	/*
	* 获取载具当前所在房间
	* @return: 房间指针
	*/
	Room* GetRoom() const;

	/*
	* 获取载具当前位置（）
	* @return: x/y/z/rotation
	*/
	std::vector<float> GetPosition() const;

	/*
	* 将载具移入指定房间
	* @room: 目标房间
	*/
	void EnterRoom(Room* room);

	/*
	* 将载具从当前房间移出
	*/
	void LeaveRoom();

	/*
	* 设置载具位置和朝向
	* @x: X坐标
	* @y: Y坐标
	* @z: Z坐标
	* @r: 旋转角度
	*/
	void SetPosition(float x, float y, float z, float r);

	/*
	* 定义载具属性
	*/
	void SetupVehicle();

	/*
	* 获取载具剧情脚本
	* @return: 剧情脚本
	*/
	Script* GetScript() const;

	/*
	* 获取载具拥有者
	* @return: 拥有者指针，nullptr 表示玩家所有
	*/
	Person* GetOwner() const;

	/*
	* 设置载具拥有者
	* @person: 拥有者指针，nullptr 表示玩家所有
	*/
	void SetOwner(Person* person);

	/*
	* 添加交互选项
	* @option: 选项名称
	* @return: 添加成功返回true，已存在返回false
	*/
	bool AddOption(const std::string& option);

	/*
	* 移除交互选项
	* @option: 选项名称
	* @return: 移除成功返回true，不存在返回false
	*/
	bool RemoveOption(const std::string& option);

	/*
	* 获取所有交互选项集合
	* @return: 选项名称集合
	*/
	std::unordered_set<std::string> GetOptions() const;

private:
	// 模组对象
	OBJECT_HOLDER VehicleMod* mod;

	// 工厂
	VehicleFactory* factory;

	// 载具类型
	std::string type;

	// 载具名称
	std::string name;

	// 载具资产路径
	std::string path;

	// 载具宽度
	float width;

	// 载具长度
	float length;

	// 拥有者
	Person* owner;

	// 司机
	Person* driver;

	// 当前所在房间
	Room* room;

	// 当前位置
	std::vector<float> xyzr;

	// 关联剧情
	OBJECT_HOLDER Script* script;

	// 交互选项
	std::unordered_set<std::string> options;
};

// 空载具
class EmptyVehicle : public VehicleMod {
public:
	/*
	* 构造空载具
	*/
	EmptyVehicle();
	
	/*
	* 析构空载具
	*/
	virtual ~EmptyVehicle();

	/*
	* 空载具静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 空载具动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 空载具实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* 自定义载具标签
	* @return: 载具标签
	*/
	static std::string GetLabel();

	/*
	* 定义载具属性
	*/
	virtual void SetupVehicle() override;

private:
	// 空载具实例计数器
	static int count;

	// 实例唯一ID
	int id;
	
	// 实例名称
	std::string name;
};
