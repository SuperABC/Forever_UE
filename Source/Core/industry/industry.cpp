#include "industry.h"
#include "utility.h"
#include "error.h"
#include "json.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <random>


using namespace std;

ProductFactory* Industry::productFactory = nullptr;
StorageFactory* Industry::storageFactory = nullptr;
ManufactureFactory* Industry::manufactureFactory = nullptr;

Industry::Industry() :
	resourcePath(),
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
	for (auto storage : storages) {
		if (storage != nullptr) {
			storageFactory->DestroyStorage(storage);
		}
	}
	for (auto manufacture : manufactures) {
		if (manufacture != nullptr) {
			manufactureFactory->DestroyManufacture(manufacture);
		}
	}
}

void Industry::SetResourcePath(const string& path) {
	resourcePath = path;
}

void Industry::InitProducts(unordered_map<string, HMODULE>& modHandles) {
	productFactory->RegisterProduct(DefaultProduct::GetId(),
		[]() { return new DefaultProduct(); },
		[](Product* product) { delete product; }
	);

	string modPath = "Mod.dll";
	HMODULE modHandle;
	if (modHandles.find(modPath) != modHandles.end()) {
		modHandle = modHandles[modPath];
	}
	else {
		modHandle = LoadLibraryA(modPath.data());
		modHandles[modPath] = modHandle;
	}
	if (modHandle) {
		debugf("Log: Mod dll loaded successfully.\n");
		RegisterModProductsFunc registerFunc =
			(RegisterModProductsFunc)GetProcAddress(modHandle, "RegisterModProducts");
		if (registerFunc) {
			registerFunc(productFactory);
		}
		else {
			debugf("Warning: Incorrect dll content.\n");
		}
	}
	else {
		debugf("Warning: Failed to load mod dll.\n");
	}

#ifdef MOD_TEST
	auto productList = { "mod" };
	for (const auto& productId : productList) {
		if (productFactory->CheckRegistered(productId)) {
			auto product = productFactory->CreateProduct(productId);
			debugf("Log: Created test product %s.\n", productId);
			productFactory->DestroyProduct(product);
		}
		else {
			debugf("Warning: Product %s not registered.\n", productId);
		}
	}
#endif // MOD_TEST
}

void Industry::InitStorages(unordered_map<string, HMODULE>& modHandles) {
	storageFactory->RegisterStorage(DefaultStorage::GetId(),
		[]() { return new DefaultStorage(); },
		[](Storage* storage) { delete storage; }
	);

	string modPath = "Mod.dll";
	HMODULE modHandle;
	if (modHandles.find(modPath) != modHandles.end()) {
		modHandle = modHandles[modPath];
	}
	else {
		modHandle = LoadLibraryA(modPath.data());
		modHandles[modPath] = modHandle;
	}
	if (modHandle) {
		debugf("Log: Mod dll loaded successfully.\n");
		RegisterModStoragesFunc registerFunc =
			(RegisterModStoragesFunc)GetProcAddress(modHandle, "RegisterModStorages");
		if (registerFunc) {
			registerFunc(storageFactory);
		}
		else {
			debugf("Warning: Incorrect dll content.\n");
		}
	}
	else {
		debugf("Warning: Failed to load mod dll.\n");
	}

#ifdef MOD_TEST
	auto storageList = { "mod" };
	for (const auto& storageId : storageList) {
		if (storageFactory->CheckRegistered(storageId)) {
			auto storage = storageFactory->CreateStorage(storageId);
			debugf("Log: Created test storage %s.\n", storageId);
			storageFactory->DestroyStorage(storage);
		}
		else {
			debugf("Warning: Storage %s not registered.\n", storageId);
		}
	}
#endif // MOD_TEST
}

void Industry::InitManufactures(unordered_map<string, HMODULE>& modHandles) {
	manufactureFactory->RegisterManufacture(DefaultManufacture::GetId(),
		[]() { return new DefaultManufacture(); },
		[](Manufacture* manufacture) { delete manufacture; }
	);

	string modPath = "Mod.dll";
	HMODULE modHandle;
	if (modHandles.find(modPath) != modHandles.end()) {
		modHandle = modHandles[modPath];
	}
	else {
		modHandle = LoadLibraryA(modPath.data());
		modHandles[modPath] = modHandle;
	}
	if (modHandle) {
		debugf("Log: Mod dll loaded successfully.\n");
		RegisterModManufacturesFunc registerFunc =
			(RegisterModManufacturesFunc)GetProcAddress(modHandle, "RegisterModManufactures");
		if (registerFunc) {
			registerFunc(manufactureFactory);
		}
		else {
			debugf("Warning: Incorrect dll content.\n");
		}
	}
	else {
		debugf("Warning: Failed to load mod dll.\n");
	}

#ifdef MOD_TEST
	auto manufactureList = { "mod" };
	for (const auto& manufactureId : manufactureList) {
		if (manufactureFactory->CheckRegistered(manufactureId)) {
			auto manufacture = manufactureFactory->CreateManufacture(manufactureId);
			debugf("Log: Created test manufacture %s.\n", manufactureId);
			manufactureFactory->DestroyManufacture(manufacture);
		}
		else {
			debugf("Warning: Manufacture %s not registered.\n", manufactureId);
		}
	}
#endif // MOD_TEST
}

