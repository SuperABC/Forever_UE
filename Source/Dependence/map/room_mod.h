#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include "geometry.h"

#include <string>
#include <functional>


// 建筑方向（完整定义见building_mod.h）
enum FACE_DIRECTION : int;

class RoomMod {
public:
	/*
	* 无构造
	*/
	RoomMod();

	/*
	* 无析构
	*/
	virtual ~RoomMod();

	/*
	* Override
	* 房间静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 房间动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 房间实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 配置房间属性
	* @quad: 房间所占据的矩形区域
	* @direction: 房间朝向
	*/
	COSTOM_INIT virtual void ConfigRoom(Quad* quad, FACE_DIRECTION direction) = 0;

	/*
	* Override
	* 放置寻址锚点
	* @room: 房间所占据的矩形区域
	*/
	COSTOM_INIT virtual void PlacePivots(Quad* room) = 0;

	/*
	* Tool
	* 将正北朝向下定义的模板锚点按房间当前朝向旋转后加入锚点列表
	* @point: 锚点相对参数（4个浮点数：x比例、x偏移、y比例、y偏移），按正北朝向定义
	* @face: 房间当前朝向
	*/
	void AddPivot(std::vector<float> point, int face);

	// 是否可居住
	bool isResidential;

	// 是否可工作
	bool isWorkspace;

	// 是否含仓库
	bool isStorage;

	// 是否含工坊
	bool isManufacture;

	// 是否含停车位
	bool isParking;

	// 居住空间
	int residentialCapacity;

	// 工作空间
	int workspaceCapacity;

	// 仓库属性
	std::unordered_map<std::string, float> storageConfig;

	// 工坊类型
	std::vector<std::string> manufactureTypes;

	// 车位信息
	std::vector<std::vector<float>> parkingSpaces;

	// 锚点
	std::vector<std::vector<float>> pivots;
};

class RoomFactory {
public:
	/*
	* 注册房间及其创建与销毁方式
	* @id: 房间静态类型标识
	* @creator, deleter: 创建/销毁房间实例的方法
	*/
	void RegisterRoom(const std::string& id,
		std::function<RoomMod* ()> creator, std::function<void(RoomMod*)> deleter);

	/*
	* 清空所有启用的房间配置
	*/
	void RemoveAll();

	/*
	* 按静态类型标识创建房间实例
	* @id: 房间静态类型标识
	*/
	RoomMod* CreateRoom(const std::string& id) const;

	/*
	* 检查房间是否已注册
	* @id: 房间静态类型标识
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置房间的启用配置
	* @name: 房间静态类型标识
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 销毁房间实例
	* @roomMod: 待销毁的房间实例
	*/
	void DestroyRoom(RoomMod* roomMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<RoomMod* ()>, std::function<void(RoomMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

