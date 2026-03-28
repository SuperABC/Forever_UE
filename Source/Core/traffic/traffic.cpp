#include "traffic.h"
#include "utility.h"
#include "error.h"


using namespace std;

Traffic::Traffic() {
}

Traffic::~Traffic() {

}

void Traffic::LoadConfigs() const {
	//routeFactory->RemoveAll();
	//stationFactory->RemoveAll();
	//vehicleFactory->RemoveAll();

	//auto routes = Config::GetEnables("route");
	//for (auto route : routes) {
	//	routeFactory->SetConfig(route, true);
	//}
	//auto stations = Config::GetEnables("station");
	//for (auto station : stations) {
	//	stationFactory->SetConfig(station, true);
	//}
	//auto vehicles = Config::GetEnables("vehicle");
	//for (auto vehicle : vehicles) {
	//	vehicleFactory->SetConfig(vehicle, true);
	//}
}

void Traffic::ApplyChange(Change* change,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {

}
