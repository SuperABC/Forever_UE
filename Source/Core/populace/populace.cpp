#include "populace.h"
#include "utility.h"
#include "error.h"
#include "json.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <random>


using namespace std;

AssetFactory* Populace::assetFactory = nullptr;
NameFactory* Populace::nameFactory = nullptr;
SchedulerFactory* Populace::schedulerFactory = nullptr;

Populace::Populace()
	: resourcePath()
	, names(nullptr)
	, citizens()
	, ids() {
	if (!assetFactory) {
		assetFactory = new AssetFactory();
	}
	if (!nameFactory) {
		nameFactory = new NameFactory();
	}
	if (!schedulerFactory) {
		schedulerFactory = new SchedulerFactory();
	}
}

Populace::~Populace() {
	for (auto citizen : citizens) {
		if (citizen) {
			for (auto asset : citizen->GetAssets()) {
				assetFactory->DestroyAsset(asset);
			}
			schedulerFactory->DestroyScheduler(citizen->GetScheduler());
			delete citizen;
		}
	}
	if (names) {
		nameFactory->DestroyName(names);
	}
}

void Populace::SetResourcePath(const string& path) {
	resourcePath = path;
}

void Populace::InitAssets(unordered_map<string, HMODULE>& modHandles) {
	assetFactory->RegisterAsset(ZoneAsset::GetId(),
		[]() { return new ZoneAsset(); },
		[](Asset* asset) { delete asset; }
	);
	assetFactory->RegisterAsset(BuildingAsset::GetId(),
		[]() { return new BuildingAsset(); },
		[](Asset* asset) { delete asset; }
	);
	assetFactory->RegisterAsset(RoomAsset::GetId(),
		[]() { return new RoomAsset(); },
		[](Asset* asset) { delete asset; }
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
		debugf("Mod dll loaded successfully.\n");

		RegisterModAssetsFunc registerFunc =
			(RegisterModAssetsFunc)GetProcAddress(modHandle, "RegisterModAssets");
		if (registerFunc) {
			registerFunc(assetFactory);
		}
		else {
			debugf("Incorrect dll content.\n");
		}
	}
	else {
		debugf("Failed to load mod.dll.\n");
	}

#ifdef MOD_TEST
	auto assetList = { "mod" };
	for (const auto& assetId : assetList) {
		if (assetFactory->CheckRegistered(assetId)) {
			auto asset = assetFactory->CreateAsset(assetId);
			debugf("Created asset: mod.\n");
			delete asset;
		}
		else {
			debugf("Asset not registered: %s.\n", assetId);
		}
	}
#endif // MOD_TEST
}

void Populace::InitNames(unordered_map<string, HMODULE>& modHandles) {
	nameFactory->RegisterName(ChineseName::GetId(),
		[]() { return new ChineseName(); },
		[](Name* name) { delete name; }
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
		debugf("Mod dll loaded successfully.\n");

		RegisterModNamesFunc registerFunc =
			(RegisterModNamesFunc)GetProcAddress(modHandle, "RegisterModNames");
		if (registerFunc) {
			registerFunc(nameFactory);
		}
		else {
			debugf("Incorrect dll content.\n");
		}
	}
	else {
		debugf("Failed to load mod.dll.\n");
	}

#ifdef MOD_TEST
	auto nameList = { "mod" };
	for (const auto& nameId : nameList) {
		if (nameFactory->CheckRegistered(nameId)) {
			auto name = nameFactory->CreateName(nameId);
			debugf("Created name: mod.\n");
			delete name;
		}
		else {
			debugf("Name not registered: %s.\n", nameId);
		}
	}
#endif // MOD_TEST
}

void Populace::InitSchedulers(unordered_map<string, HMODULE>& modHandles) {
	schedulerFactory->RegisterScheduler(WorkOnlyScheduler::GetId(),
		WorkOnlyScheduler::GetPower(),
		[]() { return new WorkOnlyScheduler(); },
		[](Scheduler* scheduler) { delete scheduler; }
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
		debugf("Mod dll loaded successfully.\n");
		RegisterModSchedulersFunc registerFunc =
			(RegisterModSchedulersFunc)GetProcAddress(modHandle, "RegisterModSchedulers");
		if (registerFunc) {
			registerFunc(schedulerFactory);
		}
		else {
			debugf("Incorrect dll content.\n");
		}
	}
	else {
		debugf("Failed to load mod.dll.\n");
	}

#ifdef MOD_TEST
	auto schedulerList = { "mod" };
	for (const auto& schedulerId : schedulerList) {
		if (schedulerFactory->CheckRegistered(schedulerId)) {
			auto scheduler = schedulerFactory->CreateScheduler(schedulerId);
			debugf("Created scheduler: mod.\n");
			delete scheduler;
		}
		else {
			debugf("Scheduler not registered: %s.\n", schedulerId);
		}
	}
#endif // MOD_TEST
}

void Populace::ReadConfigs(const string& path) const {
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
		for (auto asset : root["mods"]["asset"]) {
			assetFactory->SetConfig(asset.AsString(), true);
		}
		nameFactory->SetConfig(root["mods"]["name"].AsString(), true);
		for (auto scheduler : root["mods"]["scheduler"]) {
			schedulerFactory->SetConfig(scheduler.AsString(), true);
		}
	}
	else {
		fin.close();
		THROW_EXCEPTION(JsonFormatException,
			"Json syntax error: " + reader.GetErrorMessages() + ".\n");
	}
	fin.close();
}

void Populace::Init(int accomodation, const vector<string>& nameholders, Time* time) {
	if (!time) {
		THROW_EXCEPTION(NullPointerException, "Time pointer is null in Init.\n");
	}
	// 生成市民
	GenerateCitizens(static_cast<int>(accomodation *
		exp(GetRandom(1000) / 1000.0f - 0.5f)), nameholders, time);

	// 生成经历
	GenerateEducations(time);
	GenerateEmotions(time);
	GenerateJobs();

	// 更新个人变量
	for (auto citizen : citizens) {
		if (citizen) {
			citizen->UpdateValues(time);
		}
	}
}

