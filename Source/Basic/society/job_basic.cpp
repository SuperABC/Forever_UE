#include "job_basic.h"


using namespace std;

int ShopSalerJob::count = 0;

ShopSalerJob::ShopSalerJob() : id(count++) {

}

ShopSalerJob::~ShopSalerJob() {

}

const char* ShopSalerJob::GetId() {
	return "shop_saler";
}

const char* ShopSalerJob::GetType() const {
	return "shop_saler";
}

const char* ShopSalerJob::GetName() {
	name = "售货员" + to_string(id);
	return name.data();
}

void ShopSalerJob::InitJob() {
	script = { "empty", {"shop_saler"} };
}

void ShopSalerJob::DailyPlan() {

}

void ShopSalerJob::ExecNode(const string& name, Script* storyScript, Script* schedulerScript, Script* jobScript) {

}
