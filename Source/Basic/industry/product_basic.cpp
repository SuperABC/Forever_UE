#include "product_basic.h"


using namespace std;

int ExperienceProduct::count = 0;

ExperienceProduct::ExperienceProduct() : id(count++) {

}

ExperienceProduct::~ExperienceProduct() {

}

const char* ExperienceProduct::GetId() {
	return "workonly";
}

const char* ExperienceProduct::GetType() const {
	return "workonly";
}

const char* ExperienceProduct::GetName() {
	name = "经验产品" + to_string(id);
	return name.data();
}

void ExperienceProduct::SetProperty() {
	catagories = { "empty" };
}
