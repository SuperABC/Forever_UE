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

    // 动态返回组合静态信息
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 是否独立于路网
    virtual bool StandAlone() const = 0;

    // 是否需要建模
	virtual bool RouteVisible() const = 0;

    // 父类实现方法

    // 管理节点
	bool AddStation(std::string connectStation, int connectSlot, Station* newStation, int newSlot, std::string name);
	bool AddConnection(std::string connect1, int slot1, std::string connect2, int slot2);
	std::vector<BezierCurve*> GetConnections();

protected:
	std::unordered_map<std::string, Station*> stations;

	std::vector<BezierCurve *> connections;
};

class RouteFactory {
public:
    void RegisterRoute(const std::string& id, std::function<Route* ()> creator);
    Route* CreateRoute(const std::string& id);
    bool CheckRegistered(const std::string& id);
    void SetConfig(std::string name, bool config);

private:
    std::unordered_map<std::string, std::function<Route* ()>> registries;
    std::unordered_map<std::string, bool> configs;
};