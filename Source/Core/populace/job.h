#pragma once

#include "job_base.h"


// 子类注册函数
typedef void (*RegisterModJobsFunc)(JobFactory* factory);

// 默认职业
class DefaultJob : public Job {
public:
    DefaultJob();
    virtual ~DefaultJob();

    static std::string GetId();
    std::string GetType() const override;
    std::string GetName() const override;

    std::vector<std::string> GetScripts() const override;
};