#include "terrain_basic.h"


using namespace std;

// 海洋地形

int OceanTerrain::count = 0;

OceanTerrain::OceanTerrain() : id(count++) {

}

OceanTerrain::~OceanTerrain() {

}

const char* OceanTerrain::GetId() {
	return "ocean";
}

const char* OceanTerrain::GetType() const {
	return "ocean";
}

const char* OceanTerrain::GetName() {
	name = "海洋地形" + to_string(id);
	return name.data();
}

float OceanTerrain::GetPriority() const {
	return 1.0f;
}

void OceanTerrain::DistributeTerrain(int width, int height,
	function<bool(int, int, string, float)> setElement,
	function<string(int, int)> getTerrain, function<float(int, int)> getHeight) const {

}

// 山区地形

int MountainTerrain::count = 0;

MountainTerrain::MountainTerrain() : id(count++) {

}

MountainTerrain::~MountainTerrain() {

}

const char* MountainTerrain::GetId() {
	return "mountain";
}

const char* MountainTerrain::GetType() const {
	return "mountain";
}

const char* MountainTerrain::GetName() {
	name = "山区地形" + to_string(id);
	return name.data();
}

float MountainTerrain::GetPriority() const {
	return 0.9f;
}

void MountainTerrain::DistributeTerrain(int width, int height,
	function<bool(int, int, string, float)> setElement,
	function<string(int, int)> getTerrain, function<float(int, int)> getHeight) const {

}
