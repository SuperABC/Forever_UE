#include "vehicle.h"

#include "map/room.h"
#include "story/story.h"
#include "story/script.h"


using namespace std;

Vehicle::Vehicle(VehicleFactory* factory, const string& label) :
	mod(factory->CreateVehicle(label)),
	factory(factory),
	type(),
	name(),
	path(),
	owner(nullptr),
	driver(nullptr),
	room(nullptr),
	xyzr({ 0.f, 0.f, 0.f, 0.f }),
	script(nullptr) {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Vehicle " + label + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
	path = mod->path;
}

Vehicle::~Vehicle() {
	factory->DestroyVehicle(mod);

	if (script) delete script;
	script = nullptr;
}

string Vehicle::GetType() const {
	return type;
}

string Vehicle::GetName() const {
	return name;
}

string Vehicle::GetPath() const {
	return path;
}

Room* Vehicle::GetRoom() const {
	return room;
}

vector<float> Vehicle::GetPosition() const {
	return xyzr;
}

void Vehicle::EnterRoom(Room* room) {
	if (this->room) {
		this->room->RemoveVehicle(this);
	}
	this->room = room;
	room->AddVehicle(this);
}

void Vehicle::LeaveRoom() {
	if (room) {
		room->RemoveVehicle(this);
		room = nullptr;
	}
}

void Vehicle::SetPosition(float x, float y, float z, float r) {
	xyzr = { x, y, z, r };
}

void Vehicle::SetupVehicle() {
	mod->SetupVehicle();
	path = mod->path;
	width = mod->width;
	length = mod->length;

	if (!mod->script.first.empty()) {
		script = new Script(Story::scriptFactory, mod->script.first);
		for (auto s : mod->script.second) {
			script->ReadMilestones(Config::GetScript(s));
		}
		script->SetValue("self.name", name);
	}
}

Script* Vehicle::GetScript() const {
	return script;
}

bool Vehicle::AddOption(const string& option) {
	if (options.find(option) == options.end()) {
		options.insert(option);
		return true;
	}
	return false;
}

bool Vehicle::RemoveOption(const string& option) {
	if (options.find(option) != options.end()) {
		options.erase(option);
		return true;
	}
	return false;
}

unordered_set<string> Vehicle::GetOptions() const {
	return options;
}

int EmptyVehicle::count = 0;

EmptyVehicle::EmptyVehicle() : id(count++) {

}

EmptyVehicle::~EmptyVehicle() {

}

const char* EmptyVehicle::GetId() {
	return "empty";
}

const char* EmptyVehicle::GetType() const {
	return "empty";
}

const char* EmptyVehicle::GetName() {
	name = "空载具" + to_string(id);
	return name.data();
}

string EmptyVehicle::GetLabel() {
	return "empty";
}

void EmptyVehicle::SetupVehicle() {

}


