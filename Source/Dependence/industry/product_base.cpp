#include "../common/error.h"
#include "../common/utility.h"

#include "product_base.h"


using namespace std;

// 全成员默认构造
Product::Product() :
    amount(0.f),
    price(0.f) {

}

// 无析构
Product::~Product() {
    
}

// 获取数量
float Product::GetAmount() const {
    return amount;
}

// 设置数量
void Product::SetAmount(float amount) {
    this->amount = amount;
}

// 增加数量
void Product::IncreaseAmount(float amount) {
    this->amount += amount;
}

// 减少数量
void Product::DecreaseAmount(float amount) {
    this->amount -= amount;
}

// 获取单价
float Product::GetPrice() const {
    return price;
}

// 设置单价
void Product::SetPrice(float price) {
    this->price = price;
}

// 注册产品
void ProductFactory::RegisterProduct(const string& id,
    function<Product*()> creator, function<void(Product*)> deleter) {
    registries[id] = {creator, deleter};
}

// 创建产品
Product* ProductFactory::CreateProduct(const string& id) const {
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
    } else {
        THROW_EXCEPTION(NullPointerException, "Product " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool ProductFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void ProductFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 析构产品
void ProductFactory::DestroyProduct(Product* product) const {
    if (!product) {
        debugf("Warning: Product is null when deleting.\n");
        return;
    }

    auto it = registries.find(product->GetType());
    if (it == registries.end()) {
        debugf("Warning: Product  not registered when deleting.\n", product->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(product);
    } else {
        THROW_EXCEPTION(NullPointerException, "Product " + product->GetType() + " deleter is null.\n");
    }
}

