#pragma once

#include "../glm/glm.hpp"
#include "../map/room_base.h"

#include <string>
#include <functional>
#include <memory>


class Station : public Room {
public:
    Station();
    virtual ~Station();

    // 子类实现方法

    // 统一类型定义
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 父类实现方法

	// 获取某一车道
	std::pair<glm::vec3, glm::vec3> GetSlot(int index) const;
    
	// 获取全部车道
    std::vector<std::pair<glm::vec3, glm::vec3>> GetSlots() const;

	// 添加车道
	void AddSlot(const glm::vec3& position, const glm::vec3& direction);
    
private:
    std::vector<std::pair<glm::vec3, glm::vec3>> slots;
};

class StationFactory {
public:
    // 注册车站
    void RegisterStation(const std::string& id,
        std::function<Station*()> creator, std::function<void(Station*)> deleter);

    // 创建车站（包含new操作）
    Station* CreateStation(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 析构车站（包含delete操作）
    void DestroyStation(Station* station) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Station*()>, std::function<void(Station*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
};

