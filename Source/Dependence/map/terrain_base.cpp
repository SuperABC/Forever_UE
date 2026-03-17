#include "../common/error.h"
#include "../common/utility.h"

#include "terrain_base.h"


using namespace std;

// 全成员默认构造
Terrain::Terrain() :
    dx{ -1, 1, 0, 0 },
    dy{ 0, 0, -1, 1 } {

}

// 无析构
Terrain::~Terrain() {

}

// 地形填充，若ovewrite为true，则全图填充，否则只填充平原
int Terrain::FloodTerrain(
    int x, int y, int num, bool overwrite, int width, int height,
    function<bool(int, int, const string&, float)> set,
    function<string(int, int)> get) const {
    vector<pair<int, int>> q;

    if (x < 0 || x >= width - 1 || y < 0 || y >= height - 1)
        return 0;
    if (get(x, y) == GetType())
        return 0;

    set(x, y, GetType(), 0.f);
    int count = 1;

    for (int i = 0; i < 4; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
            q.push_back(make_pair(nx, ny));
        }
    }

    while (count < num && !q.empty()) {
        int idx = GetRandom((int)q.size());
        auto [cx, cy] = q[idx];
        q[idx] = q.back();
        q.pop_back();

        set(cx, cy, GetType(), 0.f);
        ++count;

        UpdateBoundary(cx, cy, q, overwrite, width, height, set, get);
    }

    return count;
}

// 检查地形填充处是否为当前边界
bool Terrain::CheckBoundary(
    int x, int y, bool overwrite, int width, int height,
    function<bool(int, int, const string&, float)> set,
    function<string(int, int)> get) const {
    if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
        return true;
    for (int i = 0; i < 4; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (overwrite) {
            if (get(nx, ny) != GetType())
                return true;
        }
        else {
            if (get(nx, ny) == "plain")
                return true;
        }
    }
    return false;
}

// 更新地形填充边界
void Terrain::UpdateBoundary(
    int x, int y, vector<pair<int, int>>& q, bool overwrite, int width, int height,
    function<bool(int, int, const string&, float)> set,
    function<string(int, int)> get) const {
    for (int i = 0; i < 4; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
            CheckBoundary(nx, ny, overwrite, width, height, set, get)) {
            bool overlap = false;
            for (auto [bx, by] : q) {
                if (bx == nx && by == ny) {
                    overlap = true;
                    break;
                }
            }
            if (!overlap)
                q.push_back(make_pair(nx, ny));
        }
    }
}

// 地形滤波
void Terrain::ShapeFilter(int x, int y, int width, int height,
    function<bool(int, int, const string&, float)> set,
    function<string(int, int)> get, int side, float threshold) const {
    if (get(x, y) == GetType())
        return;

    int count = 0;
    int total = (side * 2 + 1) * (side * 2 + 1);
    int x_start = max(0, x - side);
    int x_end = min(width - 1, x + side);
    int y_start = max(0, y - side);
    int y_end = min(height - 1, y + side);

    for (int i = x_start; i <= x_end; ++i) {
        for (int j = y_start; j <= y_end; ++j) {
            if (get(i, j) == GetType())
                ++count;
        }
    }
    // 实际统计的区域可能小于全窗口，按比例计算阈值
    int actual_total = (x_end - x_start + 1) * (y_end - y_start + 1);
    if (count > actual_total * threshold)
        set(x, y, GetType(), 0.f);
}

// 清空注册
void TerrainFactory::RemoveAll() {
    for(auto &config : configs) {
        config.second = false;
    }
}

// 注册地形
void TerrainFactory::RegisterTerrain(const string& id,
    function<Terrain* ()> creator, function<void(Terrain*)> deleter) {
    registries[id] = { creator, deleter };
}

// 创建地形
Terrain* TerrainFactory::CreateTerrain(const string& id) const {
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Terrain %s not enabled when creating.\n", id.data());
        return nullptr;
    }

    auto it = registries.find(id);
    if(it == registries.end()) {
        debugf("Warning: Terrain %s not registered when creating.\n", id.data());
        return nullptr;
    }
    
    if(it->second.first){
        return it->second.first();
    }
    else {
        THROW_EXCEPTION(NullPointerException, "Terrain " + id + " creater is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool TerrainFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void TerrainFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 获取所有启用地形
vector<Terrain*> TerrainFactory::GetTerrains() const {
    vector<Terrain*> terrains;
    for (auto& registry : registries) {
        if (configs.find(registry.first)->second) {
            terrains.push_back(registry.second.first());
        }
    }
    return terrains;
}

// 析构地形
void TerrainFactory::DestroyTerrains(const vector<Terrain*>& terrains) const {
    for(auto terrain : terrains){
        if (!terrain) {
            debugf("Warning: Terrain is null when deleting.\n");
            return;
        }

        auto it = registries.find(terrain->GetType());
        if (it == registries.end()) {
            debugf("Warning: Terrain %s not registered when deleting.\n", terrain->GetType().data());
            continue;
        }

        if(it->second.second){
            it->second.second(terrain);
        }
        else {
            THROW_EXCEPTION(NullPointerException, "Terrain " + terrain->GetType() + " deleter is null.\n");
        }
    }
}

