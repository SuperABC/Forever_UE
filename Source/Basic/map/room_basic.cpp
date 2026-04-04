#include "room_basic.h"


using namespace std;

int ResidentialRoom::count = 0;

ResidentialRoom::ResidentialRoom() : id(count++) {

}

ResidentialRoom::~ResidentialRoom() {

}

const char* ResidentialRoom::GetId() {
	return "residential";
}

const char* ResidentialRoom::GetType() const {
	return "residential";
}

const char* ResidentialRoom::GetName() {
	name = "住宅房间" + to_string(id);
	return name.data();
}

void ResidentialRoom::ConfigRoom() {
	isResidential = true;
	residentialCapacity = 1;
}

int ShopRoom::count = 0;

ShopRoom::ShopRoom() : id(count++) {

}

ShopRoom::~ShopRoom() {

}

const char* ShopRoom::GetId() {
	return "shop";
}

const char* ShopRoom::GetType() const {
	return "shop";
}

const char* ShopRoom::GetName() {
	name = "商店房间" + to_string(id);
	return name.data();
}

void ShopRoom::ConfigRoom() {
	isWorkspace = true;
	workspaceCapacity = 100;
}

int WarehouseRoom::count = 0;

WarehouseRoom::WarehouseRoom() : id(count++) {

}

WarehouseRoom::~WarehouseRoom() {

}

const char* WarehouseRoom::GetId() {
	return "warehouse";
}

const char* WarehouseRoom::GetType() const {
	return "warehouse";
}

const char* WarehouseRoom::GetName() {
	name = "仓库房间" + to_string(id);
	return name.data();
}

void WarehouseRoom::ConfigRoom() {
	isStorage = true;
	storageConfig = { {"empty", 100.f} };
}

int FactoryRoom::count = 0;

FactoryRoom::FactoryRoom() : id(count++) {

}

FactoryRoom::~FactoryRoom() {

}

const char* FactoryRoom::GetId() {
	return "factory";
}

const char* FactoryRoom::GetType() const {
	return "factory";
}

const char* FactoryRoom::GetName() {
	name = "工厂房间" + to_string(id);
	return name.data();
}

void FactoryRoom::ConfigRoom() {
	isManufacture = true;
	manufactureTypes = { "experience" };
}
