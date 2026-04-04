#include "manufacture_basic.h"


using namespace std;

int ExperienceManufacture::count = 0;

ExperienceManufacture::ExperienceManufacture() : id(count++) {

}

ExperienceManufacture::~ExperienceManufacture() {

}

const char* ExperienceManufacture::GetId() {
	return "experience";
}

const char* ExperienceManufacture::GetType() const {
	return "experience";
}

const char* ExperienceManufacture::GetName() {
	name = "经验工坊" + to_string(id);
	return name.data();
}

void ExperienceManufacture::SetTargets() {
	targets["targets"] = 1.f;
}

