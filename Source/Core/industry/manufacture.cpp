#include "manufacture.h"

#include "industry/industry.h"
#include "industry/storage.h"
#include "industry/product.h"

#include <unordered_set>


using namespace std;

Manufacture::Manufacture(ManufactureFactory* factory, const string& manufacture) :
	mod(factory->CreateManufacture(manufacture)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	inputCache(nullptr),
	outputCache(nullptr),
	ingredients(),
	targets(),
	byproducts(),
	currentWorkload(0.f) {

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

	inputCache = new Storage(Industry::storageFactory, "empty");
	inputCache->SetProperty(0.f);
	inputCache->SetRoom(room);
	outputCache = new Storage(Industry::storageFactory, "empty");
	outputCache->SetProperty(0.f);
	outputCache->SetRoom(room);

	targets = mod->targets;
	float inputSize = 0.f, outputSize = 0.f;
	unordered_map<string, float> inputAmount, outputAmount;
	for (auto& [target, scalar] : mod->targets) {
		outputSize += scalar;
		outputCache->InputProduct(target, 0.f);
		outputAmount[target] += scalar;
		Product tmp(Industry::productFactory, target);
		for (auto& [ingredient, amount] : tmp.GetIngredients()) {
			inputSize += amount * scalar;
			inputCache->InputProduct(target, 0.f);
			inputAmount[ingredient] += amount * scalar;
			ingredients[ingredient] += amount * scalar;
		}
		for (auto& [byproduct, amount] : tmp.GetByproducts()) {
			outputSize += amount * scalar;
			outputCache->InputProduct(target, 0.f);
			outputAmount[byproduct] += amount * scalar;
			byproducts[byproduct] += amount * scalar;
		}
	}

	inputCache->SetVolume(inputSize);
	outputCache->SetVolume(outputSize);
	for (auto [input, amount] : inputAmount) {
		inputCache->InputProduct(input, amount);
	}
	for (auto [output, amount] : outputAmount) {
		outputCache->InputProduct(output, amount);
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
		outputCache->GetProduct(type)->IncreaseAmount(amount * currentWorkload);
	}
	for (auto& [type, amount] : byproducts) {
		outputCache->GetProduct(type)->IncreaseAmount(amount * currentWorkload);
	}
}

void Manufacture::InitDelivery() {
	for (auto [output, storage] : outputCache->GetDownstreams()) {
		float amount = outputCache->GetProduct(output)->GetAmount();
		outputCache->OutputProduct(output, amount);
		storage->InputProduct(output, amount);
	}

	for (auto [input, storage] : inputCache->GetUpstreams()) {
		float amount = min(storage->GetProduct(input)->GetAmount(), ingredients[input]);
		storage->OutputProduct(input, amount);
		inputCache->InputProduct(input, amount);
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
	for (auto& [type, amount] : targets) {
		standard += amount;
	}
	for (auto& [type, amount] : byproducts) {
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


