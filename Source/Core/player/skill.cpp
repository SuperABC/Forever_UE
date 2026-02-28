#include "skill.h"


using namespace std;

string DefaultSkill::GetId() {
	return "default";
}

string DefaultSkill::GetType() const {
	return "default";
}

string DefaultSkill::GetName() const {
	return "默认技能";
}