void Populace::Destroy() {
	// 目前为空，但保留
}

void Populace::Tick(int day, int hour, int min, int sec) {
	// 更新个人变量
	// 通勤
}

void Populace::Print() const {
	// 空实现
}

void Populace::Load(const string& path) {
	// 待实现
}

void Populace::Save(const string& path) const {
	// 待实现
}

void Populace::ApplyChange(Change* change, Story* story,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	if (!change) {
		THROW_EXCEPTION(NullPointerException, "Change pointer is null in ApplyChange.\n");
	}
	if (!story) {
		THROW_EXCEPTION(NullPointerException, "Story pointer is null in ApplyChange.\n");
	}

	auto type = change->GetType();

	if (type == "add_option") {
		auto obj = dynamic_cast<AddOptionChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException,
				"Failed to cast Change to AddOptionChange.\n");
		}

		Condition conditionTarget;
		conditionTarget.ParseCondition(obj->GetTarget());
		string targetName = ToString(conditionTarget.EvaluateValue(getValues));
		Person* target = GetCitizen(targetName);
		if (!target) {
			THROW_EXCEPTION(InvalidArgumentException,
				"Target citizen not found: " + targetName + ".\n");
		}

		Condition conditionOption;
		conditionOption.ParseCondition(obj->GetOption());
		string option = ToString(conditionOption.EvaluateValue(getValues));
		target->AddOption(option);

	}
	else if (type == "remove_option") {
		auto obj = dynamic_cast<RemoveOptionChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException,
				"Failed to cast Change to RemoveOptionChange.\n");
		}

		Condition conditionTarget;
		conditionTarget.ParseCondition(obj->GetTarget());
		string targetName = ToString(conditionTarget.EvaluateValue(getValues));
		Person* target = GetCitizen(targetName);
		if (!target) {
			THROW_EXCEPTION(InvalidArgumentException,
				"Target citizen not found: " + targetName + ".\n");
		}

		Condition conditionOption;
		conditionOption.ParseCondition(obj->GetOption());
		string option = ToString(conditionOption.EvaluateValue(
			[&](const string& name) -> pair<bool, ValueType> {
				return story->GetValue(name);
			}));
		target->RemoveOption(option);

	}
	else if (type == "spawn_npc") {
		auto obj = dynamic_cast<SpawnNpcChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException,
				"Failed to cast Change to SpawnNpcChange.\n");
		}

		Condition condition;
		auto person = new Person();

		person->SetId(static_cast<int>(citizens.size()));

		condition.ParseCondition(obj->GetTarget());
		person->SetName(ToString(condition.EvaluateValue(getValues)));

		condition.ParseCondition(obj->GetGender());
		string genderStr = ToString(condition.EvaluateValue(
			[&](const string& name) -> pair<bool, ValueType> {
				return story->GetValue(name);
			}));
		person->SetGender(genderStr == "male" ? GENDER_MALE : GENDER_FEMALE);

		condition.ParseCondition(obj->GetBirthday());
		person->SetBirthday(Time(ToString(condition.EvaluateValue(getValues))));

		citizens.push_back(person);
		ids[person->GetName()] = person->GetId();

	}
	else if (type == "remove_npc") {
		auto obj = dynamic_cast<RemoveNpcChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException,
				"Failed to cast Change to RemoveNpcChange.\n");
		}

		Condition conditionTarget;
		conditionTarget.ParseCondition(obj->GetTarget());
		string targetName = ToString(conditionTarget.EvaluateValue(getValues));
		Person* person = GetCitizen(targetName);

		if (!person) {
			THROW_EXCEPTION(InvalidArgumentException,
				"Target citizen not found: " + targetName + ".\n");
		}

		int id = person->GetId();
		if (id < 0 || id >= static_cast<int>(citizens.size())) {
			THROW_EXCEPTION(OutOfRangeException,
				"Citizen ID out of range: " + to_string(id) + ".\n");
		}
		delete citizens[id];
		citizens[id] = nullptr;
		ids[person->GetName()] = -1;
	}
}

void Populace::Schedule() const {
	// 汇总权重
	auto powers = schedulerFactory->GetPowers();
	vector<pair<string, float>> cdfs;
	float sum = 0.f;
	for (const auto& [id, power] : powers) {
		sum += power;
		cdfs.emplace_back(id, sum);
	}
	if (sum == 0.f) {
		THROW_EXCEPTION(InvalidArgumentException,
			"No valid organization for generation.\n");
	}
	for (auto& cdf : cdfs) {
		cdf.second /= sum;
	}

	// 加权分配调度
	for (auto citizen : citizens) {
		if (!citizen) continue; // 跳过已删除的市民

		float r = GetRandom(1000) / 1000.f;
		string selectedScheduler;
		for (const auto& [id, cumProb] : cdfs) {
			if (r <= cumProb) {
				selectedScheduler = id;
				break;
			}
		}
		Scheduler* scheduler = schedulerFactory->CreateScheduler(selectedScheduler);
		if (!scheduler) {
			debugf("Warning: Failed to create scheduler '%s' for citizen %s.\n",
				selectedScheduler.c_str(), citizen->GetName().c_str());
			continue;
		}
		citizen->SetScheduler(scheduler);
	}
}

void Populace::Workload(Story* story) const {
	if (!story) {
		THROW_EXCEPTION(NullPointerException, "Story pointer is null in Workload.\n");
	}
	// 添加职业剧本
	unordered_map<string, Script*> jobScripts;
	for (auto citizen : citizens) {
		if (!citizen) continue;
		for (auto job : citizen->GetJobs()) {
			if (!job) continue;
			for (const auto& scriptPath : job->GetScripts()) {
				auto script = new Script();
				script->ReadMilestones(scriptPath, resourcePath + scriptPath,
					story->GetEventFactory(), story->GetChangeFactory());
				citizen->AddScript(script);
			}
		}
	}
	debugf("Generate workloads.\n");
}

