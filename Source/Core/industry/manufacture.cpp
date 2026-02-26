#include "manufacture.h"


using namespace std;

string DefaultManufacture::GetId() {
	return "default";
}

string DefaultManufacture::GetType() const {
	return "default";
}

string DefaultManufacture::GetName() const {
	return "默认生产线";
}

std::unordered_map<std::string, float> DefaultManufacture::ProductTargets() const {
	return { {"default", 1.f} };
}
