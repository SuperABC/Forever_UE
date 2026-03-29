#pragma once

#include "asset_mod.h"


// 园区资产
class ZoneAsset : public AssetMod {
public:
	ZoneAsset();
	virtual ~ZoneAsset();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

private:
	static int count;

	int id;
	std::string name;
};

// 建筑资产
class BuildingAsset : public AssetMod {
public:
	BuildingAsset();
	virtual ~BuildingAsset();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

private:
	static int count;

	int id;
	std::string name;
};

// 房间资产
class RoomAsset : public AssetMod {
public:
	RoomAsset();
	virtual ~RoomAsset();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

private:
	static int count;

	int id;
	std::string name;
};

