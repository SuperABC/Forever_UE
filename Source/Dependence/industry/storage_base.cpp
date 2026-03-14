#include "../common/error.h"
#include "../common/utility.h"

#include "storage_base.h"


using namespace std;

// 全成员默认构造
Storage::Storage() :
    accept(true),
    provide(true),
    incomes(),
    outcomes(),
    upstreams(),
    downstreams(),
    volume(0.f) {

}

// 析构自有Product
Storage::~Storage() {
    for (auto income : incomes) {
        delete income;
    }
    for (auto outcome : outcomes) {
        delete outcome;
    }
}

// 获取是否接收产品
bool Storage::GetAccept() const {
    return accept;
}

// 设置是否接收产品
void Storage::SetAccept(bool accept) {
    this->accept = accept;
}

// 获取一种接收产品
Product* Storage::GetIncome(const string& product) const {
    for (auto income : incomes) {
        if (income->GetType() == product) {
            return income;
        }
    }
    return nullptr;
}

// 获取全部接收产品
vector<Product*> Storage::GetIncomes() const {
    return incomes;
}

// 新增接收产品种类
void Storage::AddIncome(const string& product, float amount, ProductFactory* factory) {
    for (auto income : incomes) {
        if (income->GetType() == product) {
            income->IncreaseAmount(amount);
            return;
        }
    }

    auto income = factory->CreateProduct(product);
    if (income) {
        income->SetAmount(amount);
        incomes.push_back(income);
    }
}

// 获取是否提供产品
bool Storage::GetProvide() const {
    return provide;
}

// 设置是否提供产品
void Storage::SetProvide(bool provide) {
    this->provide = provide;
}

// 获取一种提供产品
Product* Storage::GetOutcome(const string& product) const {
    for (auto outcome : outcomes) {
        if (outcome->GetType() == product) {
            return outcome;
        }
    }
    return nullptr;
}

// 获取全部提供产品
vector<Product*> Storage::GetOutcomes() const {
    return outcomes;
}

// 新增提供产品种类
void Storage::AddOutcome(const string& product, float amount, ProductFactory* factory) {
    for (auto outcome : outcomes) {
        if (outcome->GetType() == product) {
            outcome->IncreaseAmount(amount);
            return;
        }
    }

    auto outcome = factory->CreateProduct(product);
    if (outcome) {
        outcome->SetAmount(amount);
        outcomes.push_back(outcome);
    }
}

// 获取上游工坊
vector<Manufacture*> Storage::GetUpstreams() const {
    return upstreams;
}

// 设置上游工坊
void Storage::ConnectUpstream(Manufacture* manufacture) {
    upstreams.push_back(manufacture);
}

// 获取下游工坊
vector<Manufacture*> Storage::GetDownstreams() const {
    return downstreams;
}

// 设置下游工坊
void Storage::ConnectDownstream(Manufacture* manufacture) {
    downstreams.push_back(manufacture);
}

// 获取容量
float Storage::GetVolume() const {
    return volume;
}

// 设置容量
void Storage::SetVolume(float volume) {
    this->volume = volume;
}

// 注册仓库
void StorageFactory::RegisterStorage(const string& id,
    function<Storage*()> creator, function<void(Storage*)> deleter) {
    registries[id] = {creator, deleter};
}

// 创建仓库
Storage* StorageFactory::CreateStorage(const string& id) const {
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Storage %s not enabled when creating.\n", id.data());
        return nullptr;
    }

    auto it = registries.find(id);
    if (it == registries.end()) {
        debugf("Warning: Storage %s not registered when creating.\n", id.data());
        return nullptr;
    }

    if (it->second.first) {
        return it->second.first();
    } else {
        THROW_EXCEPTION(NullPointerException, "Storage " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool StorageFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void StorageFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 析构仓库
void StorageFactory::DestroyStorage(Storage* storage) const {
    if (!storage) {
        debugf("Warning: Storage is null when deleting.\n");
        return;
    }

    auto it = registries.find(storage->GetType());
    if (it == registries.end()) {
        debugf("Warning: Storage %s not registered when deleting.\n", storage->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(storage);
    } else {
        THROW_EXCEPTION(NullPointerException, "Storage " + storage->GetType() + " deleter is null.\n");
    }
}

