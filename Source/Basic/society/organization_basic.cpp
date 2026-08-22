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
	requirements["shop"] = make_pair(1, 1);
}

void ShopOrganization::ArrageVacancies(const unordered_map<string, int>& components) {
	for (const auto& [name, count] : components) {
		vacancies[name] = vector<vector<pair<string, string>>>(count);
		for (int i = 0; i < count; i++) {
			for (int j = 0; j < 10; j++) {
				vacancies[name][i].emplace_back("shop_saler", "full");
			}
		}
	}
}

void ShopOrganization::ArrageRoom(vector<pair<string, int>>& arrangements,
	const vector<string>& rooms) {
	if (rooms.size() == 0)return;
	for (auto& [_, room] : arrangements) {
		room = 0;
	}
}

void ShopOrganization::InitOrganization() {
	script = { "empty", { "" } };
}

void ShopOrganization::DailyPlan(const Time& time, PostHandle* post) {
	if (time.GetDay() == 1) {
		plans["salary_payment"] = time;
	}
}

void ShopOrganization::ExecNode(const string& name,
	Container* storyScript, Container* organizationScript, PostHandle* post) {
	if (name == "salary_payment") {
		int count = ToInt(organizationScript->GetValue("self.employee_count").second);
		for (int i = 0; i < count; i++) {
			string employee = ToString(organizationScript->GetValue("self.employees[" + to_string(i) + "].name").second);
			changes.push_back(BankTransactionChange(employee, 5000));
		}
	}
}

int HospitalOrganization::count = 0;

HospitalOrganization::HospitalOrganization() : id(count++) {

}

HospitalOrganization::~HospitalOrganization() {

}

const char* HospitalOrganization::GetId() {
	return "hospital";
}

const char* HospitalOrganization::GetType() const {
	return "hospital";
}

const char* HospitalOrganization::GetName() {
	name = "医院组织" + to_string(id);
	return name.data();
}

float HospitalOrganization::GetPower() {
	return 1.f;
}

void HospitalOrganization::ComponentRequirements() {
	requirements["hospital"] = make_pair(1, 1);
}

void HospitalOrganization::ArrageVacancies(const unordered_map<string, int>& components) {
	for (const auto& [name, count] : components) {
		vacancies[name] = vector<vector<pair<string, string>>>(count);
		for (int i = 0; i < count; i++) {
			vacancies[name][i].emplace_back("hospital_reception", "full");
			// 诊室数量在这一步不可知（HospitalBuilding 楼层数 5~10 层随机，每层固定
			// 安排4间诊室，理论最大 40 间）；超配到理论上限，多出来的空缺会在
			// Organization::ArrangeRooms() 里因为房间容量已满被丢弃。如果以后
			// HospitalBuilding::LayoutBuilding 的楼层/诊室数公式改了，这个 40 也要同步改。
			for (int j = 0; j < 40; j++) {
				vacancies[name][i].emplace_back("outpatient_doctor", "full");
			}
		}
	}
}

void HospitalOrganization::ArrageRoom(vector<pair<string, int>>& arrangements,
	const vector<string>& rooms) {
	// 和 ShopOrganization::ArrageRoom（无脑指向 index 0）不同，这里要把每个职位
	// 按类型分配到不同的房间实例，否则同类型房间只有第一间能招到人。
	static const unordered_map<string, string> jobRoomTypes = {
		{ "hospital_reception", "register" },
		{ "outpatient_doctor", "doctor" },
	};
	vector<bool> used(rooms.size(), false);
	for (auto& [jobType, room] : arrangements) {
		auto it = jobRoomTypes.find(jobType);
		if (it == jobRoomTypes.end()) continue;
		const string& neededType = it->second;
		for (int k = 0; k < static_cast<int>(rooms.size()); k++) {
			if (used[k] || rooms[k] != neededType) continue;
			room = k;
			used[k] = true;
			break;
		}
	}
}

void HospitalOrganization::InitOrganization() {
	script = { "empty", { "" } };
}

void HospitalOrganization::DailyPlan(const Time& time, PostHandle* post) {
	if (time.GetDay() == 1) {
		plans["salary_payment"] = time;
	}
}

void HospitalOrganization::ExecNode(const string& name,
	Container* storyScript, Container* organizationScript, PostHandle* post) {
	if (name == "salary_payment") {
		int count = ToInt(organizationScript->GetValue("self.employee_count").second);
		for (int i = 0; i < count; i++) {
			string employee = ToString(organizationScript->GetValue("self.employees[" + to_string(i) + "].name").second);
			changes.push_back(BankTransactionChange(employee, 5000));
		}
	}
}


