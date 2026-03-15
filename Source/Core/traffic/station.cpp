#include "station.h"


using namespace std;

// 机场站台

string AirportStation::GetId() {
	return "airpot";
}

string AirportStation::GetType() const {
	return "airpot";
}

string AirportStation::GetName() const {
	return "机场";
}

bool AirportStation::IsResidential() const {
	return true;
}

bool AirportStation::IsWorkspace() const {
	return false;
}

bool AirportStation::IsStorage() const {
	return false;
}

bool AirportStation::IsManufacture() const {
	return false;
}

int AirportStation::ResidentialCapacity() const {
	return 0;
}

int AirportStation::WorkspaceCapacity() const {
	return 10;
}

pair<string, float> AirportStation::StorageConfig() const {
	return { "", 0.f };
}

vector<string> AirportStation::ManufactureTypes() const {
	return { "" };
}
