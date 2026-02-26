#pragma once

#include "storage_base.h"

#include <string>
#include <vector>


// 子类注册函数
typedef void (*RegisterModStoragesFunc)(StorageFactory* factory);

// 标准双休日程
class DefaultStorage : public Storage {
public:
    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    virtual std::vector<std::string> GetCategories() const override;
};