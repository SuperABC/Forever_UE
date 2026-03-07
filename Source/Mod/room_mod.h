#pragma once

#include "room_base.h"

#include <memory>
#include <string>


// 模组检测子类
class ModRoom : public Room {
public:
    ModRoom() : Room() {}
    virtual ~ModRoom() {}

    static std::string GetId() { return "mod"; }
    virtual std::string GetType() const override { return "mod"; }
    virtual std::string GetName() const override { return "模组房间"; }

    virtual bool IsResidential() const override { return false; };
    virtual bool IsWorkspace() const override { return false; }
    virtual bool IsStorage() const override { return false; };
    virtual bool IsManufacture() const override { return false; };
    virtual int ResidentialCapacity() const override { return 0; }
    virtual int WorkspaceCapacity() const override { return 0; }
	virtual std::pair<std::string, float> StorageConfig() const override { return { "", 0.f }; }
    virtual std::vector< std::string> ManufactureTypes() const override { return { "" }; }
};

