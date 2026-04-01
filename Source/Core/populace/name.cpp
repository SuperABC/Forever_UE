#include "name.h"


using namespace std;

Name::Name(NameFactory* factory, const string& name) :
	mod(factory->CreateName(name)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	reserve(),
	roll() {

}

Name::~Name() {
	factory->DestroyName(mod);
}

string Name::GetType() const {
	return type;
}

string Name::GetName() const {
	return name;
}

void Name::GetSurname(function<void(const string&)> set,
	const string& name) {
	mod->GetSurname(set, name);
}

void Name::GenerateName(function<void(const string&)> set,
	bool male, bool female, bool neutral) {
	string name = "";
	mod->GenerateName([&](const string& n) {name = n; }, male, female, neutral);
	if (RegisterName(name)) {
		set(name);
	}
	else {
		set("");
	}
}

void Name::GenerateName(function<void(const string&)> set,
	const string& surname, bool male, bool female, bool neutral) {
	string name = "";
	mod->GenerateName([&](const string& n) {name = n; }, surname, male, female, neutral);
	if (RegisterName(name)) {
		set(name);
	}
	else {
		set("");
	}
}

bool Name::ReserveName(const string& name) {
	if (reserve.find(name) == reserve.end()) {
		reserve.insert(name);
		return true;
	}
	return false;
}

bool Name::RegisterName(const string& name) {
	if (roll.find(name) == roll.end() && reserve.find(name) == reserve.end()) {
		roll.insert(name);
		return true;
	}
	return false;
}

int EmptyName::count = 0;

EmptyName::EmptyName() : id(count++) {

}

EmptyName::~EmptyName() {

}

const char* EmptyName::GetId() {
	return "empty";
}

const char* EmptyName::GetType() const {
	return "empty";
}

const char* EmptyName::GetName() {
	name = "空姓名" + to_string(id);
	return name.data();
}

void EmptyName::GetSurname(function<void(const string&)> set,
	const string& name) {

}

void EmptyName::GenerateName(function<void(const string&)> set,
	bool male, bool female, bool neutral) {

}

void EmptyName::GenerateName(function<void(const string&)> set,
	const string& surname, bool male, bool female, bool neutral) {

}

