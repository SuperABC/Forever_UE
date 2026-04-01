#pragma once

#include "zone_mod.h"


// 住宅园区
class ResidentialZone : public ZoneMod {
public:
	ResidentialZone();
	virtual ~ResidentialZone();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static std::function<int(const Lot*)> ZoneAssigner;

	virtual void LayoutZone(const Lot* lot) override;

private:
	static int count;

	int id;
	std::string name;
};
