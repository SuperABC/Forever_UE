#include "product.h"


using namespace std;

Product::Product(ProductFactory* factory, const string& product) :
	mod(factory->CreateProduct(product)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	catagories(),
	price(0.f),
	amount(0.f),
	ingredients(),
	byproducts() {

}

Product::~Product() {
	factory->DestroyProduct(mod);
}

string Product::GetType() const {
	return type;
}

string Product::GetName() const {
	return name;
}

void Product::SetProperty() {
	mod->SetProperty();
	catagories = mod->catagories;
	price = mod->price;
	ingredients = mod->ingredients;
	byproducts = mod->byproducts;
}

float Product::GetAmount() const {
	return amount;
}

void Product::SetAmount(float amount) {
	this->amount = amount;
}

void Product::IncreaseAmount(float amount) {
	this->amount += amount;
}

void Product::DecreaseAmount(float amount) {
	this->amount -= amount;
}

float Product::GetPrice() const {
	return price;
}

void Product::SetPrice(float price) {
	this->price = price;
}

int EmptyProduct::count = 0;

EmptyProduct::EmptyProduct() : id(count++) {

}

EmptyProduct::~EmptyProduct() {

}

const char* EmptyProduct::GetId() {
	return "empty";
}

const char* EmptyProduct::GetType() const {
	return "empty";
}

const char* EmptyProduct::GetName() {
	name = "空产品" + to_string(id);
	return name.data();
}

void EmptyProduct::SetProperty() {

}


