#include "terrain.h"


using namespace std;

Terrain::Terrain(TerrainFactory* factory, const string& terrain) :
	mod(factory->CreateTerrain(terrain)),
	factory(factory),
	type(),
	name() {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Terrain " + terrain + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Terrain::~Terrain() {
	factory->DestroyTerrain(mod);
}

string Terrain::GetType() const {
	return type;
}

string Terrain::GetName() const {
	return name;
}

float Terrain::GetPriority() const {
	return mod->GetPriority();
}

void Terrain::SetupTexture() const {
	mod->SetupTexture();
}

string Terrain::GetTexture() const {
	return mod->diffusePath;
}

pair<bool, float> Terrain::GetWater() const {
	return mod->waterHeight;
}

void Terrain::DistributeTerrain(int width, int height,
	const function<string(int, int)>& getTerrain, const function<bool(int, int, string)>& setTerrain,
	const function<float(int, int)>& getHeight, const function<bool(int, int, float)>& setHeight) const {
	return mod->DistributeTerrain(width, height, getTerrain, setTerrain, getHeight, setHeight);
}

int EmptyTerrain::count = 0;

EmptyTerrain::EmptyTerrain() : id(count++) {

}

EmptyTerrain::~EmptyTerrain() {

}

const char* EmptyTerrain::GetId() {
	return "empty";
}

const char* EmptyTerrain::GetType() const {
	return "empty";
}

const char* EmptyTerrain::GetName() {
	name = "空地形" + to_string(id);
	return name.data();
}

float EmptyTerrain::GetPriority() const {
	return 0.0f;
}

void EmptyTerrain::SetupTexture() {

}

void EmptyTerrain::DistributeTerrain(int width, int height,
	const function<string(int, int)>& getTerrain, const function<bool(int, int, string)>& setTerrain,
	const function<float(int, int)>& getHeight, const function<bool(int, int, float)>& setHeight) const {

}

