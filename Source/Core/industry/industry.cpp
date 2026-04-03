#include "industry.h"

#include "product.h"
#include "storage.h"
#include "manufacture.h"


using namespace std;

ProductFactory* Industry::productFactory = nullptr;

Industry::Industry() {
	if (!productFactory) {
		productFactory = new ProductFactory();
	}
}

Industry::~Industry() {
	Destroy();
}

void Industry::LoadConfigs() const {
	productFactory->RemoveAll();
	//storageFactory->RemoveAll();
	//manufactureFactory->RemoveAll();

	auto products = Config::GetEnables("product");
	for (auto product : products) {
		productFactory->SetConfig(product, true);
	}
	//auto storages = Config::GetEnables("storage");
	//for (auto storage : storages) {
	//	storageFactory->SetConfig(storage, true);
	//}
	//auto manufactures = Config::GetEnables("manufacture");
	//for (auto manufacture : manufactures) {
	//	manufactureFactory->SetConfig(manufacture, true);
	//}
}

void Industry::InitProducts(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {
	productFactory->RegisterProduct(EmptyProduct::GetId(),
		[]() { return new EmptyProduct(); },
		[](ProductMod* product) { delete product; }
	);

	for (auto dll : dlls) {
		HMODULE modHandle;
		if (modHandles.find(dll) != modHandles.end()) {
			modHandle = modHandles[dll];
		}
		else {
			modHandle = LoadLibraryA(dll.data());
			modHandles[dll] = modHandle;
		}
		if (modHandle) {
			debugf("Log: %s loaded successfully.\n", dll.data());

			auto registerFunc = (RegisterModProductsFunc)GetProcAddress(modHandle, "RegisterModProducts");
			if (registerFunc) {
				registerFunc(productFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}
}

void Industry::Init(Map* map) {

}

void Industry::Destroy() {

}

void Industry::ApplyChange(Change* change,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {

}