void Populace::Characterize(const string& path, Story* story) const {
	if (!story) {
		THROW_EXCEPTION(NullPointerException, "Story pointer is null in Characterize.\n");
	}
	// 汇总个性剧本
	vector<string> characters;
	for (const auto& entry : filesystem::directory_iterator(resourcePath + path)) {
		if (entry.is_regular_file() && entry.path().extension() == ".json") {
			characters.push_back(entry.path().string());
		}
	}
	if (characters.empty()) {
		debugf("No character scripts found.\n");
		return;
	}

	// 随机选择个性剧本
	for (auto citizen : citizens) {
		if (!citizen) continue;
		int r = GetRandom(static_cast<int>(characters.size()));
		Script* script = new Script();
		script->ReadMilestones(characters[r], characters[r],
			story->GetEventFactory(), story->GetChangeFactory());
		citizen->AddScript(script);
	}
	debugf("Generate characters.\n");
}

vector<Person*>& Populace::GetCitizens() {
	return citizens;
}

Person* Populace::GetCitizen(const string& name) {
	auto it = ids.find(name);
	if (it == ids.end() || it->second < 0 ||
		it->second >= static_cast<int>(citizens.size())) {
		return nullptr;
	}
	return citizens[it->second];
}

AssetFactory* Populace::GetAssetFactory() {
	return assetFactory;
}

pair<vector<Dialog>, vector<Change*>> Populace::TriggerEvent(
	const string& name, Event* event, Story* story) const {
	if (!event) {
		THROW_EXCEPTION(NullPointerException, "Event pointer is null in TriggerEvent.\n");
	}
	if (!story) {
		THROW_EXCEPTION(NullPointerException, "Story pointer is null in TriggerEvent.\n");
	}
	for (auto citizen : citizens) {
		if (citizen && citizen->GetName() == name) {
			return citizen->MatchEvent(event, story, citizen);
		}
	}
	return make_pair(vector<Dialog>(), vector<Change*>());
}

pair<vector<Dialog>, vector<Change*>> Populace::TriggerEvent(
	int id, Event* event, Story* story) const {
	if (!event) {
		THROW_EXCEPTION(NullPointerException, "Event pointer is null in TriggerEvent.\n");
	}
	if (!story) {
		THROW_EXCEPTION(NullPointerException, "Story pointer is null in TriggerEvent.\n");
	}
	if (id >= 0 && id < static_cast<int>(citizens.size()) && citizens[id]) {
		return citizens[id]->MatchEvent(event, story, citizens[id]);
	}
	return make_pair(vector<Dialog>(), vector<Change*>());
}

