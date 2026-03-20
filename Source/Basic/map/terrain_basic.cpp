#include "terrain_basic.h"


using namespace std;

// 海洋地形

OceanTerrain::OceanTerrain() {

}

OceanTerrain::~OceanTerrain() {

}

string OceanTerrain::GetId() {
    return "ocean";
}

string OceanTerrain::GetType() const {
    return "ocean";
}

string OceanTerrain::GetName() const {
    return "海洋地形";
}

float OceanTerrain::GetPriority() const {
    return 1.0f;
}

void OceanTerrain::DistributeTerrain(int width, int height,
    function<bool(int, int, string, float)> setElement,
    function<string(int, int)> getTerrain, function<float(int, int)> getHeight) const {

}

// 山区地形

MountainTerrain::MountainTerrain() {

}

MountainTerrain::~MountainTerrain() {

}

string MountainTerrain::GetId() {
    return "mountain";
}

string MountainTerrain::GetType() const {
    return "mountain";
}

string MountainTerrain::GetName() const {
    return "山区地形";
}

float MountainTerrain::GetPriority() const {
    return 0.9f;
}

void MountainTerrain::DistributeTerrain(int width, int height,
    function<bool(int, int, string, float)> setElement,
    function<string(int, int)> getTerrain, function<float(int, int)> getHeight) const {

}
