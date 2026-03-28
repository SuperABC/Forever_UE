#include "script_basic.h"


using namespace std;

int NPCScript::count = 0;

NPCScript::NPCScript() : id(count++) {

}

NPCScript::~NPCScript() {

}

const char* NPCScript::GetId() {
	return "npc";
}

const char* NPCScript::GetType() const {
	return "npc";
}

const char* NPCScript::GetName() {
	name = "NPC脚本" + to_string(id);
	return name.data();
}

bool NPCScript::MainStory() {
	return false;
}

void NPCScript::SetScript() {

}

void NPCScript::PreTrigger(Event* event) {

}

void NPCScript::PostTrigger(Event* event) {

}
