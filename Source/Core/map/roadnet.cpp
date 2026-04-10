#include "roadnet.h"

#include "map/block.h"

#include <queue>
#include <limits>
#include <algorithm>

#undef max


using namespace std;

struct pair_hash {
    template <class T1, class T2>
    size_t operator() (const pair<T1, T2>& p) const {
        auto h1 = hash<T1>{}(p.first);
        auto h2 = hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

Roadnet::Roadnet(RoadnetFactory* factory, const string& roadnet) :
	mod(factory->CreateRoadnet(roadnet)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	nodes(),
	connections(),
	blocks(),
	addresses() {

}

Roadnet::~Roadnet() {
	factory->DestroyRoadnet(mod);
	for (auto node : nodes) {
		delete node;
	}
	nodes.clear();
	for (auto connection : connections) {
		delete connection;
	}
	connections.clear();
	for (auto block : blocks) {
		delete block;
	}
	blocks.clear();
}

string Roadnet::GetType() const {
	return type;
}

string Roadnet::GetName() const {
	return name;
}

void Roadnet::DistributeRoadnet(int width, int height,
	function<string(int, int)> get) {
	mod->DistributeRoadnet(width, height, get);
	
	for (auto node : mod->nodes) {
		nodes.push_back(new Node(node));
	}
	for (auto connection : mod->connections) {
		connections.push_back(new Connection(connection));
	}
	for (auto [lot, connections] : mod->lots) {
		auto block = new Block(lot);
		vector<pair<Connection*, float>> roads;
		for (auto connection : connections) {
			roads.emplace_back(new Connection(connection.first), connection.second);
		}
		block->SetRoads(roads);
		blocks.push_back(block);
	}
}

const vector<Node*>& Roadnet::GetNodes() const {
	return nodes;
}

const vector<Connection*>& Roadnet::GetConnections() const {
	return connections;
}

const vector<Block*>& Roadnet::GetBlocks() const {
	return blocks;
}

void Roadnet::AllocateAddress() {
	addresses.clear();

	for (auto& block : blocks) {
		auto roads = block->GetRoads();
		for (const auto& road : roads) {
			addresses[road.first->GetName()].push_back(block);
			block->SetAddress(road.first->GetName(), (int)addresses[road.first->GetName()].size() - 1);
		}
	}
}

Block* Roadnet::LocateBlock(const string& road, int id) const {
	if (addresses.find(road) == addresses.end()) {
		return nullptr;
	}
	auto blocks = addresses.at(road);
	if (id < 0 || id >= (int)blocks.size()) {
		return nullptr;
	}
	return blocks[id];
}

vector<pair<Connection*, pair<float, float>>> Roadnet::AutoNavigate(
    const vector<pair<Connection*, float>>& startRoads,
    const vector<pair<Connection*, float>>& endRoads) const {

    if (startRoads.empty() || endRoads.empty()) {
        debugf("Warning: no start or end roads when navigating.\n");
        return {};
    };

    // 检查是否在同一条道路上
    for (auto& startRoad : startRoads) {
        for (auto& endRoad : endRoads) {
            if (*startRoad.first == *endRoad.first) {
                return { { startRoad.first, { startRoad.second, endRoad.second } } };
            }
        }
    }

    // 构建图结构 - 确保双向边
    unordered_map<int, vector<pair<int, float>>> graph;
    unordered_map<pair<int, int>, Connection*, pair_hash> connectionMap;

    // 添加已有道路到图 - 明确添加双向边
    for (auto* conn : connections) {
        int v1 = conn->GetStart().GetId();
        int v2 = conn->GetEnd().GetId();
        float dist = conn->CalcDistance(0.0f, 1.0f);  // 整条道路的距离

        // 双向边
        graph[v1].push_back({ v2, dist });
        graph[v2].push_back({ v1, dist });

        // 双向映射
        connectionMap[{v1, v2}] = conn;
        connectionMap[{v2, v1}] = conn;
    }

    const int START_VIRTUAL = -1;
    const int END_VIRTUAL = -2;

    // 添加起点到节点的虚拟道路到图 - 双向
    for (const auto& road : startRoads) {
        Connection* connection = road.first;
        float position = road.second;

        int v1 = connection->GetStart().GetId();
        int v2 = connection->GetEnd().GetId();

        // 从起点位置到两个端点的距离
        float dist_v1 = connection->CalcDistance(position, 0.0f);
        float dist_v2 = connection->CalcDistance(position, 1.0f);

        // 双向添加虚拟边
        graph[START_VIRTUAL].push_back({ v1, dist_v1 });
        graph[START_VIRTUAL].push_back({ v2, dist_v2 });
        graph[v1].push_back({ START_VIRTUAL, dist_v1 });
        graph[v2].push_back({ START_VIRTUAL, dist_v2 });
    }

    // 添加终点到节点的虚拟道路到图 - 双向
    for (const auto& road : endRoads) {
        Connection* connection = road.first;
        float position = road.second;

        int v1 = connection->GetStart().GetId();
        int v2 = connection->GetEnd().GetId();

        // 从两个端点到终点位置的距离
        float dist_v1 = connection->CalcDistance(0.0f, position);
        float dist_v2 = connection->CalcDistance(1.0f, position);

        // 双向添加虚拟边
        graph[v1].push_back({ END_VIRTUAL, dist_v1 });
        graph[v2].push_back({ END_VIRTUAL, dist_v2 });
        graph[END_VIRTUAL].push_back({ v1, dist_v1 });
        graph[END_VIRTUAL].push_back({ v2, dist_v2 });
    }

    // Dijkstra算法
    unordered_map<int, float> dist;
    unordered_map<int, int> prev;
    unordered_map<pair<int, int>, Connection*, pair_hash> prevConnection;

    // 初始化所有节点的距离为无穷大
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

        if (currentNode == END_VIRTUAL) {
            break;
        }

        // 遍历所有邻居
        for (const auto& neighbor : graph[currentNode]) {
            int neighborId = neighbor.first;
            float weight = neighbor.second;

            float newDist = currentDist + weight;

            if (newDist < dist[neighborId]) {
                dist[neighborId] = newDist;
                prev[neighborId] = currentNode;

                // 记录连接关系（如果是真实道路连接）
                if (currentNode >= 0 && neighborId >= 0) {  // 两个都是真实节点
                    auto it = connectionMap.find({ currentNode, neighborId });
                    if (it != connectionMap.end()) {
                        prevConnection[{currentNode, neighborId}] = it->second;
                    }
                }

                pq.push({ newDist, neighborId });
            }
        }
    }

    // 检查是否找到路径
    if (dist[END_VIRTUAL] == numeric_limits<float>::max()) {
        debugf("Warning: navigation failed because no connection found in between.\n");
        return {};
    }

    // 提取路线节点路径
    vector<int> nodePath;
    int current = END_VIRTUAL;
    while (current != START_VIRTUAL) {
        nodePath.push_back(current);
        current = prev[current];
    }
    nodePath.push_back(START_VIRTUAL);
    reverse(nodePath.begin(), nodePath.end());

    if (nodePath.size() < 3) {
        debugf("Warning: navigation failed because of node num < 3.\n");
        return {};  // 寻路失败
    }

    // 构建返回的道路路径
    vector<pair<Connection*, pair<float, float>>> path;

    // 处理起点道路
    int firstRealNode = nodePath[1];
    for (const auto& road : startRoads) {
        Connection* connection = road.first;
        int v1 = connection->GetStart().GetId();
        int v2 = connection->GetEnd().GetId();

        if (v1 == firstRealNode || v2 == firstRealNode) {
            float begin = road.second;
            float end = (v1 == firstRealNode) ? 0.0f : 1.0f;
            path.push_back({ connection, { begin, end } });
            break;
        }
    }

    // 处理中间道路
    for (size_t i = 2; i < nodePath.size() - 1; i++) {
        int from = nodePath[i - 1];
        int to = nodePath[i];

        // 查找连接这两个节点的道路
        auto it = connectionMap.find({ from, to });
        if (it != connectionMap.end()) {
            Connection* conn = it->second;
            path.push_back({ conn, { 0.0f, 1.0f } });
        }
    }

    // 处理终点道路
    int lastRealNode = nodePath[nodePath.size() - 2];
    for (const auto& road : endRoads) {
        Connection* connection = road.first;
        int v1 = connection->GetStart().GetId();
        int v2 = connection->GetEnd().GetId();

        if (v1 == lastRealNode || v2 == lastRealNode) {
            float begin = (v1 == lastRealNode) ? 0.0f : 1.0f;
            float end = road.second;
            path.push_back({ connection, { begin, end } });
            break;
        }
    }

    return path;
}

vector<pair<Connection*, pair<float, float>>> Roadnet::AutoNavigate(
    Block* startBlock, Block* endBlock) const {
    if (!startBlock || !endBlock) return {};
    if (startBlock == endBlock) return {};

    auto startRoads = startBlock->GetRoads();
    auto endRoads = endBlock->GetRoads();

    return AutoNavigate(startRoads, endRoads);
}

vector<pair<Connection*, pair<float, float>>> Roadnet::AutoNavigate(
    const vector<pair<Connection*, float>>& startRoads, Block* endBlock) const {
    if (!endBlock) return {};

    auto endRoads = endBlock->GetRoads();
    return AutoNavigate(startRoads, endRoads);
}

vector<pair<Connection*, pair<float, float>>> Roadnet::AutoNavigate(
    Block* startBlock, const vector<pair<Connection*, float>>& endRoads) const {
    if (!startBlock) return {};

    auto startRoads = startBlock->GetRoads();
    return AutoNavigate(startRoads, endRoads);
}

int EmptyRoadnet::count = 0;

EmptyRoadnet::EmptyRoadnet() : id(count++) {

}

EmptyRoadnet::~EmptyRoadnet() {

}

const char* EmptyRoadnet::GetId() {
	return "empty";
}

const char* EmptyRoadnet::GetType() const {
	return "empty";
}

const char* EmptyRoadnet::GetName() {
	name = "空路网" + to_string(id);
	return name.data();
}

void EmptyRoadnet::DistributeRoadnet(int width, int height,
	function<string(int, int)> get) {

}

