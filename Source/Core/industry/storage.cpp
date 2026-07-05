#include "storage.h"

#include "map/room.h"
#include "map/component.h"
#include "story/script.h"
#include "industry/industry.h"
#include "industry/manufacture.h"
#include "industry/product.h"


using namespace std;

Storage::Storage(StorageFactory* factory, const string& storage) :
	mod(factory->CreateStorage(storage)),
	factory(factory),
	type(),
	name(),
	room(nullptr),
	catagories(),
	accept(true),
	provide(true),
	products(),
	upstreams(),
	downstreams(),
	volume(1.f) {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Storage " + storage + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Storage::~Storage() {
	factory->DestroyStorage(mod);

	for (auto &[_, product] : products) {
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

Room* Storage::GetRoom() const {
	return room;
}

void Storage::SetRoom(Room* room) {
	this->room = room;
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
	for (auto& [name, product] : products) {
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
	float actual = min(amount, max(GetSpace(), 0.f));
	if (actual <= 0.f) return 0.f;

	auto it = products.find(type);
	if (it != products.end()) {
		products[type]->IncreaseAmount(actual);
		if (room && room->GetParentComponent() && room->GetParentComponent()->GetScript()) {
			auto [find, temp] = room->GetParentComponent()->GetScript()->GetValue("system.storage." + type);
			if (find) {
				room->GetParentComponent()->GetScript()->SetValue("system.storage." + type, get<double>(temp) + actual);
			}
		}
		return actual;
	}

	auto product = new Product(Industry::productFactory, type);
	product->SetProperty();
	product->SetAmount(actual);
	products[type] = product;
	if (room && room->GetParentComponent() && room->GetParentComponent()->GetScript()) {
		room->GetParentComponent()->GetScript()->SetValue("system.storage." + type, static_cast<double>(actual));
	}
	return actual;
}

float Storage::OutputProduct(const string& type, float amount) {
	auto it = products.find(type);
	if (it != products.end()) {
		float actual = min(amount, max(it->second->GetAmount(), 0.f));
		if (actual <= 0.f) return 0.f;
		products[type]->DecreaseAmount(actual);
		if (room && room->GetParentComponent() && room->GetParentComponent()->GetScript()) {
			auto [find, temp] = room->GetParentComponent()->GetScript()->GetValue("system.storage." + type);
			if (find) {
				room->GetParentComponent()->GetScript()->SetValue("system.storage." + type, get<double>(temp) - actual);
			}
		}
		return actual;
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
	for (auto& [_, product] : products) {
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
