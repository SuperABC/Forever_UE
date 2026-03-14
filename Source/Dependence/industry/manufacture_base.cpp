#include "../common/error.h"
#include "../common/utility.h"

#include "manufacture_base.h"

#include <unordered_set>


using namespace std;

unordered_map<string, unordered_map<string, float>> Manufacture::ingredientsCache = {};
unordered_map<string, unordered_map<string, float>> Manufacture::byproductsCache = {};

// 全成员默认构造
Manufacture::Manufacture() :
    inputs(),
    outputs(),
    currentInput(0.f),
    currentOutput(0.f),
    currentWorkload(0.f),
    ingredients(),
    targets(),
    byproducts() {

}

// inputs/outputs在industry中统一创建和析构
Manufacture::~Manufacture() {

}

// 计算全部输入输出
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
            THROW_EXCEPTION(RuntimeException, "Product not available or not enabled: " + id);
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
                THROW_EXCEPTION(RuntimeException, "Cyclic dependency detected among target products.");
            }
        }
    }

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
        THROW_EXCEPTION(RuntimeException, "Maximum iteration depth reached, possible cyclic dependency.");
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

// 获取输入
unordered_map<string, Storage*> Manufacture::GetInputs() const {
    return inputs;
}

// 设置输入
void Manufacture::SetInput(const string& product, Storage* input, ProductFactory* factory) {
    input->AddOutcome(product, 0.f, factory);
    inputs[product] = input;
}

// 获取输出
unordered_map<string, Storage*> Manufacture::GetOutputs() const {
    return outputs;
}

// 设置输出
void Manufacture::SetOutput(const string& product, Storage* output, ProductFactory* factory) {
    output->AddIncome(product, 0.f, factory);
    outputs[product] = output;
}

// 获取原料配比
unordered_map<string, float> Manufacture::GetIngredients() const {
    return ingredients;
}

// 获取产出配比
unordered_map<string, float> Manufacture::GetTargets() const {
    return targets;
}

// 获取副产物配比
unordered_map<string, float> Manufacture::GetByproducts() const {
    return byproducts;
}

// 注册工坊
void ManufactureFactory::RegisterManufacture(const string& id,
    function<Manufacture*()> creator, function<void(Manufacture*)> deleter) {
    registries[id] = {creator, deleter};
}

// 创建工坊
Manufacture* ManufactureFactory::CreateManufacture(const string& id) const {
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Manufacture %s not enabled when creating.\n", id.data());
        return nullptr;
    }

    auto it = registries.find(id);
    if (it == registries.end()) {
        debugf("Warning: Manufacture %s not registered when creating.\n", id.data());
        return nullptr;
    }

    if (it->second.first) {
        return it->second.first();
    } else {
        THROW_EXCEPTION(NullPointerException, "Manufacture " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool ManufactureFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void ManufactureFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 析构工坊
void ManufactureFactory::DestroyManufacture(Manufacture* manufacture) const {
    if (!manufacture) {
        debugf("Warning: Manufacture is null when deleting.\n");
        return;
    }

    auto it = registries.find(manufacture->GetType());
    if (it == registries.end()) {
        debugf("Warning: Manufacture  %s  not registered when deleting.\n", manufacture->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(manufacture);
    } else {
        THROW_EXCEPTION(NullPointerException, "Manufacture " + manufacture->GetType() + " deleter is null.\n");
    }
}

