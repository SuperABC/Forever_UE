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

void ShopSalerJob::DailyPlan(const Time& time, PostHandle* post) {

}

void ShopSalerJob::ExecNode(const string& name,
	Container* storyScript, Container* schedulerScript, Container* jobScript,
	PostHandle* post) {

}

int HospitalReceptionJob::count = 0;

HospitalReceptionJob::HospitalReceptionJob() : id(count++) {

}

HospitalReceptionJob::~HospitalReceptionJob() {

}

const char* HospitalReceptionJob::GetId() {
	return "hospital_reception";
}

const char* HospitalReceptionJob::GetType() const {
	return "hospital_reception";
}

const char* HospitalReceptionJob::GetName() {
	name = "前台" + to_string(id);
	return name.data();
}

void HospitalReceptionJob::InitJob() {
	script = { "empty", {"hospital_reception"} };
}

void HospitalReceptionJob::DailyPlan(const Time& time, PostHandle* post) {

}

void HospitalReceptionJob::ExecNode(const string& name,
	Container* storyScript, Container* schedulerScript, Container* jobScript,
	PostHandle* post) {

}

int OutpatientDoctorJob::count = 0;

OutpatientDoctorJob::OutpatientDoctorJob() : id(count++) {

}

OutpatientDoctorJob::~OutpatientDoctorJob() {

}

const char* OutpatientDoctorJob::GetId() {
	return "outpatient_doctor";
}

const char* OutpatientDoctorJob::GetType() const {
	return "outpatient_doctor";
}

const char* OutpatientDoctorJob::GetName() {
	name = "医生" + to_string(id);
	return name.data();
}

void OutpatientDoctorJob::InitJob() {
	script = { "empty", {"outpatient_doctor"} };
}

void OutpatientDoctorJob::DailyPlan(const Time& time, PostHandle* post) {

}

void OutpatientDoctorJob::ExecNode(const string& name,
	Container* storyScript, Container* schedulerScript, Container* jobScript,
	PostHandle* post) {

}
