#include "component.h"


using namespace std;

Component::Component(ComponentFactory* factory, string Component) :
	mod(factory->CreateComponent(Component)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	parentBuilding(nullptr),
	rooms() {

}

Component::~Component() {
	factory->DestroyComponent(mod);
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


