#include "terrain_basic.h"


using namespace std;

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
	function<string(int, int)> getTerrain, function<bool(int, int, string)> setTerrain,
	function<float(int, int)> getHeight, function<bool(int, int, float)> setHeight) const {

	// 随机生成四个方向是否临海，并保证至少有一面临海
	int distribute = 0;
	while (distribute == 0) {
		distribute = GetRandom(16);
	}

	// 处理西面海面区域
	if (distribute & 1) {
		int distance = width / 16 + GetRandom(width / 16);
		debugf("Log: Sea in the west %d.\n", distance);
		float shift = 0.0f, slope = 0.0f;
		for (int y = 0; y < height; y++) {
			int limit = distance + (int)shift;
			if (limit > width)
				limit = width;
			for (int x = 0; x < limit; x++) {
				setTerrain(x, y, GetType());
				setHeight(x, y, 0.f);
			}
			slope += float(GetRandom(33) - 16) / 256.0f;
			shift += slope;
			if (distance + shift <= width / 64) {
				slope = 0.0f;
				shift = width / 64 - (float)distance;
			}
			if (distance + shift >= width / 8) {
				slope = 0.0f;
				shift = width / 8 - (float)distance;
			}
		}
	}

	// 处理东面海面区域
	if (distribute & 2) {
		int distance = width / 16 + GetRandom(width / 16);
		debugf("Log: Sea in the east %d.\n", distance);
		float shift = 0.0f, slope = 0.0f;
		for (int y = 0; y < height; y++) {
			int start = width - distance - (int)shift;
			if (start < 0)
				start = 0;
			for (int x = width - 1; x >= start; x--) {
				setTerrain(x, y, GetType());
				setHeight(x, y, 0.f);
			}
			slope += float(GetRandom(33) - 16) / 256.0f;
			shift += slope;
			if (distance + shift <= width / 64) {
				slope = 0.0f;
				shift = width / 64.f - distance;
			}
			if (distance + shift >= width / 8) {
				slope = 0.0f;
				shift = width / 8.f - distance;
			}
		}
	}

	// 处理北面海面区域
	if (distribute & 4) {
		int distance = height / 16 + GetRandom(height / 16);
		debugf("Log: Sea in the north %d.\n", distance);
		float shift = 0.0f, slope = 0.0f;
		for (int x = 0; x < width; x++) {
			int limit = distance + (int)shift;
			if (limit > height)
				limit = height;
			for (int y = 0; y < limit; y++) {
				setTerrain(x, y, GetType());
				setHeight(x, y, 0.f);
			}
			slope += float(GetRandom(33) - 16) / 256.0f;
			shift += slope;
			if (distance + shift <= height / 64) {
				slope = 0.0f;
				shift = height / 64.f - distance;
			}
			if (distance + shift >= height / 8) {
				slope = 0.0f;
				shift = height / 8.f - distance;
			}
		}
	}

	// 处理南面海面区域
	if (distribute & 8) {
		int distance = height / 16 + GetRandom(height / 16);
		debugf("Log: Sea in the south %d.\n", distance);
		float shift = 0.0f, slope = 0.0f;
		for (int x = 0; x < width; x++) {
			int start = height - distance - (int)shift;
			if (start < 0)
				start = 0;
			for (int y = height - 1; y >= start; y--) {
				setTerrain(x, y, GetType());
				setHeight(x, y, 0.f);
			}
			slope += float(GetRandom(33) - 16) / 256.0f;
			shift += slope;
			if (distance + shift <= height / 64) {
				slope = 0.0f;
				shift = height / 64.f - distance;
			}
			if (distance + shift >= height / 8) {
				slope = 0.0f;
				shift = height / 8.f - distance;
			}
		}
	}
}

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
	function<string(int, int)> getTerrain, function<bool(int, int, string)> setTerrain,
	function<float(int, int)> getHeight, function<bool(int, int, float)> setHeight) const {

	int scalar = width * height / (512 * 512);
	int num = scalar > 1 ? (4 + GetRandom(scalar * 2)) : 0;

	Counter counter(100);
	int mountain = 0;
	for (int i = 0; i < num; i++) {
		int x = width / 8 + GetRandom(width * 3 / 4);
		int y = height / 8 + GetRandom(height * 3 / 4);

		if (getTerrain(x, y) != "plain" ||
			sqrt(pow(x - width / 2, 2) + pow(y - height / 2, 2)) < 128) {
			if (!counter.count())
				i--;
			continue;
		}
		int area = (int)(64 * 64 +
			scalar * 0.2f *
			((GetRandom(4) ? 0 : 1) * GetRandom(512 * 512)) +
			GetRandom(128 * 128));
		FloodTerrain(x, y, area, false, width, height, getTerrain, setTerrain);
		mountain++;
	}
	for (int i = 2; i < width - 2; i++) {
		for (int j = 2; j < height - 2; j++) {
			ShapeFilter(i, j, width, height, getTerrain, setTerrain, 2, 0.5f);
		}
	}
	debugf("Log: Generate mountain %d.\n", mountain);
}
