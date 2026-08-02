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
	const function<string(int, int)>& getTerrain,
	const function<float(int, int)>& getHeight,
	const function<pair<bool, float>(int, int)>& getWater,
	const function<void(Quad, float)>& addHatch, int nodeStaticCount) {
	mod->DistributeRoadnet(width, height, getTerrain, getHeight, getWater, nodeStaticCount);

	for(auto& ext : mod->externs) {
		externs.push_back(new Node(ext));
	}
	for (auto& [quad, rotation] : mod->hatches) {
		addHatch(quad, rotation);
	}
	for (auto& intersection : mod->intersections) {
		intersections.push_back(new Intersection(intersection));
	}
	for (auto& road : mod->roads) {
		roads.push_back(new Road(road));
	}
	for (auto& [lot, lotData] : mod->lots) {
		auto block = new Block(lot);
		auto& [lotRoads, lotIntersections] = lotData;

		vector<Road*> blockRoads(4, nullptr);
		if(lotRoads.find(0) != lotRoads.end()) blockRoads[0] = new Road(lotRoads.at(0));
		if(lotRoads.find(1) != lotRoads.end()) blockRoads[1] = new Road(lotRoads.at(1));
		if(lotRoads.find(2) != lotRoads.end()) blockRoads[2] = new Road(lotRoads.at(2));
		if(lotRoads.find(3) != lotRoads.end()) blockRoads[3] = new Road(lotRoads.at(3));
		block->SetRoads(blockRoads);

		vector<Intersection*> blockIntersections(4, nullptr);
		if (lotIntersections.find(0) != lotIntersections.end()) blockIntersections[0] = new Intersection(lotIntersections.at(0));
		if (lotIntersections.find(1) != lotIntersections.end()) blockIntersections[1] = new Intersection(lotIntersections.at(1));
		if (lotIntersections.find(2) != lotIntersections.end()) blockIntersections[2] = new Intersection(lotIntersections.at(2));
		if (lotIntersections.find(3) != lotIntersections.end()) blockIntersections[3] = new Intersection(lotIntersections.at(3));
		block->SetIntersections(blockIntersections);

		blocks.push_back(block);
	}
}

const vector<Node*>& Roadnet::GetExterns() const {
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
		for (auto road : block->GetRoads()) {
			if (!road) continue;
			addresses[road->GetName()].push_back(block);
			block->SetAddress(road->GetName(), static_cast<int>(addresses[road->GetName()].size()) - 1);
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
	const function<string(int, int)>& getTerrain,
	const function<float(int, int)>& getHeight,
	const function<pair<bool, float>(int, int)>& getWater, int nodeStaticCount) {
	Node::SetCount(nodeStaticCount);
}