void Populace::GenerateCitizens(int num, const vector<string>& nameholders, Time* time) {
	if (!time) {
		THROW_EXCEPTION(NullPointerException, "Time pointer is null in GenerateCitizens.\n");
	}
	enum LIFE_TYPE : int {
		LIFE_SINGLE,
		LIFE_MARRY,
		LIFE_BIRTH,
		LIFE_DEAD
	};

	struct Human {
		int idx;
		string name;
		int birth;
		int marry;
		LIFE_TYPE life;
		GENDER_TYPE gender;
		int father;
		int mother;
		int spouse;
		vector<pair<GENDER_TYPE, int>> childs;
	};

	// 分配姓名
	names = nameFactory->GetName();
	if (!names) {
		THROW_EXCEPTION(InvalidConfigException,
			"No enabled name in config.\n");
	}
	for (const auto& holder : nameholders) {
		names->ReserveName(holder);
	}

	// 临时男女数组及年表
	vector<Human> females(1, { -1, "", 0, 0, LIFE_DEAD });
	vector<Human> males(1, { -1, "", 0, 0, LIFE_DEAD });
	vector<int> maleBirths(4096, -1);
	int currentBirth = 0;
	vector<vector<pair<int, LIFE_TYPE>>> chronology(4096);

	// 初始添加100男100女
	int year = 1;
	for (int i = 1; i <= 100; i++) {
		auto n = names->GenerateName(false, true);
		if (n.empty()) continue;
		females.push_back({ -1, n, GetRandom(20), -1, LIFE_SINGLE, GENDER_FEMALE,
			-1, -1, -1, {} });
		int marryIdx = static_cast<int>(females.size()) - 1;
		int marryYear = females.back().birth + 20 + GetRandom(15);
		if (marryYear >= 0 && marryYear < 4096) {
			chronology[marryYear].emplace_back(marryIdx, LIFE_MARRY);
		}
		int deathYear = females.back().birth + 60 + GetRandom(40);
		if (deathYear >= 0 && deathYear < 4096) {
			chronology[deathYear].emplace_back(marryIdx, LIFE_DEAD);
		}
	}
	for (int i = 1; i <= 100; i++) {
		auto n = names->GenerateName(true, false);
		if (n.empty()) continue;
		males.push_back({ -1, n, GetRandom(20), -1, LIFE_SINGLE, GENDER_MALE,
			-1, -1, -1, {} });
	}
	sort(males.begin() + 1, males.end(),
		[](const Human& x, const Human& y) { return x.birth < y.birth; });
	maleBirths[0] = 0;
	for (int i = 1; i <= 100; i++) {
		if (males[i].birth > currentBirth) {
			for (int j = currentBirth + 1; j <= males[i].birth && j < 4096; j++) {
				maleBirths[j] = i;
			}
			currentBirth = males[i].birth;
		}
		int deathYear = males[i].birth + 60 + GetRandom(40);
		if (deathYear >= 0 && deathYear < 4096) {
			chronology[deathYear].emplace_back(-i, LIFE_DEAD);
		}
	}

	// 自动迭代繁殖最多4096年
	while (year < 4096 &&
		static_cast<int>(females.size()) + static_cast<int>(males.size()) < num) {
		for (const auto& [eventIdx, lifeType] : chronology[year]) {
			if (eventIdx >= 0) {
				if (eventIdx >= static_cast<int>(females.size())) {
					debugf("Warning: female index %d out of range.\n", eventIdx);
					continue;
				}
				switch (lifeType) {
				case LIFE_MARRY: {
					int lowId = max(0, females[eventIdx].birth - 10);
					int highId = max(0, females[eventIdx].birth + 5);
					if (lowId >= 4096 || highId >= 4096) break;
					if (maleBirths[lowId] == maleBirths[highId]) break;
					if (maleBirths[highId] == -1) break;
					int selectId = -1;
					for (int i = 0; i < 10; i++) {
						int candidate = maleBirths[lowId] +
							GetRandom(maleBirths[highId] - maleBirths[lowId]);
						if (candidate >= 0 && candidate < static_cast<int>(males.size()) &&
							males[candidate].spouse >= 0) {
							continue;
						}
						else {
							selectId = candidate;
							break;
						}
					}
					if (selectId < 0 || selectId >= static_cast<int>(males.size())) break;
					females[eventIdx].spouse = selectId;
					females[eventIdx].life = LIFE_MARRY;
					females[eventIdx].marry = year;
					males[selectId].spouse = eventIdx;
					males[selectId].life = LIFE_MARRY;
					males[selectId].marry = year;
					int childs = ((GetRandom(8) == 0) ? 0 : (1 + GetRandom(6)));
					int interval = 1 + GetRandom(3);
					for (int i = 0; i < childs; i++) {
						if (year + interval - females[eventIdx].birth > 45) break;
						int birthYear = year + interval;
						if (birthYear >= 0 && birthYear < 4096) {
							chronology[birthYear].emplace_back(eventIdx, LIFE_BIRTH);
						}
						interval += 1 + GetRandom(3);
					}
					break;
				}
				case LIFE_BIRTH: {
					int gender = GetRandom(2);
					auto n = names->GenerateName(
						names->GetSurname(males[females[eventIdx].spouse].name),
						gender == GENDER_MALE, gender == GENDER_FEMALE);
					if (n.empty()) break;
					if (gender == GENDER_FEMALE) {
						females[eventIdx].childs.emplace_back(GENDER_FEMALE,
							static_cast<int>(females.size()));
						males[females[eventIdx].spouse].childs.emplace_back(GENDER_FEMALE,
							static_cast<int>(females.size()));
						females.push_back({ -1, n, year, -1, LIFE_SINGLE, GENDER_FEMALE,
							females[eventIdx].spouse, eventIdx, -1, {} });
						int newIdx = static_cast<int>(females.size()) - 1;
						if (GetRandom(10) > 0) {
							int marryYear = females.back().birth + 20 + GetRandom(15);
							if (marryYear >= 0 && marryYear < 4096) {
								chronology[marryYear].emplace_back(newIdx, LIFE_MARRY);
							}
						}
						int deathYear = females.back().birth + 60 + GetRandom(40);
						if (deathYear >= 0 && deathYear < 4096) {
							chronology[deathYear].emplace_back(newIdx, LIFE_DEAD);
						}
					}
					else {
						females[eventIdx].childs.emplace_back(GENDER_MALE,
							static_cast<int>(males.size()));
						males[females[eventIdx].spouse].childs.emplace_back(GENDER_MALE,
							static_cast<int>(males.size()));
						males.push_back({ -1, n, year, -1, LIFE_SINGLE, GENDER_MALE,
							females[eventIdx].spouse, eventIdx, -1, {} });
						int newIdx = static_cast<int>(males.size()) - 1;
						if (males.back().birth > currentBirth) {
							for (int j = currentBirth + 1;
								j <= males.back().birth && j < 4096; j++) {
								maleBirths[j] = newIdx;
							}
							currentBirth = males.back().birth;
						}
						int deathYear = males.back().birth + 60 + GetRandom(40);
						if (deathYear >= 0 && deathYear < 4096) {
							chronology[deathYear].emplace_back(-newIdx, LIFE_DEAD);
						}
					}
					break;
				}
				case LIFE_DEAD:
					females[eventIdx].life = LIFE_DEAD;
					break;
				default:
					break;
				}
			}
			else {
				int maleIdx = -eventIdx;
				if (maleIdx >= 0 && maleIdx < static_cast<int>(males.size())) {
					switch (lifeType) {
					case LIFE_DEAD:
						males[maleIdx].life = LIFE_DEAD;
						break;
					default:
						break;
					}
				}
			}
		}
		year++;
	}
	time->SetYear(year + 2000);

	// 将活着的男女加入市民列表
	for (size_t i = 1; i < females.size(); i++) {
		if (females[i].life != LIFE_DEAD && GetRandom(20) > 0) {
			Person* person = new Person();
			person->SetId(static_cast<int>(citizens.size()));
			females[i].idx = static_cast<int>(citizens.size());
			person->SetName(females[i].name);
			person->SetGender(GENDER_FEMALE);
			int month = 1 + GetRandom(12);
			int day = 1 + GetRandom(Time::DaysInMonth(2000 + females[i].birth, month));
			person->SetBirthday({ 2000 + females[i].birth, month, day });
			citizens.push_back(person);
			ids[person->GetName()] = person->GetId();
		}
	}
	for (size_t i = 1; i < males.size(); i++) {
		if (males[i].life != LIFE_DEAD && GetRandom(20) > 0) {
			Person* person = new Person();
			person->SetId(static_cast<int>(citizens.size()));
			males[i].idx = static_cast<int>(citizens.size());
			person->SetName(males[i].name);
			person->SetGender(GENDER_MALE);
			int month = 1 + GetRandom(12);
			int day = 1 + GetRandom(Time::DaysInMonth(2000 + males[i].birth, month));
			person->SetBirthday({ 2000 + males[i].birth, month, day });
			citizens.push_back(person);
			ids[person->GetName()] = person->GetId();
		}
	}

	// 记录有配偶的市民的结婚日期
	for (size_t i = 1; i < females.size(); i++) {
		if (females[i].spouse >= 0 && females[i].idx >= 0 &&
			static_cast<size_t>(females[i].spouse) < males.size() &&
			males[females[i].spouse].idx >= 0) {
			int month = GetRandom(12) + 1;
			Time marryday(2000 + females[i].marry, month,
				1 + GetRandom(Time::DaysInMonth(2000 + females[i].marry, month)));
			citizens[females[i].idx]->SetMarryday(marryday);
			citizens[males[females[i].spouse].idx]->SetMarryday(marryday);
		}
	}

	// 记录亲属关系
	for (const auto& female : females) {
		if (female.idx >= 0) {
			Person* person = citizens[female.idx];
			if (female.mother >= 0 && static_cast<size_t>(female.mother) < females.size() &&
				females[female.mother].idx >= 0)
				person->AddRelative(RELATIVE_MOTHER, citizens[females[female.mother].idx]);
			if (female.father >= 0 && static_cast<size_t>(female.father) < males.size() &&
				males[female.father].idx >= 0)
				person->AddRelative(RELATIVE_FATHER, citizens[males[female.father].idx]);
			if (female.spouse >= 0 && static_cast<size_t>(female.spouse) < males.size() &&
				males[female.spouse].idx >= 0) {
				person->AddRelative(RELATIVE_HUSBAND, citizens[males[female.spouse].idx]);
			}
			for (const auto& [gender, childIdx] : female.childs) {
				if (gender == GENDER_FEMALE) {
					if (static_cast<size_t>(childIdx) < females.size() &&
						females[childIdx].idx >= 0)
						person->AddRelative(RELATIVE_DAUGHTER,
							citizens[females[childIdx].idx]);
				}
				else {
					if (static_cast<size_t>(childIdx) < males.size() &&
						males[childIdx].idx >= 0)
						person->AddRelative(RELATIVE_SON, citizens[males[childIdx].idx]);
				}
			}
		}
	}
	for (const auto& male : males) {
		if (male.idx >= 0) {
			Person* person = citizens[male.idx];
			if (male.mother >= 0 && static_cast<size_t>(male.mother) < females.size() &&
				females[male.mother].idx >= 0)
				person->AddRelative(RELATIVE_MOTHER, citizens[females[male.mother].idx]);
			if (male.father >= 0 && static_cast<size_t>(male.father) < males.size() &&
				males[male.father].idx >= 0)
				person->AddRelative(RELATIVE_FATHER, citizens[males[male.father].idx]);
			if (male.spouse >= 0 && static_cast<size_t>(male.spouse) < females.size() &&
				females[male.spouse].idx >= 0) {
				person->AddRelative(RELATIVE_WIFE, citizens[females[male.spouse].idx]);
			}
			for (const auto& [gender, childIdx] : male.childs) {
				if (gender == GENDER_FEMALE) {
					if (static_cast<size_t>(childIdx) < females.size() &&
						females[childIdx].idx >= 0)
						person->AddRelative(RELATIVE_DAUGHTER,
							citizens[females[childIdx].idx]);
				}
				else {
					if (static_cast<size_t>(childIdx) < males.size() &&
						males[childIdx].idx >= 0)
						person->AddRelative(RELATIVE_SON, citizens[males[childIdx].idx]);
				}
			}
		}
	}
	debugf("Generate %d citizens.\n", citizens.size());
}

