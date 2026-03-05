#include "product_base.h"
#include "../common/error.h"

using namespace std;

void Product::SetAmount(float amount) {
    this->amount = amount;
}

void Product::IncreaseAmount(float amount) {
    this->amount += amount;
}

void Product::DecreaseAmount(float amount) {
    this->amount -= amount;
}

float Product::GetAmount() const {
    return amount;
}

void Product::SetPrice(float price) {
    this->price = price;
}

float Product::GetPriceValue() const {
    return price;
}

void ProductFactory::RegisterProduct(const string& id,
    function<Product*()> creator, function<void(Product*)> deleter) {
    registries[id] = {creator, deleter};
}

Product* ProductFactory::CreateProduct(const string& id) {
    if (configs.find(id) == configs.end() || !configs.find(id)->second)
        return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first();
    }
    return nullptr;
}

bool ProductFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void ProductFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}

void ProductFactory::DestroyProduct(Product* product) const {
    if (!product) return;
    auto it = registries.find(product->GetType());
    if (it != registries.end()) {
        it->second.second(product);
    } else {
        THROW_EXCEPTION(StructureCrashException, "Deleter not found for " + product->GetType() + ".\n");
    }
}