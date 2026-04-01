#include "industry.h"


using namespace std;

Industry::Industry() {
}

Industry::~Industry() {
}

void Industry::LoadConfigs() const {
	//productFactory->RemoveAll();
	//storageFactory->RemoveAll();
	//manufactureFactory->RemoveAll();

	//auto products = Config::GetEnables("product");
	//for (auto product : products) {
	//	productFactory->SetConfig(product, true);
	//}
	//auto storages = Config::GetEnables("storage");
	//for (auto storage : storages) {
	//	storageFactory->SetConfig(storage, true);
	//}
	//auto manufactures = Config::GetEnables("manufacture");
	//for (auto manufacture : manufactures) {
	//	manufactureFactory->SetConfig(manufacture, true);
	//}
}

void Industry::ApplyChange(Change* change,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {

}
