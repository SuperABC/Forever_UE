#include "manufacture.h"

#include "industry/industry.h"
#include "industry/storage.h"
#include "industry/product.h"

#include <functional>
#include <unordered_set>


using namespace std;

Manufacture::Manufacture(ManufactureFactory* factory, const string& manufacture) :
	mod(factory->CreateManufacture(manufacture)),
	factory(factory),
	type(),
	name(),
	inputCache(nullptr),
	outputCache(nullptr),
	ingredients(),
	targets(),
	byproducts(),
	currentWorkload(0.f) {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Manufacture " + manufacture + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Manufacture::~Manufacture() {
	factory->DestroyManufacture(mod);

	if (inputCache)delete inputCache;
	if (outputCache)delete outputCache;
}

string Manufacture::GetType() const {
	return type;
}

string Manufacture::GetName() const {
	return name;
}

void Manufacture::SetProperty(Room* room) {
	mod->SetTargets();

	if (mod->targets.empty()) {
		ingredients.clear();
		byproducts.clear();
		targets.clear();
		return;
	}

	unordered_set<string> targetSet;
	for (auto& [t, _] : mod->targets) targetSet.insert(t);

	// Step 1: 按原始比例计算所有target产生的副产物总量
	unordered_map<string, float> allByproducts;
	for (auto& [target, scalar] : mod->targets) {
		Product tmp(Industry::productFactory, target);
		for (auto& [bp, rate] : tmp.GetByproducts()) {
			allByproducts[bp] += rate * scalar;
		}
	}

	// Step 2: 副产物覆盖的target减少主动生产量
	targets.clear();
	for (auto& [target, scalar] : mod->targets) {
		float bpCoverage = allByproducts.count(target) ? allByproducts.at(target) : 0.f;
		float active = max(0.f, scalar - bpCoverage);
		if (active > 0.f) {
			targets[target] = active;
		}
	}

	// Step 3: 递归展开配方，将target-as-ingredient替换为其原材料
	unordered_map<string, float> rawIngredients;
	function<void(const string&, float)> expand = [&](const string& productType, float amount) {
		Product tmp(Industry::productFactory, productType);
		for (auto& [ingredient, rate] : tmp.GetIngredients()) {
			if (targetSet.count(ingredient)) {
				expand(ingredient, rate * amount);
			}
			else {
				rawIngredients[ingredient] += rate * amount;
			}
		}
	};
	for (auto& [target, active] : targets) {
		expand(target, active);
	}

	// Step 4: 用副产物抵消对应原材料的外部需求
	ingredients.clear();
	for (auto& [ingredient, needed] : rawIngredients) {
		float bpAvailable = allByproducts.count(ingredient) ? allByproducts.at(ingredient) : 0.f;
		float actual = max(0.f, needed - bpAvailable);
		if (actual > 0.f) {
			ingredients[ingredient] = actual;
		}
	}

	byproducts = allByproducts;

	// 合并输出产品表：主动产量 + 副产物
	unordered_map<string, float> outputProducts;
	for (auto& [target, amount] : targets) {
		outputProducts[target] += amount;
	}
	for (auto& [byproduct, amount] : byproducts) {
		outputProducts[byproduct] += amount;
	}

	// 初始化缓存仓库
	inputCache = new Storage(Industry::storageFactory, "empty");
	inputCache->SetProperty(0.f);
	inputCache->SetRoom(room);
	outputCache = new Storage(Industry::storageFactory, "empty");
	outputCache->SetProperty(0.f);
	outputCache->SetRoom(room);

	float inputSize = 0.f, outputSize = 0.f;

	for (auto& [ingredient, amount] : ingredients) {
		inputSize += amount;
		inputCache->InputProduct(ingredient, 0.f);
	}
	for (auto& [product, total] : outputProducts) {
		outputSize += total;
		outputCache->InputProduct(product, 0.f);
	}

	inputCache->SetVolume(inputSize);
	outputCache->SetVolume(outputSize);

	for (auto& [ingredient, amount] : ingredients) {
		inputCache->InputProduct(ingredient, amount);
	}
	for (auto& [product, total] : outputProducts) {
		outputCache->InputProduct(product, total);
	}
}

