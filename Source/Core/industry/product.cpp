#include "product.h"


using namespace std;

string DefaultProduct::GetId() {
	return "default";
}

string DefaultProduct::GetType() const {
	return "default";
}

string DefaultProduct::GetName() const {
	return "默认产品";
}

vector<string> DefaultProduct::GetCategories() const {
	return { "默认分类" };
}

float DefaultProduct::UnitVolume() const {
	return 1.f;
}

float DefaultProduct::StandardPrice() const {
	return 1.f;
}

unordered_map<string, float> DefaultProduct::GetIngredients() const {
	return {};
}

unordered_map<string, float> DefaultProduct::GetByproducts() const {
	return {};
}

