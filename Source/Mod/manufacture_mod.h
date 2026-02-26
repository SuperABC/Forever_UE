#pragma once

#include "manufacture_base.h"

#include <memory>
#include <string>


// 模组检测子类
class ModManufacture : public Manufacture {
public:
    static std::string GetId() { return "mod"; }
    virtual std::string GetType() const override { return "mod"; }
    virtual std::string GetName() const override { return "模组生产线"; }

    virtual std::unordered_map<std::string, float> ProductTargets() const override {
        return { {"mod", 1.f} };
    }

};

