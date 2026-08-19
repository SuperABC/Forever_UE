#pragma once

#include "map/room_mod.h"


// 住宅房间
class ResidentialRoom : public RoomMod {
public:
	/*
	* 构造住宅房间
	*/
	ResidentialRoom();

	/*
	* 析构住宅房间
	*/
	virtual ~ResidentialRoom();

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

// 商店房间
class ShopRoom : public RoomMod {
public:
	/*
	* 构造商店房间
	*/
	ShopRoom();

	/*
	* 析构商店房间
	*/
	virtual ~ShopRoom();

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

// 仓库房间
class WarehouseRoom : public RoomMod {
public:
	/*
	* 构造仓库房间
	*/
	WarehouseRoom();

	/*
	* 析构仓库房间
	*/
	virtual ~WarehouseRoom();

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

// 停车场房间
class ParkingRoom : public RoomMod {
public:
	/*
	* 构造停车场房间
	*/
	ParkingRoom();

	/*
	* 析构停车场房间
	*/
	virtual ~ParkingRoom();

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

// 诊室
class DoctorRoom : public RoomMod {
public:
	/*
	* 构造诊室
	*/
	DoctorRoom();

	/*
	* 析构诊室
	*/
	virtual ~DoctorRoom();

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

// 太平间
class MorgueRoom : public RoomMod {
public:
	/*
	* 构造太平间
	*/
	MorgueRoom();

	/*
	* 析构太平间
	*/
	virtual ~MorgueRoom();

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

// 工厂房间
class FactoryRoom : public RoomMod {
public:
	/*
	* 构造工厂房间
	*/
	FactoryRoom();

	/*
	* 析构工厂房间
	*/
	virtual ~FactoryRoom();

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

// 大堂
class LobbyRoom : public RoomMod {
public:
	/*
	* 构造大堂
	*/
	LobbyRoom();

	/*
	* 析构大堂
	*/
	virtual ~LobbyRoom();

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

// 客房
class BunkRoom : public RoomMod {
public:
	/*
	* 构造客房
	*/
	BunkRoom();

	/*
	* 析构客房
	*/
	virtual ~BunkRoom();

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

// 办公室
class OfficeRoom : public RoomMod {
public:
	/*
	* 构造办公室
	*/
	OfficeRoom();

	/*
	* 析构办公室
	*/
	virtual ~OfficeRoom();

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

// 挂号处
class RegisterRoom : public RoomMod {
public:
	/*
	* 构造挂号处
	*/
	RegisterRoom();

	/*
	* 析构挂号处
	*/
	virtual ~RegisterRoom();

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

// 等候室
class WaitingRoom : public RoomMod {
public:
	/*
	* 构造等候室
	*/
	WaitingRoom();

	/*
	* 析构等候室
	*/
	virtual ~WaitingRoom();

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
