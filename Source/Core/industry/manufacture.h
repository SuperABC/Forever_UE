#pragma once

#include "manufacture_base.h"

#include <string>
#include <vector>


// 子类注册函数
typedef void (*RegisterModManufacturesFunc)(ManufactureFactory* factory);

// 标准双休日程
class DefaultManufacture : public Manufacture {
public:
    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual std::unordered_map<std::string, float> ProductTargets() const override;
};