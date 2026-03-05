#pragma once

#include "../glm/glm.hpp"
#include "../common/error.h"
#include "../map/room_base.h"

#include <string>
#include <functional>
#include <memory>


class Station : public Room {
public:
    Station() = default;
    virtual ~Station() = default;

    // 子类实现方法

    // 动态返回组合静态信息
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 父类实现方法

	// 获取/设置属性
	void AddSlot(const glm::vec3 position, const glm::vec3 direction);
	std::pair<glm::vec3, glm::vec3> GetSlot(int index) const;
    std::vector<std::pair<glm::vec3, glm::vec3>> GetSlots() const;

protected:
    std::vector<std::pair<glm::vec3, glm::vec3>> slots;
};

class StationFactory {
public:
    void RegisterStation(const std::string& id,
        std::function<Station*()> creator, std::function<void(Station*)> deleter);
    Station* CreateStation(const std::string& id);
    bool CheckRegistered(const std::string& id);
    void SetConfig(std::string name, bool config);
    void DestroyStation(Station* station) const;

private:
    std::unordered_map<std::string, std::pair<std::function<Station*()>, std::function<void(Station*)>>> registries;
    std::unordered_map<std::string, bool> configs;
};

