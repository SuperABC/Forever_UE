#include "manufacture.h"

#include "industry/industry.h"

#include <unordered_set>


using namespace std;

Manufacture::Manufacture(ManufactureFactory* factory, const string& component) :
	mod(factory->CreateManufacture(component)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	inputs(),
	outputs(),
	ingredients(),
	targets(),
	byproducts(),
	currentWorkload(0.f) {

}

Manufacture::~Manufacture() {
	factory->DestroyManufacture(mod);

    for (auto& [type, storage] : inputs) {
        if (storage)delete storage;
        storage = nullptr;
    }
    inputs.clear();
    for (auto& [type, storage] : outputs) {
        if (storage)delete storage;
        storage = nullptr;
    }
    outputs.clear();
}

string Manufacture::GetType() const {
	return type;
}

string Manufacture::GetName() const {
	return name;
}

void Manufacture::CalculateTargets() {
    mod->SetTargets();

    if (mod->targets.empty()) {
        ingredients.clear();
        byproducts.clear();
        targets.clear();
        return;
    }

    targets = mod->targets;
    for (auto& [target, scalar] : mod->targets) {
        Product tmp(Industry::productFactory, target);
        for (auto& [ingredient, amount] : tmp.GetIngredients()) {
            ingredients[ingredient] += amount * scalar;
        }
        for (auto& [byproduct, amount] : tmp.GetByproducts()) {
            byproducts[byproduct] += amount * scalar;
        }
    }
}

unordered_map<string, Storage*> Manufacture::GetInputs() const {
    return inputs;
}

void Manufacture::SetInput(const string& product, Storage* input) {
    input->InputProduct(product, 0.f);
    inputs[product] = input;
}

unordered_map<string, Storage*> Manufacture::GetOutputs() const {
    return outputs;
}

void Manufacture::SetOutput(const string& product, Storage* output) {
    output->InputProduct(product, 0.f);
    outputs[product] = output;
}

unordered_map<string, Storage*> Manufacture::GetUpstreams() const {
    unordered_map<string, Storage*> upstreams;
    for (auto& [type, storage] : inputs) {
        upstreams[type] = storage->GetUpstreams()[type];
    }
    return upstreams;
}

void Manufacture::ConnectUpstream(string type, Storage* storage) {
    inputs[type]->ConnectUpstream(type, storage);
}

unordered_map<string, Storage*> Manufacture::GetDownstreams() const {
    unordered_map<string, Storage*> downstreams;
    for (auto& [type, storage] : inputs) {
        downstreams[type] = storage->GetDownstreams()[type];
    }
    return downstreams;
}

void Manufacture::ConnectDownstream(string type, Storage* storage) {
    outputs[type]->ConnectUpstream(type, storage);
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

void Manufacture::StartProduce() {
    float efficiency = 1.f;
    for (auto& [type, standard] : ingredients) {
        auto input = inputs[type]->GetProduct(type)->GetAmount();
        if (input < standard && efficiency > input / standard) {
            efficiency = input / standard;
        }
    }
    unordered_map<Storage*, float> capacities;
    unordered_map<Storage*, float> standards;
    for (auto& [type, standard] : targets) {
        capacities[outputs[type]] = outputs[type]->GetSpace();
    }
    for (auto& [type, standard] : byproducts) {
        capacities[outputs[type]] = outputs[type]->GetSpace();
    }
    for (auto& [type, standard] : targets) {
        auto output = outputs[type]->GetProduct(type)->GetAmount();
        standards[outputs[type]] += output;
    }
    for (auto& [type, standard] : byproducts) {
        auto output = outputs[type]->GetProduct(type)->GetAmount();
        standards[outputs[type]] += output;
    }
    for (auto& [storage, standard] : standards) {
        auto capacity = capacities[storage];
        if (capacity < standard && efficiency > capacity / standard) {
            efficiency = capacity / standard;
        }
    }
    currentWorkload = efficiency;
}

void Manufacture::FinishProduce() {

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


