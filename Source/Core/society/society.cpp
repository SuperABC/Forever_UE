#include "society.h"
#include "utility.h"
#include "error.h"
#include "json.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <random>


using namespace std;

JobFactory* Society::jobFactory = nullptr;
CalendarFactory* Society::calendarFactory = nullptr;
OrganizationFactory* Society::organizationFactory = nullptr;

Society::Society() :
	resourcePath(),
	organizations() {
	if (!jobFactory) {
		jobFactory = new JobFactory();
	}
	if (!calendarFactory) {
		calendarFactory = new CalendarFactory();
	}
	if (!organizationFactory) {
		organizationFactory = new OrganizationFactory();
	}
}

Society::~Society() {
	for (auto organization : organizations) {
		if (organization == nullptr) {
			continue;
		}
		for (auto& [_, jobs] : organization->GetJobs()) {
			for (auto& [job, _] : jobs) {
				if (job != nullptr) {
					jobFactory->DestroyJob(job);
				}
			}
		}
		organizationFactory->DestroyOrganization(organization);
	}
}

void Society::SetResourcePath(const string& path) {
	resourcePath = path;
}

void Society::InitJobs(unordered_map<string, HMODULE>& modHandles) {
	jobFactory->RegisterJob(DefaultJob::GetId(),
		[]() { return new DefaultJob(); },
		[](Job* job) { delete job; }
	);

	string modPath = "Mod.dll";
	HMODULE modHandle;
	if (modHandles.find(modPath) != modHandles.end()) {
		modHandle = modHandles[modPath];
	}
	else {
		modHandle = LoadLibraryA(modPath.data());
		modHandles[modPath] = modHandle;
	}
	if (modHandle) {
		debugf("Log: Mod dll loaded successfully.\n");
		RegisterModJobsFunc registerFunc =
			(RegisterModJobsFunc)GetProcAddress(modHandle, "RegisterModJobs");
		if (registerFunc) {
			registerFunc(jobFactory);
		}
		else {
			debugf("Warning: Incorrect dll content.\n");
		}
	}
	else {
		debugf("Warning: Failed to load mod.dll.\n");
	}

#ifdef MOD_TEST
	auto jobList = { "mod" };
	for (const auto& jobId : jobList) {
		if (jobFactory->CheckRegistered(jobId)) {
			auto job = jobFactory->CreateJob(jobId);
			debugf("Log: Created test job %s.\n", jobId);
			jobFactory->DestroyJob(job);
		}
		else {
			debugf("Warning: Job %s not registered.\n", jobId);
		}
	}
#endif // MOD_TEST
}

void Society::InitCalendars(unordered_map<string, HMODULE>& modHandles) {
	calendarFactory->RegisterCalendar(StandardCalendar::GetId(),
		[]() { return new StandardCalendar(); },
		[](Calendar* calendar) { delete calendar; }
	);
	calendarFactory->RegisterCalendar(FullCalendar::GetId(),
		[]() { return new FullCalendar(); },
		[](Calendar* calendar) { delete calendar; }
	);

	string modPath = "Mod.dll";
	HMODULE modHandle;
	if (modHandles.find(modPath) != modHandles.end()) {
		modHandle = modHandles[modPath];
	}
	else {
		modHandle = LoadLibraryA(modPath.data());
		modHandles[modPath] = modHandle;
	}
	if (modHandle) {
		debugf("Log: Mod dll loaded successfully.\n");
		RegisterModCalendarsFunc registerFunc =
			(RegisterModCalendarsFunc)GetProcAddress(modHandle, "RegisterModCalendars");
		if (registerFunc) {
			registerFunc(calendarFactory);
		}
		else {
			debugf("Warning: Incorrect dll content.\n");
		}
	}
	else {
		debugf("Warning: Failed to load mod.dll.\n");
	}

#ifdef MOD_TEST
	auto calendarList = { "mod" };
	for (const auto& calendarId : calendarList) {
		if (calendarFactory->CheckRegistered(calendarId)) {
			auto calendar = calendarFactory->CreateCalendar(calendarId);
			debugf("Log: Created test calendar %s.\n", calendarId);
			calendarFactory->DestroyCalendar(calendar);
		}
		else {
			debugf("Warning: Calendar %s not registered.\n", calendarId);
		}
	}
#endif // MOD_TEST
}

