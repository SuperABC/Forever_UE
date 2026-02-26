#include "manufacture_base.h"

#include <unordered_set>


using namespace std;

unordered_map<string, unordered_map<string, float>> Manufacture::ingredientsCache = {};
unordered_map<string, unordered_map<string, float>> Manufacture::byproductsCache = {};

void Manufacture::CalculateTargets(ProductFactory* factory) {
    auto originTargets = this->ProductTargets();

    if (originTargets.empty()) {
        ingredients.clear();
        byproducts.clear();
        targets.clear();
        return;
    }

    unordered_set<string> targetSet;
    for (auto& p : originTargets) {
        targetSet.insert(p.first);
    }

    for (const auto& p : originTargets) {
        const string& id = p.first;
        Product* product = factory->CreateProduct(id);
        if (!product) {
            THROW_EXCEPTION(InvalidConfigException, "Product not available or not enabled: " + id);
        }
        ingredientsCache[id] = product->GetIngredients();
        byproductsCache[id] = product->GetByproducts();
    }

    unordered_map<string, vector<string>> graph;
    for (const auto& p : originTargets) {
        const string& id = p.first;
        auto it = ingredientsCache.find(id);
        if (it != ingredientsCache.end()) {
            for (auto& ing : it->second) {
                if (targetSet.find(ing.first) != targetSet.end()) {
                    graph[id].push_back(ing.first);
                }
            }
        }
    }
    unordered_map<string, int> state;
    function<bool(const string&)> dfs = [&](const string& node) -> bool {
        state[node] = 1;
        for (auto& neighbor : graph[node]) {
            if (state[neighbor] == 1) return true;
            if (state[neighbor] == 0 && dfs(neighbor)) return true;
        }
        state[node] = 2;
        return false;
        };

    for (auto& node : targetSet) {
        if (state[node] == 0) {
            if (dfs(node)) {
				THROW_EXCEPTION(InvalidConfigException, "Cyclic dependency detected among target products.");
            }
        }
    }

    // 4. 迭代计算
    unordered_map<string, float> demands = originTargets;
    unordered_map<string, float> productions;
    unordered_map<string, float> sides;
    unordered_map<string, float> requirements;
    const int MAX_DEPTH = 10000;
    int iter = 0;

    while (!demands.empty() && iter < MAX_DEPTH) {
        auto it = demands.begin();
        string id = it->first;
        float need = it->second;
        demands.erase(it);

        auto side = sides.find(id);
        if (side != sides.end()) {
            float take = min(need, side->second);
            need -= take;
            side->second -= take;
            if (fabs(side->second) < 1e-9) {
                sides.erase(side);
            }
        }

        if (need < 1e-9) continue;

        if (targetSet.find(id) == targetSet.end()) {
            requirements[id] += need;
        }
        else {
            productions[id] += need;
            auto ing_it = ingredientsCache.find(id);
            if (ing_it != ingredientsCache.end()) {
                for (auto& ing : ing_it->second) {
                    demands[ing.first] += ing.second * need;
                }
            }
            auto byp_it = byproductsCache.find(id);
            if (byp_it != byproductsCache.end()) {
                for (auto& byp : byp_it->second) {
                    sides[byp.first] += byp.second * need;
                }
            }
        }
        ++iter;
    }

    if (iter >= MAX_DEPTH) {
        THROW_EXCEPTION(InvalidConfigException, "Maximum iteration depth reached, possible cyclic dependency.");
    }

    unordered_map<string, float> byproducts_result;
    for (auto& p : sides) {
        if (targetSet.find(p.first) != targetSet.end()) {
            productions[p.first] += p.second;
        }
        else {
            byproducts_result[p.first] = p.second;
        }
    }

    ingredients = requirements;
    targets = productions;
    byproducts = byproducts_result;
}

void Manufacture::SetInput(string product, Storage* input, ProductFactory* factory) {
	input->AddOutcome(product, 0.f, factory);
	inputs[product] = input;
}

unordered_map<string, Storage*> Manufacture::GetInputs() const {
    return inputs;
}

void Manufacture::SetOutput(string product, Storage* output, ProductFactory* factory) {
    output->AddIncome(product, 0.f, factory);
	outputs[product] = output;
}

unordered_map<string, Storage*> Manufacture::GetOutputs() const {
    return outputs;
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

void ManufactureFactory::RegisterManufacture(const string& id, function<Manufacture* ()> creator) {
    registries[id] = creator;
}

Manufacture* ManufactureFactory::CreateManufacture(const string& id) {
    if (configs.find(id) == configs.end() || !configs.find(id)->second)return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second();
    }
    return nullptr;
}

bool ManufactureFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void ManufactureFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}
