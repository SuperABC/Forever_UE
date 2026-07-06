#include "roadnet.h"

#include "map/block.h"

#include <algorithm>


using namespace std;

Roadnet::Roadnet(RoadnetFactory* factory, const string& roadnet) :
	mod(factory->CreateRoadnet(roadnet)),
	factory(factory),
	type(),
	name(),
	externs(),
	intersections(),
	roads(),
	blocks(),
	addresses() {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Roadnet " + roadnet + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Roadnet::~Roadnet() {
	factory->DestroyRoadnet(mod);
	for(auto ext : externs) {
		delete ext;
	}
	externs.clear();
	for (auto intersection : intersections) {
		delete intersection;
	}
	intersections.clear();
	for (auto road : roads) {
		delete road;
	}
	roads.clear();
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
	const function<string(int, int)>& get) {
	mod->DistributeRoadnet(width, height, get);

	for(auto& ext : mod->externs) {
		externs.push_back(new Node(ext));
	}
	for (auto& intersection : mod->intersections) {
		intersections.push_back(new Intersection(intersection));
	}
	for (auto& road : mod->roads) {
		roads.push_back(new Road(road));
	}
	for (auto& [lot, lotRoads] : mod->lots) {
		auto block = new Block(lot);
		vector<pair<Road*, float>> blockRoads;
		for (auto& [road, position] : lotRoads) {
			blockRoads.emplace_back(new Road(road), position);
		}
		block->SetRoads(blockRoads);
		blocks.push_back(block);
	}
}

const std::vector<Node*>& Roadnet::GetExterns() const {
	return externs;
}

const vector<Intersection*>& Roadnet::GetIntersections() const {
	return intersections;
}

const vector<Road*>& Roadnet::GetRoads() const {
	return roads;
}

const vector<Block*>& Roadnet::GetBlocks() const {
	return blocks;
}

void Roadnet::AllocateAddress() {
	addresses.clear();

	for (auto& block : blocks) {
		auto blockRoads = block->GetRoads();
		for (const auto& road : blockRoads) {
			addresses[road.first->GetName()].push_back(block);
			block->SetAddress(road.first->GetName(), static_cast<int>(addresses[road.first->GetName()].size()) - 1);
		}
	}
}

Block* Roadnet::LocateBlock(const string& road, int id) const {
	if (addresses.find(road) == addresses.end()) {
		return nullptr;
	}
	auto blocks = addresses.at(road);
	if (id < 0 || id >= static_cast<int>(blocks.size())) {
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
	const function<string(int, int)>& get) {

}