void Society::InitOrganizations(unordered_map<string, HMODULE>& modHandles) {
	organizationFactory->RegisterOrganization(DefaultOrganization::GetId(),
		DefaultOrganization::GetPower(),
		[]() { return new DefaultOrganization(); },
		[](Organization* organization) { delete organization; }
	);

	string modPath = "Mod.dll";
	HMODULE modHandle;
	if (modHandles.find(modPath) != modHandles.end()) {
		modHandle = modHandles[modPath];
	}
	else {
		modHandle = LoadLibraryA(modPath.data());
		modHandles[modPath] = modHandle;
	}
	if (modHandle) {
		debugf("Log: Mod dll loaded successfully.\n");
		RegisterModOrganizationsFunc registerFunc =
			(RegisterModOrganizationsFunc)GetProcAddress(modHandle,
				"RegisterModOrganizations");
		if (registerFunc) {
			registerFunc(organizationFactory);
		}
		else {
			debugf("Warning: Incorrect dll content.\n");
		}
	}
	else {
		debugf("Warning: Failed to load mod.dll.\n");
	}

#ifdef MOD_TEST
	auto organizationList = { "mod" };
	for (const auto& organizationId : organizationList) {
		if (organizationFactory->CheckRegistered(organizationId)) {
			auto organization = organizationFactory->CreateOrganization(organizationId);
			debugf("Log: Created test organization %s.\n", organizationId);
			organizationFactory->DestroyOrganization(organization);
		}
		else {
			debugf("Warning: Organization %s not registered.\n", organizationId);
		}
	}
#endif // MOD_TEST
}

void Society::ReadConfigs(const string& path) const {
	string fullPath = resourcePath + path;
	if (!filesystem::exists(fullPath)) {
		THROW_EXCEPTION(IOException, "Path does not exist: " + fullPath + ".\n");
	}

	JsonReader reader;
	JsonValue root;

	ifstream fin(fullPath);
	if (!fin.is_open()) {
		THROW_EXCEPTION(IOException, "Failed to open file: " + fullPath + ".\n");
	}
	if (reader.Parse(fin, root)) {
		for (auto job : root["mods"]["job"]) {
			jobFactory->SetConfig(job.AsString(), true);
		}
		for (auto calendar : root["mods"]["calendar"]) {
			calendarFactory->SetConfig(calendar.AsString(), true);
		}
		for (auto organization : root["mods"]["organization"]) {
			organizationFactory->SetConfig(organization.AsString(), true);
		}
	}
	else {
		fin.close();
		THROW_EXCEPTION(JsonFormatException, "Json syntax error: " + reader.GetErrorMessages() + ".\n");
	}
	fin.close();
}

