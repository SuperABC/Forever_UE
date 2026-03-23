#include "component_basic.h"


using namespace std;

int ResidentialComponent::count = 0;

ResidentialComponent::ResidentialComponent() : id(count++) {

}

ResidentialComponent::~ResidentialComponent() {

}

const char* ResidentialComponent::GetId() {
	return "residential";
}

const char* ResidentialComponent::GetType() const {
	return "residential";
}

const char* ResidentialComponent::GetName() {
	name = "住宅组合" + to_string(id);
	return name.data();
}
