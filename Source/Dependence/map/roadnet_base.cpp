#include "../common/error.h"
#include "../common/utility.h"

#include "roadnet_base.h"

#include <queue>
#include <unordered_map>
#include <limits>
#include <algorithm>
#include <functional>

#undef max


using namespace std;

struct pair_hash {
	template <class T1, class T2>
	size_t operator()(const pair<T1, T2>& p) const {
		auto hash1 = hash<T1>{}(p.first);
		auto hash2 = hash<T2>{}(p.second);

		return hash1 ^ (hash2 << 1) ^ (hash2 >> 31);
	}
};

// 全成员默认构造
Roadnet::Roadnet() :
	nodes(),
	connections(),
	plots(),
	routes(),
	addresses() {

}

Roadnet::~Roadnet() {
	// routes在traffic中统一创建和析构
	for (auto& plot : plots) {
		delete plot;
	}
}

// 获取所有节点
const vector<Node>& Roadnet::GetNodes() const {
	return nodes;
}

// 获取所有公路
const vector<Connection>& Roadnet::GetConnections() const {
	return connections;
}

// 获取所有地块
const vector<Plot*>& Roadnet::GetPlots() const {
	return plots;
}

// 分配地块地址
void Roadnet::AllocateAddress() {
	addresses.clear();

	for (auto& plot : plots) {
		auto roads = plot->GetRoads();
		for (const auto& road : roads) {
			if (addresses.find(road.first.GetName()) == addresses.end()) {
				addresses[road.first.GetName()] = vector<Plot*>();
			}
			addresses[road.first.GetName()].push_back(plot);
			plot->SetAddress(road.first.GetName(), (int)addresses[road.first.GetName()].size());
		}
	}
}

// 寻址地块
Plot* Roadnet::LocatePlot(const string& road, int id) const {
	if (addresses.find(road) == addresses.end()) {
		return nullptr;
	}
	auto plots = addresses.at(road);
	if (id < 0 || id >= (int)plots.size()) {
		return nullptr;
	}
	return plots[id];
}

