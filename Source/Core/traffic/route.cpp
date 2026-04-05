#include "route.h"


using namespace std;

Route::Route(RouteFactory* factory, const string& route) :
	mod(factory->CreateRoute(route)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()) {

}

Route::~Route() {
	factory->DestroyRoute(mod);
}

string Route::GetType() const {
	return type;
}

string Route::GetName() const {
	return name;
}

int EmptyRoute::count = 0;

EmptyRoute::EmptyRoute() : id(count++) {

}

EmptyRoute::~EmptyRoute() {

}

const char* EmptyRoute::GetId() {
	return "empty";
}

const char* EmptyRoute::GetType() const {
	return "empty";
}

const char* EmptyRoute::GetName() {
	name = "空线路" + to_string(id);
	return name.data();
}


