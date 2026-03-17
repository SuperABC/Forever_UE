#pragma once

#include "../common/error.h"
#include "../common/utility.h"

#include "job_base.h"

#include <memory>
#include <string>


// 模组检测子类
class ModJob : public Job {
public:
    ModJob() {}
    virtual ~ModJob() {}

    static std::string GetId() { return "mod"; }
    std::string GetType() const override { return "mod"; }
    std::string GetName() const override { return "模组职业"; }

    std::vector<std::string> GetScripts() const override {
        return { "mod" };
    }
};