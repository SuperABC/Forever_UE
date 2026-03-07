#include "room.h"


using namespace std;

// 默认住宅房间

DefaultResidentialRoom::DefaultResidentialRoom() {
}

DefaultResidentialRoom::~DefaultResidentialRoom() {
}

string DefaultResidentialRoom::GetId() {
	return "default_residential";
}

string DefaultResidentialRoom::GetType() const {
	return "default_residential";
}

string DefaultResidentialRoom::GetName() const {
	return "默认住宅房间";
}

bool DefaultResidentialRoom::IsResidential() const {
	return true;
}

bool DefaultResidentialRoom::IsWorkspace() const {
	return false;
}

bool DefaultResidentialRoom::IsStorage() const {
	return false;
}

bool DefaultResidentialRoom::IsManufacture() const {
	return false;
}

int DefaultResidentialRoom::ResidentialCapacity() const {
	return 1;
}

int DefaultResidentialRoom::WorkspaceCapacity() const {
	return 0;
}

pair<string, float> DefaultResidentialRoom::StorageConfig() const {
	return { "", 0.f };
}

vector<string> DefaultResidentialRoom::ManufactureTypes() const {
	return { "" };
}

// 默认工作房间

DefaultWorkingRoom::DefaultWorkingRoom() {
}

DefaultWorkingRoom::~DefaultWorkingRoom() {
}

string DefaultWorkingRoom::GetId() {
	return "default_working";
}

string DefaultWorkingRoom::GetType() const {
	return "default_working";
}

string DefaultWorkingRoom::GetName() const {
	return "默认工作房间";
}

bool DefaultWorkingRoom::IsResidential() const {
	return false;
}

bool DefaultWorkingRoom::IsWorkspace() const {
	return true;
}

bool DefaultWorkingRoom::IsStorage() const {
	return false;
}

bool DefaultWorkingRoom::IsManufacture() const {
	return false;
}

int DefaultWorkingRoom::ResidentialCapacity() const {
	return 0;
}

int DefaultWorkingRoom::WorkspaceCapacity() const {
	return 50;
}

pair<string, float> DefaultWorkingRoom::StorageConfig() const {
	return { "", 0.f };
}

vector<string> DefaultWorkingRoom::ManufactureTypes() const {
	return { "" };
}