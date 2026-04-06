#pragma once

#include "society/organization_mod.h"


// 商店组织
class ShopOrganization : public OrganizationMod {
public:
	ShopOrganization();
	virtual ~ShopOrganization();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static float GetPower();

	virtual void ComponentRequirements() override;

	virtual void ArrageVacancies(const std::unordered_map<std::string, int>& components) override;

	virtual void ArrageRoom(std::vector<std::pair<std::string, int>>& arrages,
		const std::vector<std::string>& rooms);

private:
	static int count;

	int id;
	std::string name;
};
