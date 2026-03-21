#pragma once

#include "roadnet_mod.h"

#include "plot.h"


// 子类注册函数
typedef void (*RegisterModRoadnetsFunc)(RoadnetFactory* factory);

// 空路网
class EmptyRoadnet : public RoadnetMod {
public:
    EmptyRoadnet();
    virtual ~EmptyRoadnet();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual void DistributeRoadnet(int width, int height,
        std::function<std::string(int, int)> get) override;
};

// 路网实体
class Roadnet {
public:
    Roadnet() = delete;
    Roadnet(RoadnetFactory* factory, std::string roadnet);
    ~Roadnet();

    // 获取类型
    std::string GetType() const;

    // 获取名称
    std::string GetName() const;

    // 构建路网
    void DistributeRoadnet(int width, int height,
        std::function<std::string(int, int)> get);

    // 获取所有节点
    const std::vector<Node*>& GetNodes() const;

    // 获取所有连接
    const std::vector<Connection*>& GetConnections() const;

    // 获取所有地块
    const std::vector<Plot*>& GetPlots() const;

    // 分配地块地址
    void AllocateAddress();

    // 按地址查找
    Plot* LocatePlot(std::string road, int id) const;

    // 自动寻路
    //const std::vector<Connection> AutoNavigate(
    //    const std::vector<std::pair<Connection, float>>& startRoads, const std::vector<std::pair<Connection, float>>& endRoads) const;
    //const std::vector<Connection> AutoNavigate(
    //    const Plot* startPlot, const Plot* endPlot) const;
    //const std::vector<Connection> AutoNavigate(
    //    const std::vector<std::pair<Connection, float>>& startRoads, const Plot* endPlot) const;
    //const std::vector<Connection> AutoNavigate(
    //    const Plot* startPlot, const std::vector<std::pair<Connection, float>>& endRoads) const;

private:
    RoadnetMod* mod;
    RoadnetFactory* factory;

    std::string type;
    std::string name;

    VARIABLE_HOLDER std::vector<Node*> nodes;
    VARIABLE_HOLDER std::vector<Connection*> connections;
    VARIABLE_HOLDER std::vector<Plot*> plots;
    //VARIABLE_HOLDER std::vector<Route*> routes;
    std::unordered_map<std::string, std::vector<Plot*>> addresses;
};

