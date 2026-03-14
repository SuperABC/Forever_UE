#pragma once

#include "station_base.h"
#include "vehicle_base.h"

#include <string>
#include <functional>
#include <memory>


class BezierCurve {
public:
    BezierCurve(std::string station1, int slot1, glm::vec3 pos1, glm::vec3 dir1,
        std::string station2, int slot2, glm::vec3 pos2, glm::vec3 dir2);
    ~BezierCurve();

private:
    std::pair<std::string, std::string> vertices;
	std::pair<int, int> slots;
	std::vector<std::pair<glm::vec3, glm::vec3>> controlPoints;
};

class Route {
public:
    Route();
    virtual ~Route();

    // 子类实现方法

    // 统一类型定义
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 是否独立于路网
    virtual bool StandAlone() const = 0;

    // 是否需要建模
	virtual bool RouteVisible() const = 0;

    // 父类实现方法

    // 获取路线连接图
	std::vector<BezierCurve*> GetConnections();
    
    // 路线图中加入车站
	bool AddStation(std::string connectStation, int connectSlot, Station* newStation, int newSlot, std::string name);
    
    // 连接两个已有车站
	bool AddConnection(std::string connect1, int slot1, std::string connect2, int slot2);

private:
	std::unordered_map<std::string, Station*> stations;

	std::vector<BezierCurve *> connections;
};

class RouteFactory {
public:
    // 注册路线
    void RegisterRoute(const std::string& id,
        std::function<Route*()> creator, std::function<void(Route*)> deleter);

    // 创建路线（包含new操作）
    Route* CreateRoute(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 析构路线（包含delete操作）
    void DestroyRoute(Route* route) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Route*()>, std::function<void(Route*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
};

