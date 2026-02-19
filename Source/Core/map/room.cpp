#include "room.h"


using namespace std;

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

int DefaultResidentialRoom::GetLivingCapacity() const {
	return 1;
}

int DefaultResidentialRoom::GetPersonnelCapacity() const {
	return 0;
}

pair<string, float> DefaultResidentialRoom::GetIndustryCapacity() const {
	return { "", 0.f };
}

string DefaultResidentialRoom::GetFactoryPipeline() const {
	return "";
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

int DefaultWorkingRoom::GetLivingCapacity() const {
	return 0;
}

int DefaultWorkingRoom::GetPersonnelCapacity() const {
	return 50;
}

pair<string, float> DefaultWorkingRoom::GetIndustryCapacity() const {
	return { "", 0.f };
}

string DefaultWorkingRoom::GetFactoryPipeline() const {
	return "";
}
