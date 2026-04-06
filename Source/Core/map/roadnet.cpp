#include "roadnet.h"

#include "map/block.h"


using namespace std;

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
			block->SetAddress(road.first->GetName(), (int)addresses[road.first->GetName()].size());
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