void Populace::GenerateEducations(Time* time) {
	if (!time) {
		THROW_EXCEPTION(NullPointerException, "Time pointer is null in GenerateEducations.\n");
	}
	enum EducationLevel : int {
		EDUCATION_PRIMARY,
		EDUCATION_JUNIOR,
		EDUCATION_SENIOR,
		EDUCATION_COLLEGE,
		EDUCATION_POST,
		EDUCATION_END
	};

	struct SchoolClass {
		string schoolName;
		EducationLevel level;
		int startYear;
		int grade;
		vector<int> students;
		int teacher = -1;
	};
	vector<SchoolClass> levelClasses[EDUCATION_END];

	// 从120年前开始模拟
	mt19937 generator(random_device{}());
	vector<int> levelPotentials[EDUCATION_END];
	for (int year = time->GetYear() - 120; year <= time->GetYear(); year++) {
		// 班级变动
		for (int level = EDUCATION_PRIMARY; level <= EDUCATION_POST; level++) {
			for (auto& cls : levelClasses[level]) {
				Time begin(year - 1, 9, 1);
				Time end(year, 6, 30);

				for (int student : cls.students) {
					if (student >= 0 && student < static_cast<int>(citizens.size()) &&
						citizens[student]) {
						EducationExperience eduExp;
						eduExp.SetSchool(cls.schoolName);
						eduExp.SetTime(begin, end);
						citizens[student]->AddEducationExperience(eduExp);
					}
				}
			}

			for (auto& cls : levelClasses[level]) {
				for (auto studentIt = cls.students.begin();
					studentIt != cls.students.end(); ) {
					int fate = GetRandom(100);

					if (fate > 95) { // 转学
						vector<SchoolClass*> targetClasses;
						for (auto& target : levelClasses[level]) {
							if (&target != &cls && target.grade == cls.grade &&
								target.startYear == cls.startYear) {
								targetClasses.push_back(&target);
							}
						}
						if (!targetClasses.empty()) {
							SchoolClass* targetClass =
								targetClasses[GetRandom(static_cast<int>(targetClasses.size()))];
							targetClass->students.push_back(*studentIt);
							studentIt = cls.students.erase(studentIt);
						}
						else {
							++studentIt;
						}
					}
					else if (fate > 92) { // 留级
						vector<SchoolClass*> targetClasses;
						for (auto& target : levelClasses[level]) {
							if (&target != &cls && target.grade == cls.grade - 1 &&
								target.schoolName == cls.schoolName) {
								targetClasses.push_back(&target);
							}
						}
						if (!targetClasses.empty()) {
							SchoolClass* targetClass =
								targetClasses[GetRandom(static_cast<int>(targetClasses.size()))];
							targetClass->students.push_back(*studentIt);
							studentIt = cls.students.erase(studentIt);
						}
						else {
							++studentIt;
						}
					}
					else if (fate > 90) { // 跳级
						bool valid = false;
						if (level == EDUCATION_PRIMARY && cls.grade < 6) {
							valid = true;
						}
						if (level == EDUCATION_JUNIOR && cls.grade < 3) {
							valid = true;
						}
						if (level == EDUCATION_SENIOR && cls.grade < 3) {
							valid = true;
						}
						if (level == EDUCATION_COLLEGE && cls.grade < 4) {
							valid = true;
						}

						if (valid) {
							vector<SchoolClass*> targetClasses;
							for (auto& target : levelClasses[level]) {
								if (target.grade == cls.grade + 1 &&
									target.schoolName == cls.schoolName) {
									targetClasses.push_back(&target);
								}
							}
							if (!targetClasses.empty()) {
								SchoolClass* targetClass =
									targetClasses[GetRandom(static_cast<int>(targetClasses.size()))];
								targetClass->students.push_back(*studentIt);
								studentIt = cls.students.erase(studentIt);
							}
							else {
								++studentIt;
							}
						}
						else {
							++studentIt;
						}
					}
					else {
						++studentIt;
					}
				}

				cls.grade++;
				if (level < EDUCATION_POST) {
					int maxGrade;
					float continueRatio;
					float stayRatio;
					if (level == EDUCATION_PRIMARY) {
						maxGrade = 6;
						continueRatio = 0.98f;
						stayRatio = 0.99f;
					}
					else if (level == EDUCATION_JUNIOR) {
						maxGrade = 3;
						continueRatio = 0.9f;
						stayRatio = 0.98f;
					}
					else if (level == EDUCATION_SENIOR) {
						maxGrade = 3;
						continueRatio = 0.7f;
						stayRatio = 0.96f;
					}
					else { // EDUCATION_COLLEGE
						maxGrade = 4;
						continueRatio = 0.3f;
						stayRatio = 0.92f;
					}

					if (cls.grade > maxGrade) {
						size_t newSize = static_cast<size_t>(cls.students.size() *
							continueRatio);
						for (size_t j = newSize; j < cls.students.size(); j++) {
							int student = cls.students[j];
							if (student >= 0 && student < static_cast<int>(citizens.size()) &&
								citizens[student]) {
								levelPotentials[level + 1].push_back(student);
							}
						}
						cls.students.resize(newSize);
					}
					else {
						float ratio = (100 - GetRandom(static_cast<int>(
							(1.0f - stayRatio) * 100))) / 100.0f;
						size_t newSize = static_cast<size_t>(cls.students.size() * ratio);
						for (size_t j = newSize; j < cls.students.size(); j++) {
							int student = cls.students[j];
							if (student >= 0 && student < static_cast<int>(citizens.size()) &&
								citizens[student]) {
								auto& exps = citizens[student]->GetEducationExperiences();
								if (!exps.empty()) {
									exps.back().SetGraduate(false);
								}
							}
						}
						cls.students.resize(newSize);
					}
				}
				else { // EDUCATION_POST
					for (auto studentIt = cls.students.begin();
						studentIt != cls.students.end(); ) {
						if (cls.grade > 2 && GetRandom(7 - cls.grade) == 0) {
							studentIt = cls.students.erase(studentIt);
						}
						else {
							++studentIt;
						}
					}
				}
			}

			auto& classes = levelClasses[level];
			classes.erase(
				remove_if(classes.begin(), classes.end(),
					[](const SchoolClass& c) { return c.students.empty(); }),
				classes.end());
		}

		// 适龄小学生
		for (int i = 0; i < static_cast<int>(citizens.size()); i++) {
			if (!citizens[i]) continue;
			int birthYear = citizens[i]->GetBirthday().GetYear();
			if (birthYear < year - 5) {
				if (GetRandom(9 + birthYear - year) == 0 &&
					citizens[i]->GetEducationExperiences().empty()) {
					levelPotentials[EDUCATION_PRIMARY].push_back(i);
				}
			}
		}

		// 小学入学
		if (!levelPotentials[EDUCATION_PRIMARY].empty()) {
			int classCount = max(1,
				static_cast<int>(levelPotentials[EDUCATION_PRIMARY].size()) /
				(20 + GetRandom(20)));
			if (classCount == 0) classCount = 1;

			int schoolCount = max(1, classCount / 5);
			vector<string> schoolNames;
			for (int i = 0; i < schoolCount; i++) {
				schoolNames.push_back("第" + to_string(i + 1) + "小学");
			}
			shuffle(levelPotentials[EDUCATION_PRIMARY].begin(),
				levelPotentials[EDUCATION_PRIMARY].end(), generator);
			int studentsPerClass =
				static_cast<int>(levelPotentials[EDUCATION_PRIMARY].size()) / classCount;

			for (int i = 0; i < classCount; i++) {
				SchoolClass newClass;
				newClass.schoolName = schoolNames[i % schoolNames.size()];
				newClass.startYear = year;
				newClass.grade = 1;
				newClass.level = EDUCATION_PRIMARY;

				int startIdx = i * studentsPerClass;
				int endIdx = (i == classCount - 1) ?
					static_cast<int>(levelPotentials[EDUCATION_PRIMARY].size()) :
					(i + 1) * studentsPerClass;
				for (int j = startIdx; j < endIdx; j++) {
					newClass.students.push_back(levelPotentials[EDUCATION_PRIMARY][j]);
				}
				levelClasses[EDUCATION_PRIMARY].push_back(newClass);
			}
			levelPotentials[EDUCATION_PRIMARY].clear();
		}

		// 初中入学
		if (!levelPotentials[EDUCATION_JUNIOR].empty()) {
			int classCount = max(1,
				static_cast<int>(levelPotentials[EDUCATION_JUNIOR].size()) /
				(20 + GetRandom(20)));
			if (classCount == 0) classCount = 1;

			int schoolCount = max(1, classCount / 4);
			vector<string> schoolNames;
			for (int i = 0; i < schoolCount; i++) {
				schoolNames.push_back("第" + to_string(i + 1) + "初中");
			}

			shuffle(levelPotentials[EDUCATION_JUNIOR].begin(),
				levelPotentials[EDUCATION_JUNIOR].end(), generator);
			int studentsPerClass =
				static_cast<int>(levelPotentials[EDUCATION_JUNIOR].size()) / classCount;

			for (int i = 0; i < classCount; i++) {
				SchoolClass newClass;
				newClass.schoolName = schoolNames[i % schoolNames.size()];
				newClass.startYear = year;
				newClass.grade = 1;
				newClass.level = EDUCATION_JUNIOR;

				int startIdx = i * studentsPerClass;
				int endIdx = (i == classCount - 1) ?
					static_cast<int>(levelPotentials[EDUCATION_JUNIOR].size()) :
					(i + 1) * studentsPerClass;
				for (int j = startIdx; j < endIdx; j++) {
					newClass.students.push_back(levelPotentials[EDUCATION_JUNIOR][j]);
				}
				levelClasses[EDUCATION_JUNIOR].push_back(newClass);
			}
			levelPotentials[EDUCATION_JUNIOR].clear();
		}

		// 高中入学
		if (!levelPotentials[EDUCATION_SENIOR].empty()) {
			int classCount = max(1,
				static_cast<int>(levelPotentials[EDUCATION_SENIOR].size()) /
				(30 + GetRandom(20)));
			if (classCount == 0) classCount = 1;

			int schoolCount = max(1, classCount / 4);
			vector<string> schoolNames;
			for (int i = 0; i < schoolCount; i++) {
				schoolNames.push_back("第" + to_string(i + 1) + "高中");
			}

			shuffle(levelPotentials[EDUCATION_SENIOR].begin(),
				levelPotentials[EDUCATION_SENIOR].end(), generator);
			int studentsPerClass =
				static_cast<int>(levelPotentials[EDUCATION_SENIOR].size()) / classCount;

			for (int i = 0; i < classCount; i++) {
				SchoolClass newClass;
				newClass.schoolName = schoolNames[i % schoolNames.size()];
				newClass.startYear = year;
				newClass.grade = 1;
				newClass.level = EDUCATION_SENIOR;

				int startIdx = i * studentsPerClass;
				int endIdx = (i == classCount - 1) ?
					static_cast<int>(levelPotentials[EDUCATION_SENIOR].size()) :
					(i + 1) * studentsPerClass;
				for (int j = startIdx; j < endIdx; j++) {
					newClass.students.push_back(levelPotentials[EDUCATION_SENIOR][j]);
				}
				levelClasses[EDUCATION_SENIOR].push_back(newClass);
			}
			levelPotentials[EDUCATION_SENIOR].clear();
		}

		// 大学入学
		if (!levelPotentials[EDUCATION_COLLEGE].empty()) {
			int majorCount = 5 + GetRandom(5);
			vector<string> schoolNames;
			for (int i = 0; i < majorCount; i++) {
				schoolNames.push_back("第" + to_string(i + 1) + "大学");
			}

			shuffle(levelPotentials[EDUCATION_COLLEGE].begin(),
				levelPotentials[EDUCATION_COLLEGE].end(), generator);
			int studentsPerMajor =
				static_cast<int>(levelPotentials[EDUCATION_COLLEGE].size()) / majorCount;

			for (int i = 0; i < majorCount; i++) {
				SchoolClass newClass;
				newClass.schoolName = schoolNames[i];
				newClass.startYear = year;
				newClass.grade = 1;
				newClass.level = EDUCATION_COLLEGE;

				int startIdx = i * studentsPerMajor;
				int endIdx = (i == majorCount - 1) ?
					static_cast<int>(levelPotentials[EDUCATION_COLLEGE].size()) :
					(i + 1) * studentsPerMajor;
				for (int j = startIdx; j < endIdx; j++) {
					newClass.students.push_back(levelPotentials[EDUCATION_COLLEGE][j]);
				}
				levelClasses[EDUCATION_COLLEGE].push_back(newClass);
			}
			levelPotentials[EDUCATION_COLLEGE].clear();
		}

		// 研究生入学
		if (!levelPotentials[EDUCATION_POST].empty()) {
			int majorCount = 3 + GetRandom(3);
			vector<string> schoolNames;
			for (int i = 0; i < majorCount; i++) {
				schoolNames.push_back("第" + to_string(i + 1) + "研究院");
			}

			shuffle(levelPotentials[EDUCATION_POST].begin(),
				levelPotentials[EDUCATION_POST].end(), generator);
			int studentsPerMajor =
				static_cast<int>(levelPotentials[EDUCATION_POST].size()) / majorCount;

			for (int i = 0; i < majorCount; i++) {
				SchoolClass newClass;
				newClass.schoolName = schoolNames[i];
				newClass.startYear = year;
				newClass.grade = 1;
				newClass.level = EDUCATION_POST;

				int startIdx = i * studentsPerMajor;
				int endIdx = (i == majorCount - 1) ?
					static_cast<int>(levelPotentials[EDUCATION_POST].size()) :
					(i + 1) * studentsPerMajor;
				for (int j = startIdx; j < endIdx; j++) {
					newClass.students.push_back(levelPotentials[EDUCATION_POST][j]);
				}
				levelClasses[EDUCATION_POST].push_back(newClass);
			}
			levelPotentials[EDUCATION_POST].clear();
		}
	}
	for (auto citizen : citizens) {
		if (citizen) {
			citizen->ExperienceComposition();
		}
	}
	debugf("Generate educations.\n");
}