Storage* Manufacture::GetInput() const {
	return inputCache;
}

Storage* Manufacture::GetOutput() const {
	return outputCache;
}

unordered_map<string, Storage*> Manufacture::GetUpstreams() const {
	return inputCache->GetUpstreams();
}

void Manufacture::ConnectUpstream(string type, Storage* storage) {
	inputCache->ConnectUpstream(type, storage);
}

unordered_map<string, Storage*> Manufacture::GetDownstreams() const {
	return outputCache->GetDownstreams();
}

void Manufacture::ConnectDownstream(string type, Storage* storage) {
	outputCache->ConnectDownstream(type, storage);
}

unordered_map<string, float> Manufacture::GetIngredients() const {
	return ingredients;
}

unordered_map<string, float> Manufacture::GetTargets() const {
	return targets;
}

unordered_map<string, float> Manufacture::GetByproducts() const {
	return byproducts;
}

void Manufacture::WorkAccount() {
	for (auto& [type, amount] : targets) {
		auto product = outputCache->GetProduct(type);
		if (!product)
			THROW_EXCEPTION(NullPointerException, "WorkAccount: product " + type + " not found in outputCache.\n");
		product->IncreaseAmount(amount * currentWorkload);
	}
	for (auto& [type, amount] : byproducts) {
		auto product = outputCache->GetProduct(type);
		if (!product)
			THROW_EXCEPTION(NullPointerException, "WorkAccount: byproduct " + type + " not found in outputCache.\n");
		product->IncreaseAmount(amount * currentWorkload);
	}
}

void Manufacture::InitDelivery() {
	for (auto& [output, storage] : outputCache->GetDownstreams()) {
		auto product = outputCache->GetProduct(output);
		if (!product) continue;
		float available = product->GetAmount();
		float space = max(storage->GetSpace(), 0.f);
		float actual = min(available, space);
		float out = outputCache->OutputProduct(output, actual);
		storage->InputProduct(output, out);
	}

	for (auto& [input, storage] : inputCache->GetUpstreams()) {
		auto product = storage->GetProduct(input);
		if (!product) continue;
		float available = product->GetAmount();
		float need = min(ingredients[input], max(inputCache->GetSpace(), 0.f));
		float actual = min(available, need);
		float out = storage->OutputProduct(input, actual);
		inputCache->InputProduct(input, out);
	}
}

void Manufacture::StartProduce() {
	float efficiency = 1.f;

	for (auto& [type, standard] : ingredients) {
		auto input = inputCache->GetProduct(type)->GetAmount();
		if (input < standard && efficiency > input / standard) {
			efficiency = input / standard;
		}
	}

	float capacity = outputCache->GetSpace();
	float standard = 0.f;
	for (auto& [_, amount] : targets) {
		standard += amount;
	}
	for (auto& [_, amount] : byproducts) {
		standard += amount;
	}
	if (capacity < standard && efficiency > capacity / standard) {
		efficiency = capacity / standard;
	}
	currentWorkload = efficiency;

	for (auto& [type, standard] : ingredients) {
		inputCache->GetProduct(type)->DecreaseAmount(standard * efficiency);
	}
}

int EmptyManufacture::count = 0;

EmptyManufacture::EmptyManufacture() : id(count++) {

}

EmptyManufacture::~EmptyManufacture() {

}

const char* EmptyManufacture::GetId() {
	return "empty";
}

const char* EmptyManufacture::GetType() const {
	return "empty";
}

const char* EmptyManufacture::GetName() {
	name = "空工坊" + to_string(id);
	return name.data();
}

void EmptyManufacture::SetTargets() {

}


