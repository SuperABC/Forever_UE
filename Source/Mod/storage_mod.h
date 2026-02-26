#pragma once

#include "storage_base.h"

#include <memory>
#include <string>


// 模组检测子类
class ModStorage : public Storage {
public:
    static std::string GetId() { return "mod"; }
    virtual std::string GetType() const override { return "mod"; }
    virtual std::string GetName() const override { return "模组仓库"; }

    virtual std::vector<std::string> GetCategories() const override {
        return { "模组分类" };
    }

};

