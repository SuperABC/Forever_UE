#include "skill.h"


using namespace std;

// 默认技能

string DefaultSkill::GetId() {
	return "default";
}

string DefaultSkill::GetType() const {
	return "default";
}

string DefaultSkill::GetName() const {
	return "默认技能";
}
