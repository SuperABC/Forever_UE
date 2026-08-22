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

void ResidentialComponent::InitComponent() {

}

int ShopComponent::count = 0;

ShopComponent::ShopComponent() : id(count++) {

}

ShopComponent::~ShopComponent() {

}

const char* ShopComponent::GetId() {
	return "shop";
}

const char* ShopComponent::GetType() const {
	return "shop";
}

const char* ShopComponent::GetName() {
	name = "商店组合" + to_string(id);
	return name.data();
}

void ShopComponent::InitComponent() {

}

int FactoryComponent::count = 0;

FactoryComponent::FactoryComponent() : id(count++) {

}

FactoryComponent::~FactoryComponent() {

}

const char* FactoryComponent::GetId() {
	return "factory";
}

const char* FactoryComponent::GetType() const {
	return "factory";
}

const char* FactoryComponent::GetName() {
	name = "工厂组合" + to_string(id);
	return name.data();
}

void FactoryComponent::InitComponent() {

}

int HotelComponent::count = 0;

HotelComponent::HotelComponent() : id(count++) {

}

HotelComponent::~HotelComponent() {

}

const char* HotelComponent::GetId() {
	return "hotel";
}

const char* HotelComponent::GetType() const {
	return "hotel";
}

const char* HotelComponent::GetName() {
	name = "酒店组合" + to_string(id);
	return name.data();
}

void HotelComponent::InitComponent() {

}

int HospitalComponent::count = 0;

HospitalComponent::HospitalComponent() : id(count++) {

}

HospitalComponent::~HospitalComponent() {

}

const char* HospitalComponent::GetId() {
	return "hospital";
}

const char* HospitalComponent::GetType() const {
	return "hospital";
}

const char* HospitalComponent::GetName() {
	name = "医院组合" + to_string(id);
	return name.data();
}

void HospitalComponent::InitComponent() {

}
