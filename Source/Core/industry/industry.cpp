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

Industry::Industry() {
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

}

void Industry::SetResourcePath(string path) {
	resourcePath = path;
}

void Industry::InitProducts(unordered_map<string, HMODULE>& modHandles) {
	productFactory->RegisterProduct(DefaultProduct::GetId(),
		[]() { return new DefaultProduct(); });

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
		debugf("Mod dll loaded successfully.\n");
		RegisterModProductsFunc registerFunc = (RegisterModProductsFunc)GetProcAddress(modHandle, "RegisterModProducts");
		if (registerFunc) {
			registerFunc(productFactory);
		}
		else {
			debugf("Incorrect dll content.\n");
		}
	}
	else {
		debugf("Failed to load mod dll.\n");
	}

#ifdef MOD_TEST
	auto productList = { "mod" };
	for (const auto& productId : productList) {
		if (productFactory->CheckRegistered(productId)) {
			auto product = productFactory->CreateProduct(productId);
			debugf("Created product: mod.\n");
			delete product;
		}
		else {
			debugf("Product not registered: %s.\n", productId);
		}
	}
#endif // MOD_TEST

}

void Industry::InitStorages(unordered_map<string, HMODULE>& modHandles) {
	storageFactory->RegisterStorage(DefaultStorage::GetId(),
		[]() { return new DefaultStorage(); });

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
		debugf("Mod dll loaded successfully.\n");
		RegisterModStoragesFunc registerFunc = (RegisterModStoragesFunc)GetProcAddress(modHandle, "RegisterModStorages");
		if (registerFunc) {
			registerFunc(storageFactory);
		}
		else {
			debugf("Incorrect dll content.\n");
		}
	}
	else {
		debugf("Failed to load mod dll.\n");
	}

#ifdef MOD_TEST
	auto storageList = { "mod" };
	for (const auto& storageId : storageList) {
		if (storageFactory->CheckRegistered(storageId)) {
			auto storage = storageFactory->CreateStorage(storageId);
			debugf("Created storage: mod.\n");
			delete storage;
		}
		else {
			debugf("Storage not registered: %s.\n", storageId);
		}
	}
#endif // MOD_TEST

}

void Industry::InitManufactures(unordered_map<string, HMODULE>& modHandles) {
	manufactureFactory->RegisterManufacture(DefaultManufacture::GetId(),
		[]() { return new DefaultManufacture(); });

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
		debugf("Mod dll loaded successfully.\n");
		RegisterModManufacturesFunc registerFunc = (RegisterModManufacturesFunc)GetProcAddress(modHandle, "RegisterModManufactures");
		if (registerFunc) {
			registerFunc(manufactureFactory);
		}
		else {
			debugf("Incorrect dll content.\n");
		}
	}
	else {
		debugf("Failed to load mod dll.\n");
	}

#ifdef MOD_TEST
	auto manufactureList = { "mod" };
	for (const auto& manufactureId : manufactureList) {
		if (manufactureFactory->CheckRegistered(manufactureId)) {
			auto manufacture = manufactureFactory->CreateManufacture(manufactureId);
			debugf("Created manufacture: mod.\n");
			delete manufacture;
		}
		else {
			debugf("Manufacture not registered: %s.\n", manufactureId);
		}
	}
#endif // MOD_TEST

}

void Industry::Init(Map* map) {
	auto rooms = map->GetRooms();

	for (auto room : rooms) {
		if (room->IsStorage()) {
			if (room->GetStorage()) {
				storages.push_back(room->GetStorage());
			}
			auto [type, capacity] = room->StorageConfig();
			if (!storageFactory->CheckRegistered(type))continue;
			Storage* storage = storageFactory->CreateStorage(type);
			storage->SetVolume(capacity * room->GetAcreage());
			room->SetStorage(storage);
			storages.push_back(storage);
		}
		else if (room->IsManufacture()) {
			auto types = room->ManufactureTypes();
			for (auto type : types) {
				if (!manufactureFactory->CheckRegistered(type))continue;
				Manufacture* manufacture = manufactureFactory->CreateManufacture(type);
				manufacture->CalculateTargets(productFactory);
				room->AddManufacture(manufacture);
				manufactures.push_back(manufacture);
			}
		}
	}

	unordered_map<string, vector<Storage*>> accepts;
	for(auto storage : storages) {
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
	for(auto manufacture : manufactures) {
		auto ingredients = manufacture->GetIngredients();
		for (auto [product, amount] : ingredients) {
			if (provides.find(product) == provides.end()) continue;
			auto suppliers = provides[product];
			int r = GetRandom((int)suppliers.size());
			suppliers[r]->ConnectDownstream(manufacture);
			manufacture->SetInput(product, suppliers[r], productFactory);
		}
		auto target = manufacture->GetTargets();
		for (auto [product, amount] : target) {
			if (accepts.find(product) == accepts.end()) continue;
			auto buyers = accepts[product];
			int r = GetRandom((int)buyers.size());
			buyers[r]->ConnectUpstream(manufacture);
			manufacture->SetOutput(product, buyers[r], productFactory);
		}
	}
}

void Industry::ReadConfigs(string path) const {
	path = resourcePath + path;
	if (!filesystem::exists(path)) {
		THROW_EXCEPTION(IOException, "Path does not exist: " + path + ".\n");
	}

	JsonReader reader;
	JsonValue root;

	ifstream fin(path);
	if (!fin.is_open()) {
		THROW_EXCEPTION(IOException, "Failed to open file: " + path + ".\n");
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
	// 每日产品分配
	// 每日生产
}

void Industry::Print() const {

}

void Industry::Load(string path) {

}

void Industry::Save(string path) const {

}

void Industry::ApplyChange(Change* change, Story* story,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {

}


