#include "storage_basic.h"


using namespace std;

int ShopStorage::count = 0;

ShopStorage::ShopStorage() : id(count++) {

}

ShopStorage::~ShopStorage() {

}

const char* ShopStorage::GetId() {
	return "shop";
}

const char* ShopStorage::GetType() const {
	return "shop";
}

const char* ShopStorage::GetName() {
	name = "商店仓库" + to_string(id);
	return name.data();
}

void ShopStorage::SetProperty() {
	catagories = { "empty" };
	density = 1.f;
	accept = true;
	provide = false;
}