// 自动寻路
const vector<Connection> Roadnet::AutoNavigate(
	const vector<pair<Connection, float>>& startRoads,
	const vector<pair<Connection, float>>& endRoads) const {
	if (startRoads.empty() || endRoads.empty()) return {};

	for (const auto& [connectionStart, positionStart] : startRoads) {
		for (const auto& [connectionEnd, positionEnd] : endRoads) {
			if (connectionStart == connectionEnd) {
				if (connectionStart.GetV1() == connectionEnd.GetV1()) {
					return { Connection(connectionStart.GetName(), connectionStart.GetRoadnet(),
						connectionStart.GetV1(), connectionStart.GetV2(), positionStart, positionEnd) };
				}
				else {
					return { Connection(connectionStart.GetName(), connectionStart.GetRoadnet(),
						connectionStart.GetV1(), connectionStart.GetV2(), positionStart, 1.0f - positionEnd) };
				}
			}
		}
	}

	unordered_map<int, vector<pair<int, float>>> graph;
	unordered_map<pair<int, int>, int, pair_hash> connectionMap;
	unordered_map<int, unordered_map<int, int>> connectionIndexMap;

	// 添加已有道路到图
	for (size_t i = 0; i < connections.size(); i++) {
		const auto& connection = connections[i];
		int v1 = connection.GetV1();
		int v2 = connection.GetV2();
		float dist = connection.distance();

		graph[v1].push_back({ v2, dist });
		graph[v2].push_back({ v1, dist });

		connectionMap[{v1, v2}] = (int)i;
		connectionMap[{v2, v1}] = (int)i;

		connectionIndexMap[v1][v2] = (int)i;
		connectionIndexMap[v2][v1] = (int)i;
	}

	const int START_VIRTUAL = -1;
	const int END_VIRTUAL = -2;

	// 添加起点到节点的虚拟道路到图
	for (const auto& [connection, pos] : startRoads) {
		int v1 = connection.GetV1();
		int v2 = connection.GetV2();

		float dist_v1 = pos * connection.distance();
		float dist_v2 = (1.0f - pos) * connection.distance();

		graph[START_VIRTUAL].push_back({ v1, dist_v1 });
		graph[START_VIRTUAL].push_back({ v2, dist_v2 });
		graph[v1].push_back({ START_VIRTUAL, dist_v1 });
		graph[v2].push_back({ START_VIRTUAL, dist_v2 });
	}

	// 添加终点到节点的虚拟道路到图
	for (const auto& [connection, pos] : endRoads) {
		int v1 = connection.GetV1();
		int v2 = connection.GetV2();

		float dist_v1 = pos * connection.distance();
		float dist_v2 = (1.0f - pos) * connection.distance();

		graph[v1].push_back({ END_VIRTUAL, dist_v1 });
		graph[v2].push_back({ END_VIRTUAL, dist_v2 });
		graph[END_VIRTUAL].push_back({ v1, dist_v1 });
		graph[END_VIRTUAL].push_back({ v2, dist_v2 });
	}

	// Dijkstra算法
	unordered_map<int, float> dist;
	unordered_map<int, int> prev;
	unordered_map<int, int> prevConnection;
	for (const auto& node : graph) {
		dist[node.first] = numeric_limits<float>::max();
	}
	dist[START_VIRTUAL] = 0.0f;

	using QueueItem = pair<float, int>;
	priority_queue<QueueItem, vector<QueueItem>, greater<QueueItem>> pq;
	pq.push({ 0.0f, START_VIRTUAL });

	while (!pq.empty()) {
		auto [currentDist, currentNode] = pq.top();
		pq.pop();

		if (currentDist > dist[currentNode]) {
			continue;
		}

		if (currentNode == END_VIRTUAL) break;

		for (const auto& neighbor : graph[currentNode]) {
			int neighborId = neighbor.first;
			float weight = neighbor.second;

			float newDist = currentDist + weight;

			if (newDist < dist[neighborId]) {
				dist[neighborId] = newDist;
				prev[neighborId] = currentNode;

				if (currentNode != START_VIRTUAL && neighborId != END_VIRTUAL) {
					auto it = connectionIndexMap[currentNode].find(neighborId);
					if (it != connectionIndexMap[currentNode].end()) {
						prevConnection[neighborId] = it->second;
					}
					else {
						debugf("Warning: Missing connection index for (%d, %d).\n", currentNode, neighborId);
					}
				}

				pq.push({ newDist, neighborId });
			}
		}
	}
	if (dist[END_VIRTUAL] == numeric_limits<float>::max()) return {};

	// 提取路线
	vector<int> nodePath;
	int current = END_VIRTUAL;
	while (current != START_VIRTUAL) {
		nodePath.push_back(current);
		current = prev[current];
	}
	nodePath.push_back(START_VIRTUAL);
	reverse(nodePath.begin(), nodePath.end());

	if (nodePath.size() < 3) {
		THROW_EXCEPTION(RuntimeException, "Auto navigation failed becuase path is too short.\n");
	}

	vector<Connection> path;
	int firstRealNode = nodePath[1];
	bool foundStart = false;
	for (const auto& [connection, pos] : startRoads) {
		if (connection.GetV1() == firstRealNode || connection.GetV2() == firstRealNode) {
			float begin = pos;
			float end = (connection.GetV1() == firstRealNode) ? 0.0f : 1.0f;

			path.push_back(Connection(connection.GetName(), connection.GetRoadnet(),
				connection.GetV1(), connection.GetV2(), begin, end));
			foundStart = true;
			break;
		}
	}
	if (!foundStart) {
		debugf("Warning: Start road not found for node %d.\n", firstRealNode);
	}

	for (size_t i = 2; i < nodePath.size() - 1; i++) {
		int from = nodePath[i - 1];
		int to = nodePath[i];

		auto it = connectionMap.find({ from, to });
		if (it != connectionMap.end()) {
			int connIdx = it->second;
			if (connIdx >= 0 && connIdx < (int)connections.size()) {
				const Connection& original = connections[connIdx];
				path.push_back(Connection(original.GetName(), original.GetRoadnet(),
					original.GetV1(), original.GetV2(), 0.0f, 1.0f));
			}
			else {
				THROW_EXCEPTION(RuntimeException, "Auto navigation failed becuase of internal index error.\n");
			}
		}
		else {
			debugf("Warning: Missing connection between %d and %d.\n", from, to);
		}
	}
	int lastRealNode = nodePath[nodePath.size() - 2];
	bool foundEnd = false;
	for (const auto& [connection, pos] : endRoads) {
		if (connection.GetV1() == lastRealNode || connection.GetV2() == lastRealNode) {
			float begin = (connection.GetV1() == lastRealNode) ? 0.0f : 1.0f;
			float end = pos;

			path.push_back(Connection(connection.GetName(), connection.GetRoadnet(),
				connection.GetV1(), connection.GetV2(), begin, end));
			foundEnd = true;
			break;
		}
	}
	if (!foundEnd) {
		debugf("Warning: End road not found for node %d.\n", lastRealNode);
	}

	return path;
}

