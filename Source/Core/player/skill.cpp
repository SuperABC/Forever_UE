#include "skill.h"


using namespace std;

Skill::Skill(SkillFactory* factory, const string& skill) :
	mod(factory->CreateSkill(skill)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()) {

}

Skill::~Skill() {
	factory->DestroySkill(mod);
}

string Skill::GetType() const {
	return type;
}

string Skill::GetName() const {
	return name;
}

int EmptySkill::count = 0;

EmptySkill::EmptySkill() : id(count++) {

}

EmptySkill::~EmptySkill() {

}

const char* EmptySkill::GetId() {
	return "empty";
}

const char* EmptySkill::GetType() const {
	return "empty";
}

const char* EmptySkill::GetName() {
	name = "空技能" + to_string(id);
	return name.data();
}