void Society::Init(Map* map, Populace* populace, Time* time) {
	if (map == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Map is null.\n");
	}
	if (populace == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Populace is null.\n");
	}
	if (time == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Time is null.\n");
	}

	// 计算职缺
	auto components = map->GetComponents();
	if (components.empty()) {
		debugf("Warning: No components found in map.\n");
	}

	unordered_map<string, vector<Component*>> componentMap;
	for (auto component : components) {
		if (component == nullptr) {
			debugf("Warning: Null component in map components.\n");
			continue;
		}
		componentMap[component->GetType()].push_back(component);
	}

	auto powers = organizationFactory->GetPowers();
	vector<pair<string, float>> cdfs;
	float sum = 0.f;
	for (const auto& [id, power] : powers) {
		sum += power;
		cdfs.emplace_back(id, sum);
	}
	if (sum == 0.f) {
		THROW_EXCEPTION(RuntimeException, "No valid organization for generation.\n");
	}
	for (auto& [id, cdf] : cdfs) {
		cdf /= sum;
	}

	int attempt = 0;
	while (attempt < 16) {
		float r = GetRandom(1000) / 1000.f;
		string selectedOrganization;
		for (const auto& [id, cdf] : cdfs) {
			if (r <= cdf) {
				selectedOrganization = id;
				break;
			}
		}
		Organization* organization =
			organizationFactory->CreateOrganization(selectedOrganization);
		if (organization == nullptr) {
			attempt++;
			continue;
		}

		auto requirements = organization->ComponentRequirements();
		bool valid = true;
		for (const auto& [type, range] : requirements) {
			auto it = componentMap.find(type);
			if (it == componentMap.end()) {
				valid = false;
				break;
			}
			if (it->second.size() < static_cast<size_t>(range.first)) {
				valid = false;
				break;
			}
		}
		if (!valid) {
			attempt++;
			continue;
		}

		organizations.push_back(organization);
		vector<pair<string, int>> usedComponents;
		for (const auto& [type, range] : requirements) {
			auto it = componentMap.find(type);
			size_t available = it->second.size();
			int count;
			if (available < static_cast<size_t>(range.second)) {
				count = static_cast<int>(available);
			}
			else {
				count = range.first +
					GetRandom(range.second - range.first + 1);
			}
			usedComponents.emplace_back(type, count);
		}

		auto jobArrangements = organization->ArrageVacancies(usedComponents);
		for (size_t i = 0; i < usedComponents.size(); i++) {
			const auto& [type, count] = usedComponents[i];
			auto& availableComponents = componentMap[type];
			for (int j = 0; j < count; j++) {
				if (availableComponents.empty()) {
					THROW_EXCEPTION(OutOfRangeException, "Not enough components of type " + type + ".\n");
				}
				auto component = availableComponents.back();
				availableComponents.pop_back();
				vector<Job*> jobs;
				if (i >= jobArrangements.size()) {
					THROW_EXCEPTION(OutOfRangeException, "Job arrangement index out of range.\n");
				}
				for (const auto& componentName : jobArrangements[i].second) {
					for (const auto& jobName : componentName) {
						Job* job = jobFactory->CreateJob(jobName);
						if (job) {
							jobs.push_back(job);
						}
					}
				}
				organization->AddVacancy(component, jobs);
			}
		}

		organization->SetCalendar(calendarFactory);
		organization->ArrangeRooms();
	}

	// 分配工作
	vector<Organization*> temps;
	for (auto organization : organizations) {
		if (organization != nullptr) {
			temps.push_back(organization);
		}
	}
	auto citizens = populace->GetCitizens();
	vector<Person*> adults;
	for (auto citizen : citizens) {
		if (citizen == nullptr) {
			debugf("Warning: Null citizen in populace.\n");
			continue;
		}
		if (citizen->GetAge(time) < 18) continue;
		if (citizen->GetAge(time) >= 60) continue;
		adults.push_back(citizen);
	}
	for (int i = 0; i < static_cast<int>(adults.size()); i++) {
		if (temps.empty()) {
			break;
		}
		int r = GetRandom(static_cast<int>(temps.size()));
		if (r < 0 || r >= static_cast<int>(temps.size())) {
			THROW_EXCEPTION(OutOfRangeException, "Random index out of range in job assignment.\n");
		}
		Organization* org = temps[r];
		if (org == nullptr) {
			debugf("Warning: Null organization in temps.\n");
			temps[r] = temps.back();
			temps.pop_back();
			i--;
			continue;
		}
		vector<int> ids;
		ids.push_back(adults[i]->GetId());
		auto jobs = org->EnrollEmployee(ids);
		if (!jobs.empty()) {
			adults[i]->AddJob(jobs[0]);
		}
		else {
			temps[r] = temps.back();
			temps.pop_back();
			if (temps.empty()) break;
			i--;
		}
	}
}

void Society::Destroy() {

}

void Society::Tick(int day, int hour, int min, int sec) {

}

void Society::Print() const {

}

void Society::Load(const string& path) {

}

void Society::Save(const string& path) const {

}

void Society::ApplyChange(Change* change, Story* story,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	if (change == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Change pointer is null in ApplyChange.\n");
	}
	if (story == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Story pointer is null in ApplyChange.\n");
	}

}
