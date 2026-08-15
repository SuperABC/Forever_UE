#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include "geometry.h"

#include <string>
#include <functional>


// 建筑方向（定义在building_mod.h）
enum FACE_DIRECTION : int;

// 房间Mod基类
class RoomMod {
public:
	/*
	* 构造房间
	*/
	RoomMod();

	/*
	* 析构房间
	*/
	virtual ~RoomMod();

	/*
	* Override
	* 房间静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 房间动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 房间实例唯一名称
	* @return: 实例唯一名称
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

	/*
	* Tool
	* 添加家具，px/py/rz 按房间朝北（face=FACE_NORTH）时的位形定义，
	* 内部根据 direction 自动旋转到房间实际朝向后加入家具列表
	* @path: 家具资源路径
	* @quad: 房间所占据的矩形区域，即 ConfigRoom 传入的 quad
	* @px, py: 家具位置（朝北朝向下的定义，相对房间自身左下角的绝对坐标）
	* @pz: 家具位置Z轴高度（相对所在楼层地面，不随朝向变化）
	* @sx, sy, sz: 家具缩放（绝对值，不随朝向变化）
	* @rx, ry, rz: 家具旋转（rz 为朝北朝向下的定义，度；rx/ry 暂不随朝向变化）
	* @direction: 房间当前朝向（0~3，对应 FACE_DIRECTION）
	*/
	void AddFurniture(std::string path, const Quad* quad,
		float px, float py, float pz, float sx, float sy, float sz, float rx, float ry, float rz,
		int direction);

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

	// 家具
	std::vector<std::pair<std::string, std::vector<float>>> furniture;
};

class RoomFactory {
public:
	/*
	* 注册房间
	* @id: 房间类型
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterRoom(const std::string& id,
		std::function<RoomMod* ()> creator, std::function<void(RoomMod*)> deleter);

	/*
	* 清空所有注册
	*/
	void RemoveAll();

	/*
	* 创建房间
	* @id: 房间类型
	*/
	RoomMod* CreateRoom(const std::string& id) const;

	/*
	* 检查房间是否已注册
	* @id: 房间类型
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置房间启用配置
	* @name: 房间类型
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 析构房间
	* @roomMod: 待析构的房间
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

