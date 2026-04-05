#include "industry.h"

#include "product.h"
#include "storage.h"
#include "manufacture.h"

#include "map/room.h"
#include "map/map.h"


using namespace std;

ProductFactory* Industry::productFactory = nullptr;
StorageFactory* Industry::storageFactory = nullptr;
ManufactureFactory* Industry::manufactureFactory = nullptr;

Industry::Industry() :
	storages(),
	manufactures() {
	if (!productFactory) {
		productFactory = new ProductFactory();
	}
	if (!storageFactory) {
		storageFactory = new StorageFactory();
	}
	if (!manufactureFactory) {
		manufactureFactory = new ManufactureFactory();
	}
}

Industry::~Industry() {
	Destroy();
}

void Industry::LoadConfigs() const {
	productFactory->RemoveAll();
	storageFactory->RemoveAll();
	manufactureFactory->RemoveAll();

	auto products = Config::GetEnables("product");
	for (auto product : products) {
		productFactory->SetConfig(product, true);
	}
	auto storages = Config::GetEnables("storage");
	for (auto storage : storages) {
		storageFactory->SetConfig(storage, true);
	}
	auto manufactures = Config::GetEnables("manufacture");
	for (auto manufacture : manufactures) {
		manufactureFactory->SetConfig(manufacture, true);
	}
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

void Industry::InitStorages(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {
	storageFactory->RegisterStorage(EmptyStorage::GetId(),
		[]() { return new EmptyStorage(); },
		[](StorageMod* storage) { delete storage; }
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

			auto registerFunc = (RegisterModStoragesFunc)GetProcAddress(modHandle, "RegisterModStorages");
			if (registerFunc) {
				registerFunc(storageFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}
}

void Industry::InitManufactures(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {
	manufactureFactory->RegisterManufacture(EmptyManufacture::GetId(),
		[]() { return new EmptyManufacture(); },
		[](ManufactureMod* manufacture) { delete manufacture; }
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

			auto registerFunc = (RegisterModManufacturesFunc)GetProcAddress(modHandle, "RegisterModManufactures");
			if (registerFunc) {
				registerFunc(manufactureFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}
}

void Industry::Init(Map* map) {
	Destroy();

	auto rooms = map->GetRooms();

	for (auto room : rooms) {
		if (room == nullptr) {
			debugf("Warning: Null room in map rooms.\n");
			continue;
		}
		if (room->IsStorage()) {
			for (auto& [type, capacity] : room->StorageConfig()) {
				auto storage = new Storage(storageFactory, type);
				storage->SetProperty(room->GetAcreage());
				room->AddStorage(storage);
				storages.push_back(storage);
			}
		}
		else if (room->IsManufacture()) {
			for (const auto& type : room->ManufactureTypes()) {
				auto manufacture = new Manufacture(manufactureFactory, type);
				manufacture->SetProperty();
				room->AddManufacture(manufacture);
				manufactures.push_back(manufacture);
			}
		}
	}

	unordered_map<string, vector<Storage*>> accepts;
	for (auto storage : storages) {
		if (!storage->GetAccept()) continue;
		for (auto category : storage->GetCategories()) {
			accepts[category].push_back(storage);
		}
	}
	unordered_map<string, vector<Storage*>> provides;
	for (auto storage : storages) {
		if (!storage->GetProvide()) continue;
		for (auto category : storage->GetCategories()) {
			provides[category].push_back(storage);
		}
	}
	for (auto manufacture : manufactures) {
		auto inputCache = manufacture->GetInput();
		auto inputs = inputCache->GetProducts();
		for (auto& [type, input] : inputs) {
			vector<Storage*> suppliers;
			for (auto catagory : input->GetCategories()) {
				auto it = provides.find(catagory);
				if (it == provides.end() || it->second.empty()) {
					continue;
				}
				else {
					suppliers.insert(suppliers.end(), it->second.begin(), it->second.end());
				}
			}
			int r = GetRandom((int)suppliers.size());
			suppliers[r]->ConnectDownstream(type, manufacture->GetInput());
			manufacture->ConnectUpstream(type, suppliers[r]);
		}
		auto outputCache = manufacture->GetOutput();
		auto outputs = outputCache->GetProducts();
		for (auto& [type, output] : outputs) {
			vector<Storage*> buyers;
			for (auto catagory : output->GetCategories()) {
				auto it = accepts.find(catagory);
				if (it == accepts.end() || it->second.empty()) {
					continue;
				}
				else {
					buyers.insert(buyers.end(), it->second.begin(), it->second.end());
				}
			}
			int r = GetRandom((int)buyers.size());
			buyers[r]->ConnectUpstream(type, manufacture->GetOutput());
			manufacture->ConnectDownstream(type, buyers[r]);
		}
	}
}

void Industry::Destroy() {

}

void Industry::Tick(Player* player) {

}

void Industry::ApplyChange(Change* change,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {

}
