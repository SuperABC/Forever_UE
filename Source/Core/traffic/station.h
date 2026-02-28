#pragma once

#include "station_base.h"

#include <string>
#include <vector>


// 子类注册函数
typedef void (*RegisterModStationsFunc)(StationFactory* factory);

// 机场站台
class AirportStation : public Station {
public:
	static std::string GetId();
	virtual std::string GetType() const override;
	virtual std::string GetName() const override;

    virtual bool IsResidential() const override;
    virtual bool IsWorkspace() const override;
    virtual bool IsStorage() const override;
    virtual bool IsManufacture() const override;
    virtual int ResidentialCapacity() const override;
    virtual int WorkspaceCapacity() const override;
    virtual std::pair<std::string, float> StorageConfig() const override;
    virtual std::vector<std::string> ManufactureTypes() const override;
};