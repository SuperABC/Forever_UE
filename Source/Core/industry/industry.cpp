#include "industry.h"

#include "map/map.h"
#include "map/room.h"
#include "industry/manufacture.h"
#include "industry/storage.h"
#include "industry/product.h"
#include "player/player.h"

#include <functional>
#include <unordered_set>


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

			auto registerFunc = reinterpret_cast<RegisterModProductsFunc>(GetProcAddress(modHandle, "RegisterModProducts"));
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

			auto registerFunc = reinterpret_cast<RegisterModStoragesFunc>(GetProcAddress(modHandle, "RegisterModStorages"));
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

			auto registerFunc = reinterpret_cast<RegisterModManufacturesFunc>(GetProcAddress(modHandle, "RegisterModManufactures"));
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

	// 对所有启用产品的ingredient/byproduct依赖图进行无环检测
	auto enabledProducts = Config::GetEnables("product");
	unordered_map<string, vector<string>> depGraph;
	for (auto& productId : enabledProducts) {
		Product tmp(productFactory, productId);
		for (auto& [ingredient, _] : tmp.GetIngredients()) {
			depGraph[productId].push_back(ingredient);
		}
		for (auto& [byproduct, _] : tmp.GetByproducts()) {
			depGraph[productId].push_back(byproduct);
		}
	}
	unordered_set<string> visited, inStack;
	function<void(const string&)> dfs = [&](const string& node) {
		if (inStack.count(node)) {
			THROW_EXCEPTION(RuntimeException, "Product dependency cycle detected at: " + node + ".\n");
		}
		if (visited.count(node)) return;
		visited.insert(node);
		inStack.insert(node);
		for (auto& dep : depGraph[node]) {
			if (depGraph.count(dep)) {
				dfs(dep);
			}
		}
		inStack.erase(node);
	};
	for (auto& productId : enabledProducts) {
		dfs(productId);
	}

	// 根据所有房间配置创建仓库和工坊
	auto rooms = map->GetRooms();
	for (auto room : rooms) {
		if (room == nullptr) {
			debugf("Warning: Null room in map rooms.\n");
			continue;
		}
		if (room->IsStorage()) {
			for (auto& [type, _] : room->StorageConfig()) {
				auto storage = new Storage(storageFactory, type);
				storage->SetProperty(room->GetAcreage());
				room->AddStorage(storage);
				storage->SetRoom(room);
				storages.push_back(storage);
			}
		}
		else if (room->IsManufacture()) {
			for (const auto& type : room->ManufactureTypes()) {
				auto manufacture = new Manufacture(manufactureFactory, type);
				manufacture->SetProperty(room);
				room->AddManufacture(manufacture);
				manufactures.push_back(manufacture);
			}
		}
	}

	// 连接仓库和工坊上下游
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

	// 第一阶段：连接下游，将每个工坊一天的产出注入选定仓库
	for (auto manufacture : manufactures) {
		auto manufactureRoom = manufacture->GetRoom();
		auto [centerX, centerY] = manufactureRoom
			? manufactureRoom->GetPosition(manufactureRoom->GetSizeX() / 2, manufactureRoom->GetSizeY() / 2)
			: make_pair(0.f, 0.f);
		auto outputCache = manufacture->GetOutput();
		for (auto& [type, output] : outputCache->GetProducts()) {
			float amount = output->GetAmount();
			unordered_set<Storage*> seen;
			vector<Storage*> candidates;
			for (auto category : output->GetCategories()) {
				auto it = accepts.find(category);
				if (it == accepts.end()) continue;
				for (auto storage : it->second) {
					if (seen.insert(storage).second) candidates.push_back(storage);
				}
			}
			if (candidates.empty()) continue;
			Storage* best = nullptr;
			float bestScore = -1.f;
			for (auto storage : candidates) {
				if (storage->GetSpace() < amount) continue;
				auto storageRoom = storage->GetRoom();
				float distanceSquared = 0.f;
				if (manufactureRoom && storageRoom) {
					auto [storageCenterX, storageCenterY] = storageRoom->GetPosition(storageRoom->GetSizeX() / 2, storageRoom->GetSizeY() / 2);
					float diffX = centerX - storageCenterX, diffY = centerY - storageCenterY;
					distanceSquared = diffX * diffX + diffY * diffY;
				}
				float score = storage->GetSpace() / (distanceSquared + 1.f);
				if (score > bestScore) { bestScore = score; best = storage; }
			}
			if (!best) {
				float maxSpace = -1.f;
				for (auto storage : candidates) {
					if (storage->GetSpace() > maxSpace) { maxSpace = storage->GetSpace(); best = storage; }
				}
			}
			best->ConnectUpstream(type, outputCache);
			manufacture->ConnectDownstream(type, best);
			best->InputProduct(type, amount);
		}
	}

	// 第二阶段：连接上游，从选定仓库提取每个工坊一天的原材料消耗
	for (auto manufacture : manufactures) {
		auto manufactureRoom = manufacture->GetRoom();
		auto [centerX, centerY] = manufactureRoom
			? manufactureRoom->GetPosition(manufactureRoom->GetSizeX() / 2, manufactureRoom->GetSizeY() / 2)
			: make_pair(0.f, 0.f);
		auto inputCache = manufacture->GetInput();
		for (auto& [type, input] : inputCache->GetProducts()) {
			float amount = input->GetAmount();
			unordered_set<Storage*> seen;
			vector<Storage*> candidates;
			for (auto category : input->GetCategories()) {
				auto it = provides.find(category);
				if (it == provides.end()) continue;
				for (auto storage : it->second) {
					if (seen.insert(storage).second) candidates.push_back(storage);
				}
			}
			if (candidates.empty()) continue;
			Storage* best = nullptr;
			float bestScore = -1.f;
			for (auto storage : candidates) {
				auto product = storage->GetProduct(type);
				float stock = product ? product->GetAmount() : 0.f;
				if (stock < amount) continue;
				auto storageRoom = storage->GetRoom();
				float distanceSquared = 0.f;
				if (manufactureRoom && storageRoom) {
					auto [storageCenterX, storageCenterY] = storageRoom->GetPosition(storageRoom->GetSizeX() / 2, storageRoom->GetSizeY() / 2);
					float diffX = centerX - storageCenterX, diffY = centerY - storageCenterY;
					distanceSquared = diffX * diffX + diffY * diffY;
				}
				float score = stock / (distanceSquared + 1.f);
				if (score > bestScore) { bestScore = score; best = storage; }
			}
			if (!best) {
				float maxStock = -1.f;
				for (auto storage : candidates) {
					auto product = storage->GetProduct(type);
					float stock = product ? product->GetAmount() : 0.f;
					if (stock > maxStock) { maxStock = stock; best = storage; }
				}
			}
			best->ConnectDownstream(type, inputCache);
			manufacture->ConnectUpstream(type, best);
			best->OutputProduct(type, amount);
		}
	}
}

void Industry::Destroy() {

}

void Industry::Tick(Player* player) {
	auto time = player->GetTime();
	auto cross = player->CrossDay();

	if (cross) {
		for (auto manufacture : manufactures) {
			manufacture->WorkAccount();
		}
		for (auto manufacture : manufactures) {
			manufacture->InitDelivery();
		}
		for (auto manufacture : manufactures) {
			manufacture->StartProduce();
		}
	}
}

vector<Event*> Industry::ApplyChange(Change* change,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	vector<Event*> result;
	return result;
}
