#include "component.h"


using namespace std;

// 默认住宅组合

DefaultResidentialComponent::DefaultResidentialComponent() : Component() {

}

DefaultResidentialComponent::~DefaultResidentialComponent() {

}

string DefaultResidentialComponent::GetId() {
	return "default_residential";
}

string DefaultResidentialComponent::GetType() const {
	return "default_residential";
}

string DefaultResidentialComponent::GetName() const {
	return "默认住宅组合";
}

// 默认工作组合

DefaultWorkingComponent::DefaultWorkingComponent() : Component() {

}

DefaultWorkingComponent::~DefaultWorkingComponent() {

}

string DefaultWorkingComponent::GetId() {
	return "default_working";
}

string DefaultWorkingComponent::GetType() const {
	return "default_working";
}

string DefaultWorkingComponent::GetName() const {
	return "默认工作组合";
}