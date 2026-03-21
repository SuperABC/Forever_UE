#include "roadnet.h"


using namespace std;

EmptyRoadnet::EmptyRoadnet() {

}

EmptyRoadnet::~EmptyRoadnet() {

}

string EmptyRoadnet::GetId() {
	return "empty";
}

string EmptyRoadnet::GetType() const {
	return "empty";
}

string EmptyRoadnet::GetName() const {
	return "空路网";
}

void EmptyRoadnet::DistributeRoadnet(int width, int height,
	function<string(int, int)> get) {

}

Roadnet::Roadnet(RoadnetFactory* factory, string roadnet) :
	mod(factory->CreateRoadnet(roadnet)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()) {

}

Roadnet::~Roadnet() {
	factory->DestroyRoadnet(mod);
	for (auto node : nodes) {
		delete node;
	}
	for (auto connection : connections) {
		delete connection;
	}
	for (auto plot : plots) {
		delete plot;
	}
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
	for (auto [lot, connections] : mod->quads) {
		auto plot = new Plot(lot);
		vector<pair<Connection*, float>> roads;
		for (auto connection : connections) {
			roads.emplace_back(new Connection(connection.first), connection.second);
		}
		plot->SetRoads(roads);
		plots.push_back(plot);
	}
}

const vector<Node*>& Roadnet::GetNodes() const {
	return nodes;
}

const vector<Connection*>& Roadnet::GetConnections() const {
	return connections;
}

const vector<Plot*>& Roadnet::GetPlots() const {
	return plots;
}

void Roadnet::AllocateAddress() {
	addresses.clear();

	for (auto& plot : plots) {
		auto roads = plot->GetRoads();
		for (const auto& road : roads) {
			addresses[road.first->GetName()].push_back(plot);
			plot->SetAddress(road.first->GetName(), (int)addresses[road.first->GetName()].size());
		}
	}
}

// 寻址地块
Plot* Roadnet::LocatePlot(string road, int id) const {
	if (addresses.find(road) == addresses.end()) {
		return nullptr;
	}
	auto plots = addresses.at(road);
	if (id < 0 || id >= (int)plots.size()) {
		return nullptr;
	}
	return plots[id];
}

