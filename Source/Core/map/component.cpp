#include "component.h"

#include "map/building.h"
#include "map/room.h"
#include "story/story.h"
#include "story/script.h"


using namespace std;

Component::Component(ComponentFactory* factory, const string& component) :
	mod(factory->CreateComponent(component)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	parentBuilding(nullptr),
	rooms(),
	script(nullptr) {

}

Component::~Component() {
	factory->DestroyComponent(mod);

	if (script)delete script;
	script = nullptr;
}

string Component::GetType() const {
	return type;
}

string Component::GetName() const {
	return name;
}

Building* Component::GetParent() const {
	return parentBuilding;
}

void Component::SetParent(Building* building) {
	parentBuilding = building;
}

vector<Room*>& Component::GetRooms() {
	return rooms;
}

void Component::AddRoom(Room* room) {
	if (room == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Room is null when adding to component.");
	}
	rooms.push_back(room);
}

Script* Component::GetScript() const {
	return script;
}

void Component::InitComponent() {
	script = new Script(Story::scriptFactory, mod->script.first);
	for (auto s : mod->script.second) {
		script->ReadMilestones(Config::GetScript(s));
	}
	script->SetValue("self.name", name);
}

int EmptyComponent::count = 0;

EmptyComponent::EmptyComponent() : id(count++) {

}

EmptyComponent::~EmptyComponent() {

}

const char* EmptyComponent::GetId() {
	return "empty";
}

const char* EmptyComponent::GetType() const {
	return "empty";
}

const char* EmptyComponent::GetName() {
	name = "空组合" + to_string(id);
	return name.data();
}

void EmptyComponent::InitComponent() {

}


