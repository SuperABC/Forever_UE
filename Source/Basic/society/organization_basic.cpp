#include "organization_basic.h"


using namespace std;

int ShopOrganization::count = 0;

ShopOrganization::ShopOrganization() : id(count++) {

}

ShopOrganization::~ShopOrganization() {

}

const char* ShopOrganization::GetId() {
	return "shop";
}

const char* ShopOrganization::GetType() const {
	return "shop";
}

const char* ShopOrganization::GetName() {
	name = "商店组织" + to_string(id);
	return name.data();
}

float ShopOrganization::GetPower() {
	return 1.f;
}

void ShopOrganization::ComponentRequirements() {
	requirements["residential"] = make_pair(1, 1);
}

void ShopOrganization::ArrageVacancies(const unordered_map<string, int>& components) {
	for (const auto& [name, count] : components) {
		vacancies[name] = vector<vector<pair<string, string>>>(count);
		for (int i = 0; i < count; ++i) {
			for (int j = 0; j < 10; j++) {
				vacancies[name][i].emplace_back("shop_saler", "full");
			}
		}
	}
}

void ShopOrganization::ArrageRoom(vector<pair<string, int>>& arrages,
	const vector<string>& rooms) {
	if (rooms.size() == 0)return;
	for (auto& [job, room] : arrages) {
		room = 0;
	}
}


