#pragma once

#include "map/room_mod.h"


// 住宅房间
class ResidentialRoom : public RoomMod {
public:
	ResidentialRoom();
	virtual ~ResidentialRoom();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void ConfigRoom() override;

private:
	static int count;

	int id;
	std::string name;
};

// 商店房间
class ShopRoom : public RoomMod {
public:
	ShopRoom();
	virtual ~ShopRoom();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void ConfigRoom() override;

private:
	static int count;

	int id;
	std::string name;
};

// 仓库房间
class WarehouseRoom : public RoomMod {
public:
	WarehouseRoom();
	virtual ~WarehouseRoom();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void ConfigRoom() override;

private:
	static int count;

	int id;
	std::string name;
};

// 工厂房间
class FactoryRoom : public RoomMod {
public:
	FactoryRoom();
	virtual ~FactoryRoom();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void ConfigRoom() override;

private:
	static int count;

	int id;
	std::string name;
};