void Industry::Init(Map* map) {
	if (map == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Map is null.\n");
	}

	auto rooms = map->GetRooms();

	for (auto room : rooms) {
		if (room == nullptr) {
			debugf("Warning: Null room in map rooms.\n");
			continue;
		}
		if (room->IsStorage()) {
			if (room->GetStorage() != nullptr) {
				storages.push_back(room->GetStorage());
			}
			auto [type, capacity] = room->StorageConfig();
			if (!storageFactory->CheckRegistered(type)) {
				continue;
			}
			Storage* storage = storageFactory->CreateStorage(type);
			if (storage == nullptr) {
				debugf("Warning: Failed to create storage of type %s.\n", type.data());
				continue;
			}
			storage->SetVolume(capacity * room->GetAcreage());
			room->SetStorage(storage);
			storages.push_back(storage);
		}
		else if (room->IsManufacture()) {
			auto types = room->ManufactureTypes();
			for (const auto& type : types) {
				if (!manufactureFactory->CheckRegistered(type)) {
					continue;
				}
				Manufacture* manufacture =
					manufactureFactory->CreateManufacture(type);
				if (manufacture == nullptr) {
					debugf("Warning: Failed to create manufacture of type %s.\n", type.data());
					continue;
				}
				manufacture->CalculateTargets(productFactory);
				room->AddManufacture(manufacture);
				manufactures.push_back(manufacture);
			}
		}
	}

	unordered_map<string, vector<Storage*>> accepts;
	for (auto storage : storages) {
		if (storage == nullptr) {
			debugf("Warning: Null storage in storages.\n");
			continue;
		}
		if (!storage->GetAccept()) continue;
		for (auto category : storage->GetCategories()) {
			accepts[category].push_back(storage);
		}
	}
	unordered_map<string, vector<Storage*>> provides;
	for (auto storage : storages) {
		if (storage == nullptr) {
			continue;
		}
		if (!storage->GetProvide()) continue;
		for (auto category : storage->GetCategories()) {
			provides[category].push_back(storage);
		}
	}
	for (auto manufacture : manufactures) {
		if (manufacture == nullptr) {
			debugf("Warning: Null manufacture in manufactures.\n");
			continue;
		}
		auto ingredients = manufacture->GetIngredients();
		for (const auto& [product, amount] : ingredients) {
			auto it = provides.find(product);
			if (it == provides.end() || it->second.empty()) {
				continue;
			}
			auto& suppliers = it->second;
			int r = GetRandom((int)suppliers.size());
			suppliers[r]->ConnectDownstream(manufacture);
			manufacture->SetInput(product, suppliers[r], productFactory);
		}
		auto target = manufacture->GetTargets();
		for (const auto& [product, amount] : target) {
			auto it = accepts.find(product);
			if (it == accepts.end() || it->second.empty()) {
				continue;
			}
			auto& buyers = it->second;
			int r = GetRandom((int)buyers.size());
			buyers[r]->ConnectUpstream(manufacture);
			manufacture->SetOutput(product, buyers[r], productFactory);
		}
	}
}

void Industry::ReadConfigs(const string& path) const {
	string fullPath = resourcePath + path;
	if (!filesystem::exists(fullPath)) {
		THROW_EXCEPTION(IOException, "Path does not exist: " + fullPath + ".\n");
	}

	JsonReader reader;
	JsonValue root;

	ifstream fin(fullPath);
	if (!fin.is_open()) {
		THROW_EXCEPTION(IOException, "Failed to open file: " + fullPath + ".\n");
	}
	if (reader.Parse(fin, root)) {
		for (auto product : root["mods"]["product"]) {
			productFactory->SetConfig(product.AsString(), true);
		}
		for (auto storage : root["mods"]["storage"]) {
			storageFactory->SetConfig(storage.AsString(), true);
		}
		for (auto manufacture : root["mods"]["manufacture"]) {
			manufactureFactory->SetConfig(manufacture.AsString(), true);
		}
	}
	else {
		fin.close();
		THROW_EXCEPTION(JsonFormatException, "Json syntax error: " + reader.GetErrorMessages() + ".\n");
	}
	fin.close();
}

void Industry::Destroy() {

}

void Industry::Tick(int day, int hour, int min, int sec) {

}

void Industry::Print() const {

}

void Industry::Load(const string& path) {

}

void Industry::Save(const string& path) const {

}

void Industry::ApplyChange(Change* change, Story* story,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {

}

