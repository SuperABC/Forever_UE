#include "storage_base.h"


using namespace std;

Storage::Storage() {

}

Storage::~Storage() {
    for (auto income : incomes) {
        delete income;
    }
    for (auto outcome : outcomes) {
        delete outcome;
    }
}

void Storage::SetAccept(bool accept) {
	this->accept = accept;
}

bool Storage::GetAccept() const {
    return this->accept;
}

void Storage::AddIncome(string product, float amount, ProductFactory* factory) {
    for(auto income : incomes) {
        if(income->GetName() == product) {
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

Product* Storage::GetIncome(string product) const {
    for (auto income : incomes) {
        if (income->GetType() == product) {
            return income;
        }
    }
    return nullptr;
}

vector<Product*> Storage::GetIncomes() const {
    return incomes;
}

void Storage::SetProvide(bool provide) {
    this->provide = provide;
}

bool Storage::GetProvide() const {
    return this->provide;
}

void Storage::AddOutcome(string product, float amount, ProductFactory* factory) {
    for (auto outcome : outcomes) {
        if (outcome->GetName() == product) {
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

Product* Storage::GetOutcome(string product) const {
    for (auto outcome : outcomes) {
        if (outcome->GetType() == product) {
            return outcome;
        }
    }
    return nullptr;
}

vector<Product*> Storage::GetOutcomes() const {
    return incomes;
}

void Storage::ConnectUpstream(Manufacture* manufacture) {
    upstreams.push_back(manufacture);
}

vector<Manufacture*> Storage::GetUpstreams() const {
    return upstreams;
}

void Storage::ConnectDownstream(Manufacture* manufacture) {
    downstreams.push_back(manufacture);
}

vector<Manufacture*> Storage::GetDownstreams() const {
    return downstreams;
}

void Storage::SetVolume(float volume) {
    this->volume = volume;
}

float Storage::GetVolume() const {
    return this->volume;
}

void StorageFactory::RegisterStorage(const string& id, function<Storage*()> creator) {
    registries[id] = creator;
}

Storage* StorageFactory::CreateStorage(const string& id) {
    if (configs.find(id) == configs.end() || !configs.find(id)->second)return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second();
    }
    return nullptr;
}

bool StorageFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void StorageFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}
