#include "society.h"

#include "map/map.h"
#include "map/component.h"
#include "map/room.h"
#include "populace/populace.h"
#include "populace/person.h"
#include "populace/scheduler.h"
#include "society/calendar.h"
#include "society/job.h"
#include "society/organization.h"
#include "story/story.h"
#include "story/script.h"
#include "story/change.h"
#include "player/player.h"

// 每次Tick最多执行的计划节点数量（职业与组织合计）
#define MAX_TIMERS_PER_TICK 4

// 资源/空间分配重试的最大尝试次数
#define MAX_ALLOCATION_ATTEMPTS 16


using namespace std;

CalendarFactory* Society::calendarFactory = nullptr;
JobFactory* Society::jobFactory = nullptr;
OrganizationFactory* Society::organizationFactory = nullptr;

Society::Society() :
	organizations(),
	currentTime(),
	timerSet() {
	if (!calendarFactory) {
		calendarFactory = new CalendarFactory();
	}
	if (!jobFactory) {
		jobFactory = new JobFactory();
	}
	if (!organizationFactory) {
		organizationFactory = new OrganizationFactory();
	}
}

Society::~Society() {

}

void Society::LoadConfigs() const {
	calendarFactory->RemoveAll();
	jobFactory->RemoveAll();
	organizationFactory->RemoveAll();

	auto calendars = Config::GetEnables("calendar");
	for (auto calendar : calendars) {
		calendarFactory->SetConfig(calendar, true);
	}
	auto jobs = Config::GetEnables("job");
	for (auto job : jobs) {
		jobFactory->SetConfig(job, true);
	}
	auto organizations = Config::GetEnables("organization");
	for (auto organization : organizations) {
		organizationFactory->SetConfig(organization, true);
	}
}

