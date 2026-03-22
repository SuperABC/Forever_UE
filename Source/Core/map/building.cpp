#include "building.h"


using namespace std;

int EmptyBuilding::count = 0;

EmptyBuilding::EmptyBuilding() : id(count++) {

}

EmptyBuilding::~EmptyBuilding() {

}

const char* EmptyBuilding::GetId() {
	return "empty";
}

const char* EmptyBuilding::GetType() const {
	return "empty";
}

const char* EmptyBuilding::GetName() {
	name = "空建筑" + to_string(id);
	return name.data();
}

vector<float> EmptyBuilding::GetPowers() {
	return vector<float>(AREA_END, 0.f);
}

function<int(Lot*)> EmptyBuilding::BuildingAssigner = [](Lot* lot) {
	return 0;
};

float EmptyBuilding::RandomAcreage() {
	return 0.f;
}

void EmptyBuilding::LayoutBuilding() {

}

void EmptyBuilding::PlaceConstruction() {

}

Building::Building(BuildingFactory* factory, string building) :
	mod(factory->CreateBuilding(building)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	parentZone(nullptr),
	parentBlock(nullptr),
	stated(false),
	layers(1),
	basements(0),
	height(0.4f),
	construction() {

}

Building::~Building() {
	factory->DestroyBuilding(mod);
}

string Building::GetType() const {
	return type;
}

string Building::GetName() const {
	return name;
}

Quad* Building::GetQuad() {
	return mod;
}

float Building::RandomAcreage() {
	return mod->RandomAcreage();
}

float Building::GetAcreageMin() {
	return mod->minAcreage;
}

float Building::GetAcreageMax() {
	return mod->maxAcreage;
}

Block* Building::GetParentBlock() const {
	return parentBlock;
}

void Building::SetParent(Block* block) {
	parentBlock = block;
}

Zone* Building::GetParentZone() const {
	return parentZone;
}

void Building::SetParent(Zone* zone) {
	parentZone = zone;
}

//int Building::GetOwner() const {
//	return owner;
//}

//void Building::SetOwner(int owner) {
//	this->owner = owner;
//}

bool Building::GetStated() const {
	return stated;
}

void Building::SetStated(bool stated) {
	this->stated = stated;
}

int Building::GetLayers() const {
	return layers;
}

void Building::SetLayers(int layers) {
	this->layers = layers;
}

int Building::GetBasements() const {
	return basements;
}

void Building::SetBasements(int basements) {
	this->basements = basements;
}

float Building::GetHeight() const {
	return height;
}

void Building::SetHeight(float height) {
	this->height = height;
}

const Quad Building::GetConstruction() const {
	return construction;
}

