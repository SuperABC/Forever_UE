#pragma once

#include "asset_base.h"
#include "zone_base.h"


// 子类注册函数
typedef void (*RegisterModAssetsFunc)(AssetFactory* factory);

// 园区资产
class ZoneAsset : public Asset {
public:
	ZoneAsset();
	~ZoneAsset();

	static std::string GetId();
	virtual std::string GetType() const override;
	virtual std::string GetName() const override;
};

// 建筑资产
class BuildingAsset : public Asset {
public:
	BuildingAsset();
	~BuildingAsset();

	static std::string GetId();
	virtual std::string GetType() const override;
	virtual std::string GetName() const override;
};

// 房间资产
class RoomAsset : public Asset {
public:
	RoomAsset();
	~RoomAsset();

	static std::string GetId();
	virtual std::string GetType() const override;
	virtual std::string GetName() const override;
};