#pragma once

#include "../common/utility.h"
#include "../common/error.h"
#include "../common/geometry.h"

#include <string>
#include <functional>


class RoadnetMod {
public:
	// 无构造
	RoadnetMod();

	// 无析构
	virtual ~RoadnetMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 构建路网
	COMSTOM_INIT virtual void DistributeRoadnet(int width, int height,
		std::function<std::string(int, int)> get) = 0;

	// 路口
	std::vector<Node> nodes;

	// 路线
	std::vector<Connection> connections;

	// 地块
	std::vector<std::pair<Lot, std::vector<std::pair<Connection, float>>>> lots;
};

class RoadnetFactory {
public:
	// 注册路网
	void RegisterRoadnet(const std::string& id,
		std::function<RoadnetMod* ()> creator, std::function<void(RoadnetMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建路网
	RoadnetMod* CreateRoadnet(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 获取启用路网
	std::string GetRoadnet() const;

	// 析构路网
	void DestroyRoadnet(RoadnetMod* roadnet) const;

private:
    // 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<RoadnetMod* ()>, std::function<void(RoadnetMod*)>>
	> registries;
	
    // 启用配置
	std::unordered_map<std::string, bool> configs;
};

