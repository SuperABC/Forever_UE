#include "storage.h"

#include "industry/industry.h"


using namespace std;

Storage::Storage(StorageFactory* factory, const string& storage) :
	mod(factory->CreateStorage(storage)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	catagories(),
	accept(true),
	provide(true),
	products(),
	upstreams(),
	downstreams(),
    volume(1.f) {

}

Storage::~Storage() {
	factory->DestroyStorage(mod);

    for (auto &[name, product] : products) {
        if (product)delete product;
        product = nullptr;
    }
    products.clear();
}

string Storage::GetType() const {
	return type;
}

string Storage::GetName() const {
	return name;
}

void Storage::SetProperty(float acreage) {
    mod->SetProperty();
    catagories = mod->catagories;
    accept = mod->accept;
    provide = mod->provide;
    volume = mod->density * acreage;
}

vector<string> Storage::GetCategories() const {
	return catagories;
}

bool Storage::GetAccept() const {
    return accept;
}

void Storage::SetAccept(bool accept) {
    this->accept = accept;
}

bool Storage::GetProvide() const {
    return provide;
}

void Storage::SetProvide(bool provide) {
    this->provide = provide;
}

Product* Storage::GetProduct(const string& type) const {
    for (auto [name, product] : products) {
        if (name == type) {
            return product;
        }
    }
    return nullptr;
}

unordered_map<string, Product*> Storage::GetProducts() const {
    return products;
}

float Storage::InputProduct(const string& type, float amount) {
    auto it = products.find(type);
    if (it != products.end()) {
        products[type]->IncreaseAmount(amount);
        return amount;
    }

    auto product = new Product(Industry::productFactory, type);
    product->SetProperty();
    product->SetAmount(amount);
    products[type] = product;
    return amount;
}

float Storage::OutputProduct(const string& type, float amount) {
    auto it = products.find(type);
    if (it != products.end()) {
        products[type]->DecreaseAmount(amount);
        return amount;
    }

    return 0.f;
}

unordered_map<string, Storage*> Storage::GetUpstreams() const {
    return upstreams;
}

void Storage::ConnectUpstream(string type, Storage* storage) {
    upstreams[type] = storage;
}

unordered_map<string, Storage*> Storage::GetDownstreams() const {
    return downstreams;
}

void Storage::ConnectDownstream(string type, Storage* storage) {
    downstreams[type] = storage;
}

float Storage::GetVolume() const {
    return volume;
}

void Storage::SetVolume(float volume) {
    this->volume = volume;
}

float Storage::GetSpace() const {
    return volume - GetOccupied();
}

float Storage::GetOccupied() const {
    float occupied = 0.f;
    for (auto& [type, product] : products) {
        occupied += product->GetAmount();
    }
    return occupied;
}

int EmptyStorage::count = 0;

EmptyStorage::EmptyStorage() : id(count++) {

}

EmptyStorage::~EmptyStorage() {

}

const char* EmptyStorage::GetId() {
	return "empty";
}

const char* EmptyStorage::GetType() const {
	return "empty";
}

const char* EmptyStorage::GetName() {
	name = "空仓库" + to_string(id);
	return name.data();
}

void EmptyStorage::SetProperty() {
    catagories = { "all" };
}
