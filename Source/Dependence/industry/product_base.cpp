#include "product_base.h"


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

void ProductFactory::RegisterProduct(const string& id, function<Product*()> creator) {
    registries[id] = creator;
}

Product* ProductFactory::CreateProduct(const string& id) {
    if (configs.find(id) == configs.end() || !configs.find(id)->second)return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second();
    }
    return nullptr;
}

bool ProductFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void ProductFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}
