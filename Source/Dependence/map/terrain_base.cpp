#include "../common/error.h"
#include "../common/utility.h"

#include "terrain_base.h"


using namespace std;

Terrain::Terrain() :
    dx{ -1, 1, 0, 0 },
    dy{ 0, 0, -1, 1 } {
    // 全成员默认构造

}

Terrain::~Terrain() {

}

int Terrain::FloodTerrain(
    int x, int y, int num, bool overwrite, int width, int height,
    function<bool(int, int, const string&, float)> set,
    function<string(int, int)> get) const {
    // 地形填充，若ovewrite为true，则全图填充，否则只填充平原
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

bool Terrain::CheckBoundary(
    int x, int y, bool overwrite, int width, int height,
    function<bool(int, int, const string&, float)> set,
    function<string(int, int)> get) const {
    // 检查地形填充处是否为当前边界
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

void Terrain::UpdateBoundary(
    int x, int y, vector<pair<int, int>>& q, bool overwrite, int width, int height,
    function<bool(int, int, const string&, float)> set,
    // 更新地形填充边界
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

void Terrain::ShapeFilter(int x, int y, int width, int height,
    function<bool(int, int, const string&, float)> set,
    function<string(int, int)> get, int side, float threshold) const {
    // 地形滤波
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

void TerrainFactory::RegisterTerrain(const string& id,
    function<Terrain* ()> creator, function<void(Terrain*)> deleter) {
    // 注册构造器和析构器
    registries[id] = { creator, deleter };
}

Terrain* TerrainFactory::CreateTerrain(const string& id) const {
    // 根据配置构造园区
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        return nullptr;
    }

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first();
    }
    return nullptr;
}

bool TerrainFactory::CheckRegistered(const string& id) const {
    // 检查是否注册
    return registries.find(id) != registries.end();
}

void TerrainFactory::SetConfig(const string& name, bool config) {
    // 设置配置
    configs[name] = config;
}

vector<Terrain*> TerrainFactory::GetTerrains() const {
    // 获取所有启用地形
    vector<Terrain*> terrains;
    for (auto& r : registries) {
        if (configs.find(r.first)->second) {
            terrains.push_back(r.second.first());
        }
    }
    return terrains;
}

void TerrainFactory::DestroyTerrains(const vector<Terrain*>& terrains) const {
    // 析构地形（包含delete操作）
    for(auto terrain : terrains){
        if(!terrain)continue;
        auto it = registries.find(terrain->GetType());
        if (it != registries.end()) {
            return it->second.second(terrain);
        }
        else{
            debugf(("Deleter not found for " + terrain->GetType() + ".\n").data());
        }
    }
}