void Society::InitCalendars(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {

	calendarFactory->RegisterCalendar(EmptyCalendar::GetId(),
		[]() { return new EmptyCalendar(); },
		[](CalendarMod* calendar) { delete calendar; }
	);

	for (auto dll : dlls) {
		HMODULE modHandle;
		if (modHandles.find(dll) != modHandles.end()) {
			modHandle = modHandles[dll];
		}
		else {
			modHandle = LoadLibraryA(dll.data());
			modHandles[dll] = modHandle;
		}

		if (modHandle) {
			debugf("Log: %s loaded successfully.\n", dll.data());

			auto registerFunc =
				reinterpret_cast<RegisterModCalendarsFunc>(GetProcAddress(modHandle, "RegisterModCalendars"));
			if (registerFunc) {
				registerFunc(calendarFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}
}

void Society::InitJobs(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {

	jobFactory->RegisterJob(EmptyJob::GetId(),
		[]() { return new EmptyJob(); },
		[](JobMod* job) { delete job; }
	);

	for (auto dll : dlls) {
		HMODULE modHandle;
		if (modHandles.find(dll) != modHandles.end()) {
			modHandle = modHandles[dll];
		}
		else {
			modHandle = LoadLibraryA(dll.data());
			modHandles[dll] = modHandle;
		}

		if (modHandle) {
			debugf("Log: %s loaded successfully.\n", dll.data());

			auto registerFunc =
				reinterpret_cast<RegisterModJobsFunc>(GetProcAddress(modHandle, "RegisterModJobs"));
			if (registerFunc) {
				registerFunc(jobFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}
}

void Society::InitOrganizations(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {

	organizationFactory->RegisterOrganization(EmptyOrganization::GetId(), EmptyOrganization::GetPower(),
		[]() { return new EmptyOrganization(); },
		[](OrganizationMod* job) { delete job; }
	);

	for (auto dll : dlls) {
		HMODULE modHandle;
		if (modHandles.find(dll) != modHandles.end()) {
			modHandle = modHandles[dll];
		}
		else {
			modHandle = LoadLibraryA(dll.data());
			modHandles[dll] = modHandle;
		}

		if (modHandle) {
			debugf("Log: %s loaded successfully.\n", dll.data());

			auto registerFunc =
				reinterpret_cast<RegisterModOrganizationsFunc>(GetProcAddress(modHandle, "RegisterModOrganizations"));
			if (registerFunc) {
				registerFunc(organizationFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}
}

void Society::Init(Map* map, Populace* populace, Player* player) {
	Destroy();

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
	for (auto& [_, cdf] : cdfs) {
		cdf /= sum;
	}

	int attempt = 0;
	while (attempt < MAX_ALLOCATION_ATTEMPTS) {
		float r = GetRandom(1000) / 1000.f;
		string selectedOrganization;
		for (const auto& [id, cdf] : cdfs) {
			if (r < cdf) {
				selectedOrganization = id;
				break;
			}
		}
		Organization* organization = new Organization(organizationFactory, selectedOrganization);

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
			delete organization;
			continue;
		}

		organizations.push_back(organization);
		unordered_map<string, int> usedComponents;
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
			usedComponents[type] = count;
		}

		auto jobArrangements = organization->ArrageVacancies(usedComponents);
		for (auto [type, count] : usedComponents) {
			auto& availableComponents = componentMap[type];
			for (int j = 0; j < count; j++) {
				if (availableComponents.empty()) {
					THROW_EXCEPTION(OutOfRangeException, "Not enough components of type " + type + ".\n");
				}
				auto component = availableComponents.back();
				availableComponents.pop_back();
				vector<Job*> jobs;
				for (const auto& componentName : jobArrangements[type]) {
					for (const auto& [jobName, calendarName] : componentName) {
						Job* job = new Job(jobFactory, jobName);
						job->SetCalendar(new Calendar(calendarFactory, calendarName));
						jobs.push_back(job);
					}
				}
				organization->AddVacancy(component, jobs);
			}
		}

		organization->ArrangeRooms();
		organization->InitOrganization();
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
		if (citizen->GetAge(player->GetTime()) < 18) continue;
		if (citizen->GetAge(player->GetTime()) >= 60) continue;
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
		Organization* organization = temps[r];
		if (organization == nullptr) {
			debugf("Warning: Null organization in temps.\n");
			temps[r] = temps.back();
			temps.pop_back();
			i--;
			continue;
		}
		auto jobs = organization->EnrollEmployee({ adults[i] });
		if (!jobs.empty()) {
			jobs[0]->InitJob(adults[i]->GetName(), 0);
			adults[i]->AddJob(jobs[0]);
			jobs[0]->GetPosition()->AddWorker(adults[i]);
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
	for (auto organization : organizations) {
		if (organization)delete organization;
	}
	organizations.clear();
}

vector<Change*> Society::Tick(Player* player, Story* story) {
	auto time = player->GetTime();
	auto cross = player->CrossDay();
	currentTime = *time;

	if (cross) {
		for (auto organization : organizations) {
			if (!organization) continue;
			organization->DailyPlan(*time);
			for (auto& [node, timer] : organization->GetPlans()) {
				timerSet.insert({ timer, nullptr, nullptr, organization, node });
			}
			for (auto& [component, jobs] : organization->GetJobs()) {
				for (auto& [job, person] : jobs) {
					auto signin = job->GetCalendar()->SigninTime(*time);
					auto signout = job->GetCalendar()->SignoutTime(*time);
					job->GetScript()->SetValue("self.signin_time", signin.ToString(false, true));
					job->GetScript()->SetValue("self.signout_time", signout.ToString(false, true));
					job->DailyPlan(*time);
					for (auto& [node, timer] : job->GetPlans()) {
						timerSet.insert({ timer, job, person, nullptr, node });
					}
				}
			}
		}
	}

	vector<Change*> result;

	int count = 0;
	while (count < MAX_TIMERS_PER_TICK && !timerSet.empty()) {
		auto it = timerSet.begin();
		auto& [target, job, person, organization, node] = *it;
		if (currentTime < target) break;
		vector<Change*> changes;
		if (job) {
			changes = job->ExecNode(node, story->GetScript(), person->GetScheduler()->GetScript());
		}
		else if (organization) {
			changes = organization->ExecNode(node, story->GetScript());
		}
		result.insert(result.end(), changes.begin(), changes.end());
		timerSet.erase(it);
		count++;
	}

	return result;
}

Organization* Society::GetOrganization(const string& name) const {
	for (auto organization : organizations) {
		if (organization && organization->GetName() == name) {
			return organization;
		}
	}
	return nullptr;
}

void Society::ApplyChange(Populace* populace, Player* player, Change* change,
	const vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	auto type = change->GetType();

	if (type == "bank_transaction") {
		auto obj = dynamic_cast<BankTransactionChange*>(change);
		if (!obj) return;

		if (obj->GetName().size() == 0) {
			player->AddDeposit(obj->GetAmount());
		}
		else {
			auto citizen = populace->GetCitizen(obj->GetName());
			if(citizen) {
				citizen->AddDeposit(obj->GetAmount());
			}
		}
	}
}
