#include "manufacture.h"


using namespace std;

// 默认工坊

string DefaultManufacture::GetId() {
	return "default";
}

string DefaultManufacture::GetType() const {
	return "default";
}

string DefaultManufacture::GetName() const {
	return "默认生产线";
}

unordered_map<string, float> DefaultManufacture::ProductTargets() const {
	return { {"default", 1.f} };
}
