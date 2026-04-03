#include "product_mod.h"


using namespace std;

ProductMod::ProductMod() :
	catagories(),
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
	for (auto& config : configs) {
		config.second = false;
	}
}

ProductMod* ProductFactory::CreateProduct(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Product %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Product %s not registered when creating.\n", id.data());
		return nullptr;
	}

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

void ProductFactory::DestroyProduct(ProductMod* Product) const {
	if (!Product) {
		debugf("Warning: Product is null when deleting.\n");
		return;
	}

	auto it = registries.find(Product->GetType());
	if (it == registries.end()) {
		debugf("Warning: Product %s not registered when deleting.\n", Product->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(Product);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Product " + string(Product->GetType()) + " deleter is null.\n");
	}
}

