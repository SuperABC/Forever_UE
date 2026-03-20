#include "terrain.h"


using namespace std;

EmptyTerrain::EmptyTerrain() {

}

EmptyTerrain::~EmptyTerrain() {

}

string EmptyTerrain::GetId() {
	return "empty";
}

string EmptyTerrain::GetType() const {
	return "empty";
}

string EmptyTerrain::GetName() const {
	return "空地形";
}

float EmptyTerrain::GetPriority() const {
	return 0.0f;
}

void EmptyTerrain::DistributeTerrain(int width, int height,
	function<bool(int, int, string, float)> setElement,
	function<string(int, int)> getTerrain, function<float(int, int)> getHeight) const {

}

Terrain::Terrain(TerrainFactory* factory, string terrain) :
	mod(factory->CreateTerrain(terrain)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()) {

}

Terrain::~Terrain() {
	factory->DestroyTerrain(mod);
}

float Terrain::GetPriority() const {
	return mod->GetPriority();
}

void Terrain::DistributeTerrain(int width, int height,
	function<bool(int, int, string, float)> setElement,
	function<string(int, int)> getTerrain, function<float(int, int)> getHeight) const {
	return mod->DistributeTerrain(width, height, setElement, getTerrain, getHeight);
}

