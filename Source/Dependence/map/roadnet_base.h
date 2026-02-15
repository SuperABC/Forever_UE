#pragma once

#include "../common/plot.h"

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>


class Roadnet {
public:
    Roadnet() = default;
    virtual ~Roadnet() = default;

	// 子类实现方法

    // 动态返回路网名称
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 在平原上生成路网
    virtual void DistributeRoadnet(int width, int height,
        std::function<std::string(int, int)> get) = 0;

	// 父类实现方法

    // 提供只读访问接口
    const std::vector<Node>& GetNodes() const;
    const std::vector<Connection>& GetConnections() const;
    const std::vector<std::shared_ptr<Plot>>& GetPlots() const;

    // 分配地址
	void AllocateAddress();
	std::shared_ptr<Plot> LocatePlot(const std::string& road, int id) const;

    // 自动寻路
    const std::vector<Connection> AutoNavigate(
        std::vector<std::pair<Connection, float>> startRoads, std::vector<std::pair<Connection, float>> endRoads) const;
    const std::vector<Connection> AutoNavigate(
        std::shared_ptr<Plot> startPlot, std::shared_ptr<Plot> endPlot) const;
    const std::vector<Connection> AutoNavigate(
        std::vector<std::pair<Connection, float>> startRoads, std::shared_ptr<Plot> endPlot) const;
    const std::vector<Connection> AutoNavigate(
        std::shared_ptr<Plot> startPlot, std::vector<std::pair<Connection, float>> endRoads) const;

protected:
    std::vector<Node> nodes;
    std::vector<Connection> connections;
    std::vector<std::shared_ptr<Plot>> plots;

    std::unordered_map<std::string, std::vector<std::shared_ptr<Plot>>> addresses;
};

class RoadnetFactory {
public:
    void RegisterRoadnet(const std::string& id, std::function<Roadnet* ()> creator);
    Roadnet* CreateRoadnet(const std::string& id);
    bool CheckRegistered(const std::string& id);
    void SetConfig(std::string name, bool config);
    Roadnet* GetRoadnet() const;

private:
    std::unordered_map<std::string, std::function<Roadnet* ()>> registries;
    std::unordered_map<std::string, bool> configs;
};