void Populace::GenerateEmotions(Time* time) {
	if (!time) {
		THROW_EXCEPTION(NullPointerException, "Time pointer is null in GenerateEmotions.\n");
	}
	for (auto citizen : citizens) {
		if (!citizen) continue;
		// 计算本人情感经历范围
		int birthYear = citizen->GetBirthday().GetYear();
		int currentAge = time->GetYear() - birthYear;
		if (currentAge < 16) continue;

		Time startBound = citizen->GetBirthday() + Time(16, 1, 1);
		Time endBound;
		Person* spouse = citizen->GetSpouse();
		if (spouse) {
			endBound = citizen->GetMarryday();
		}
		else {
			endBound = *time;
		}
		if (startBound >= endBound) continue;

		int maxRelationships = min(10, (endBound - startBound).GetYear() / 3 + 1);
		int relationshipCount = GetRandom(maxRelationships + 1);
		relationshipCount = max(relationshipCount -
			static_cast<int>(citizen->GetEmotionExperiences().size()), 0);

		// 随机生成指定段数的经历
		vector<EmotionExperience> newEmotions;
		vector<pair<Time, Time>> allocatedPeriods;
		for (int i = 0; i < relationshipCount; i++) {
			Time startTime, endTime;
			bool validPeriod = false;
			int attempts = 0;

			// 首先生成一段符合本人时间段的经历
			while (!validPeriod && attempts < 100) {
				attempts++;

				startTime = GetRandom(startBound, endBound);

				int minDuration = 1;
				int maxDuration = Time::DaysBetween(startTime, endBound) + 1;
				float r = GetRandom(1000) / 1000.0f;
				int durationDays = minDuration +
					static_cast<int>((maxDuration - minDuration + 1) * pow(r, 4.f));
				endTime = startTime;
				endTime.AddDays(durationDays - 1);
				if (endTime > endBound) endTime = endBound;

				validPeriod = true;
				for (const auto& [pStart, pEnd] : allocatedPeriods) {
					if (!(endTime < pStart || startTime > pEnd)) {
						validPeriod = false;
						break;
					}
				}

				if (validPeriod) {
					for (const auto& exp : citizen->GetEmotionExperiences()) {
						if (!(endTime < exp.GetBeginTime() || startTime > exp.GetEndTime())) {
							validPeriod = false;
							break;
						}
					}
				}
			}
			if (!validPeriod) continue;

			// 随机寻找伴侣并判断该经历是否符合其时间段
			Person* partner = nullptr;
			int candidateAttempts = 0;
			while (!partner && candidateAttempts < 100) {
				candidateAttempts++;

				int idx = GetRandom(static_cast<int>(citizens.size()));
				Person* candidate = citizens[idx];
				if (!candidate) continue;
				if (candidate->GetGender() == citizen->GetGender()) continue;
				if (candidate == citizen) continue;

				bool isCloseRelative = false;
				if (candidate == citizen->GetSpouse()) isCloseRelative = true;
				if (citizen->GetFather()) {
					for (auto relative : citizen->GetFather()->GetChilds()) {
						if (candidate == relative) {
							isCloseRelative = true;
							break;
						}
					}
				}
				if (citizen->GetMother()) {
					for (auto relative : citizen->GetMother()->GetChilds()) {
						if (candidate == relative) {
							isCloseRelative = true;
							break;
						}
					}
				}
				if (isCloseRelative) continue;

				int candidateBirthYear = candidate->GetBirthday().GetYear();
				int candidateAge = startTime.GetYear() - candidateBirthYear;
				int ageDiff = abs(birthYear - candidateBirthYear);
				if (candidateAge < 16 || ageDiff > 10) continue;

				bool timeAvailable = true;
				for (const auto& exp : candidate->GetEmotionExperiences()) {
					if (!(endTime < exp.GetBeginTime() || startTime > exp.GetEndTime())) {
						timeAvailable = false;
						break;
					}
				}
				if (!timeAvailable) continue;

				Person* candidateSpouse = candidate->GetSpouse();
				if (candidateSpouse) {
					Time marryTime = candidate->GetMarryday();
					if (marryTime < endTime) {
						continue;
					}
				}
				partner = candidate;
			}
			if (!partner) continue;
			if (startTime > endTime) continue;

			// 写入经历
			EmotionExperience emotionExp;
			emotionExp.SetPerson(partner);
			emotionExp.SetTime(startTime, endTime);
			newEmotions.push_back(emotionExp);
			allocatedPeriods.emplace_back(startTime, endTime);

			EmotionExperience partnerExp;
			partnerExp.SetPerson(citizen);
			partnerExp.SetTime(startTime, endTime);
			partner->AddEmotionExperience(partnerExp);
		}

		// 将临时经历全部写入本人经历
		for (const auto& exp : newEmotions) {
			citizen->AddEmotionExperience(exp);
		}

		// 四成未婚市民存在正在进行的恋爱
		if (!spouse && GetRandom(100) < 40) {
			Person* currentPartner = nullptr;
			int candidateAttempts = 0;

			Time marrigeBegin = citizen->GetBirthday() + Time(16, 1, 1);
			for (const auto& experience : citizen->GetEmotionExperiences()) {
				if (experience.GetEndTime() > marrigeBegin)
					marrigeBegin = experience.GetEndTime();
			}
			Time marrigeTime = GetRandom(marrigeBegin, *time);
			while (!currentPartner && candidateAttempts < 100) {
				candidateAttempts++;

				int idx = GetRandom(static_cast<int>(citizens.size()));
				Person* candidate = citizens[idx];
				if (!candidate) continue;
				if (candidate->GetGender() == citizen->GetGender()) continue;
				if (candidate == citizen || candidate->GetSpouse()) continue;

				bool isCloseRelative = false;
				if (candidate == citizen->GetSpouse()) isCloseRelative = true;
				if (citizen->GetFather()) {
					for (auto relative : citizen->GetFather()->GetChilds()) {
						if (candidate == relative) {
							isCloseRelative = true;
							break;
						}
					}
				}
				if (citizen->GetMother()) {
					for (auto relative : citizen->GetMother()->GetChilds()) {
						if (candidate == relative) {
							isCloseRelative = true;
							break;
						}
					}
				}
				if (isCloseRelative) continue;

				int candidateBirthYear = candidate->GetBirthday().GetYear();
				int candidateAge = time->GetYear() - candidateBirthYear;
				int ageDiff = abs(currentAge - candidateAge);
				if (ageDiff > 10) continue;

				bool timeAvailable = true;
				for (const auto& exp : candidate->GetEmotionExperiences()) {
					if (exp.GetEndTime() >= marrigeTime) {
						timeAvailable = false;
						break;
					}
				}
				if (!timeAvailable) continue;

				currentPartner = candidate;
			}

			if (currentPartner) {
				EmotionExperience currentExp;
				currentExp.SetPerson(currentPartner);
				currentExp.SetTime(marrigeTime, *time);

				citizen->AddEmotionExperience(currentExp);

				EmotionExperience partnerExp;
				partnerExp.SetPerson(citizen);
				partnerExp.SetTime(marrigeTime, *time);
				currentPartner->AddEmotionExperience(partnerExp);
			}
		}
	}
	debugf("Generate emotions.\n");
}

void Populace::GenerateJobs() {
	debugf("Generate jobs.\n");
}