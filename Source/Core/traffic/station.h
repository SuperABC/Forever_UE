#pragma once

#include "station_mod.h"


// 子类注册函数
typedef void (*RegisterModStationsFunc)(StationFactory* factory);

// 站点实体
class Station {
public:
	// 禁止默认构造
	Station() = delete;

	// 通过类型从工厂构造
	Station(StationFactory* factory, const std::string& station);

	// 析构站点
	~Station();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

private:
	// 模组对象
	OBJECT_HOLDER StationMod* mod;

	// 工厂
	StationFactory* factory;

	// 站点类型
	std::string type;

	// 站点名称
	std::string name;

};

// 空站点
class EmptyStation : public StationMod {
public:
	EmptyStation();
	virtual ~EmptyStation();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

private:
	static int count;

	int id;
	std::string name;
};