// 自动寻路
const vector<Connection> Roadnet::AutoNavigate(const Plot* startPlot, const Plot* endPlot) const {
	if (!startPlot || !endPlot) {
		debugf("Warning: Null plot pointer in AutoNavigate.\n");
		return {};
	}
	if (startPlot == endPlot) return {};

	auto startRoads = startPlot->GetRoads();
	auto endRoads = endPlot->GetRoads();

	return AutoNavigate(startRoads, endRoads);
}

// 自动寻路
const vector<Connection> Roadnet::AutoNavigate(
	const vector<pair<Connection, float>>& startRoads, const Plot* endPlot) const {
	if (!endPlot) {
		debugf("Warning: Null endPlot in AutoNavigate.\n");
		return {};
	}

	auto endRoads = endPlot->GetRoads();
	return AutoNavigate(startRoads, endRoads);
}

// 自动寻路
const vector<Connection> Roadnet::AutoNavigate(
	const Plot* startPlot, const vector<pair<Connection, float>>& endRoads) const {
	if (!startPlot) {
		debugf("Warning: Null startPlot in AutoNavigate.\n");
		return {};
	}

	auto startRoads = startPlot->GetRoads();
	return AutoNavigate(startRoads, endRoads);
}

// 注册路网
void RoadnetFactory::RegisterRoadnet(const string& id,
    function<Roadnet* ()> creator, function<void(Roadnet*)> deleter) {
    registries[id] = { creator, deleter };
}

// 创建路网
Roadnet* RoadnetFactory::CreateRoadnet(const string& id) const {
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Roadnet %s not enabled when creating.\n", id.data());
        return nullptr;
    }

    auto it = registries.find(id);
    if (it == registries.end()) {
        debugf("Warning: Roadnet %s not registered when creating.\n", id.data());
        return nullptr;
    }

    if (it->second.first) {
        return it->second.first();
    } else {
        THROW_EXCEPTION(NullPointerException, "Roadnet " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool RoadnetFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void RoadnetFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 获取启用路网
Roadnet* RoadnetFactory::GetRoadnet() const {
    for (const auto& [name, enabled] : configs) {
        if (enabled) {
			return CreateRoadnet(name);
        }
    }
    return nullptr;
}

// 析构路网
void RoadnetFactory::DestroyRoadnet(Roadnet* roadnet) const {
    if (!roadnet) {
        debugf("Warning: Roadnet is null when deleting.\n");
        return;
    }

    auto it = registries.find(roadnet->GetType());
    if (it == registries.end()) {
        debugf("Warning: Roadnet %s not registered when deleting.\n", roadnet->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(roadnet);
    } else {
        THROW_EXCEPTION(NullPointerException, "Roadnet " + roadnet->GetType() + " deleter is null.\n");
    }
}

