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
	name = "默认NPC脚本" + to_string(id);
	return name.data();
}

bool NPCScript::MainStory() {
	return false;
}

void NPCScript::SetScript() {

}

void NPCScript::PreTrigger(const Event* event) {

}

void NPCScript::PostTrigger(const Event* event) {

}

int ElevatorScript::count = 0;

ElevatorScript::ElevatorScript() : id(count++) {

}

ElevatorScript::~ElevatorScript() {

}

const char* ElevatorScript::GetId() {
	return "elevator";
}

const char* ElevatorScript::GetType() const {
	return "elevator";
}

const char* ElevatorScript::GetName() {
	name = "默认电梯脚本" + to_string(id);
	return name.data();
}

bool ElevatorScript::MainStory() {
	return false;
}

void ElevatorScript::SetScript() {

}

void ElevatorScript::PreTrigger(const Event* event) {

}

void ElevatorScript::PostTrigger(const Event* event) {

}
