#include "product_mod.h"


using namespace std;

ProductMod::ProductMod() :
	categories(),
	price(0.f),
	ingredients(),
	byproducts() {

}

ProductMod::~ProductMod() {

}

void ProductFactory::RegisterProduct(const string& id,
	function<ProductMod* ()> creator, function<void(ProductMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void ProductFactory::RemoveAll() {
	for (auto& [_, enabled] : configs) {
		enabled = false;
	}
}

ProductMod* ProductFactory::CreateProduct(const string& id) const {
	// 检查产品是否已启用
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Product %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	// 检查产品是否已注册
	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Product %s not registered when creating.\n", id.data());
		return nullptr;
	}

	// 调用注册的创建函数
	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Product " + id + " creater is null.\n");
	}

	return nullptr;
}

bool ProductFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void ProductFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

void ProductFactory::DestroyProduct(ProductMod* productMod) const {
	// 检查模组是否为空
	if (!productMod) {
		debugf("Warning: Product is null when deleting.\n");
		return;
	}

	// 检查产品是否已注册
	auto it = registries.find(productMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: Product %s not registered when deleting.\n", productMod->GetType());
		return;
	}

	// 调用注册的析构函数
	if (it->second.second) {
		it->second.second(productMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Product " + string(productMod->GetType()) + " deleter is null.\n");
	}
}

