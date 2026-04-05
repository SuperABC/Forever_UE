#include "skill_basic.h"


using namespace std;

int InfomationSkill::count = 0;

InfomationSkill::InfomationSkill() : id(count++) {

}

InfomationSkill::~InfomationSkill() {

}

const char* InfomationSkill::GetId() {
	return "information";
}

const char* InfomationSkill::GetType() const {
	return "information";
}

const char* InfomationSkill::GetName() {
	name = "读心技能" + to_string(id);
	return name.data();
}
