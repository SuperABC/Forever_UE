#include "StoryBase.h"

#include "GlobalBase.h"
#include "BuildingBase.h"
#include "PopulaceBase.h"
#include "TrafficBase.h"

#include "map/map.h"
#include "map/zone.h"
#include "map/building.h"
#include "map/room.h"
#include "map/component.h"
#include "populace/populace.h"
#include "populace/person.h"
#include "populace/commute.h"
#include "populace/scheduler.h"
#include "society/society.h"
#include "society/job.h"
#include "society/organization.h"
#include "story/story.h"
#include "story/script.h"
#include "common/implement.h"
#include "story/event.h"
#include "story/dialog.h"
#include "story/change.h"
#include "industry/industry.h"
#include "traffic/traffic.h"
#include "traffic/vehicle.h"
#include "player/player.h"

#include <climits>

// 正常帧率下，每次Tick最多处理的到时全局计时器数量，避免单帧处理过多计时器导致卡顿
#define MAX_TIMERS_PER_CHECK 2

// change_time等跨天时间跳变手动模拟Tick时使用，不限制单次执行的计时器/计划节点数量
#define UNLIMITED_TIMERS INT_MAX


using namespace std;

AStoryBase::AStoryBase() :
	interacting(false) {
	PrimaryActorTick.bCanEverTick = true;
}

AStoryBase::~AStoryBase() {

}

void AStoryBase::BeginPlay() {
	Super::BeginPlay();
}

void AStoryBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (!global->GetPaused() && interacting == false && dialogQueue.size() > 0) {
		interacting = true;
		auto section = dialogQueue.front();
		if (section.IsBranch()) {
			TArray<FString> options;
			for (auto option : section.GetOptions()) {
				options.Add(UTF8_TO_TCHAR(option.GetOption().data()));
			}
			UpdateDialogBranch(options);
		}
		else {
			dialogQueue.pop_front();

			auto speaking = section.GetSpeaking();
			auto speaker = get<0>(speaking);
			auto content = get<1>(speaking);
			auto label = get<2>(speaking);
			auto voice = get<3>(speaking);
			global->GetStory()->AddTalk(speaker, content);
			UpdateDialog(UTF8_TO_TCHAR(speaker.data()), UTF8_TO_TCHAR(content.data()), UTF8_TO_TCHAR(label.data()), UTF8_TO_TCHAR(voice.data()));
		}
	}

	ProcessExpiredTimers(MAX_TIMERS_PER_CHECK);
}

void AStoryBase::SetGlobal(AGlobalBase* g) {
	this->global = g;
}

void AStoryBase::AddFront(const Dialog* dialog, Script* ownerScript, const vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	for(int i = dialog->GetDialogs().size() - 1; i >= 0; i--) {
		auto section = dialog->GetDialogs()[i];
		section.SetOwnerScript(ownerScript);
		section.EvaluateText(getValues);
		dialogQueue.push_front(section);
	}
}

void AStoryBase::AddBack(const Dialog* dialog, Script* ownerScript, const vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	for (auto section : dialog->GetDialogs()) {
		section.SetOwnerScript(ownerScript);
		section.EvaluateText(getValues);
		dialogQueue.push_back(section);
	}
}

void AStoryBase::MatchEvent(Event* event, Script* script,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	if (!script) {
		return;
	}

	bool wrapped = false;
	try {
		auto actions = script->MatchEvent(event, getValues);
		auto& wrappedActions = script->WrapScript(event, actions, getValues, global->GetImplement());
		wrapped = true;

		for (auto action : wrappedActions) {
			visit([&](auto* ptr) {
				using T = decay_t<decltype(ptr)>;
				if constexpr (is_same_v<T, const Dialog*>) {
					if (ptr->GetCondition().EvaluateBool(getValues)) {
						AddBack(ptr, script, getValues);
					}
				}
				else if constexpr (is_same_v<T, const Change*>) {
					ApplyChanges({ ptr }, getValues, script);
				}
			}, action);
		}
	}
	catch (ExceptionBase& e) {
		UE_LOGFMT(LogTemp, Log, "Exception: {0}", FString(UTF8_TO_TCHAR(e.GetDetailedInfo().data())));
	}
	if (wrapped) {
		script->AutoPop();
	}
}

void AStoryBase::ApplyChanges(const vector<const Change*>& changes,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues,
	Script* ownerScript) {
	for (auto change : changes) {
		if (!change) continue;
		if (change->GetCondition().EvaluateBool(getValues)) {
			vector<Event*> events;
			for (auto e : global->GetMap()->ApplyChange(change, getValues)) events.push_back(e);
			for (auto e : global->GetPopulace()->ApplyChange(global->GetMap(), global->GetPlayer(), global->GetTraffic(), change, getValues)) events.push_back(e);
			for (auto e : global->GetSociety()->ApplyChange(change, getValues)) events.push_back(e);
			for (auto e : global->GetStory()->ApplyChange(change, getValues, ownerScript)) events.push_back(e);
			for (auto e : global->GetIndustry()->ApplyChange(change, getValues)) events.push_back(e);
			for (auto e : global->GetTraffic()->ApplyChange(change, getValues)) events.push_back(e);
			for (auto e : global->GetPlayer()->ApplyChange(change, getValues)) events.push_back(e);

			for (auto e : ApplyChange(change, getValues, ownerScript)) events.push_back(e);

			for (auto event : events) {
				Script* local = Story::CreateLocal(event, getValues);
				MatchEvent(event, ownerScript, getValues);
				if (local) { getValues.pop_back(); delete local; }
				delete event;
			}
		}
	}
}

void AStoryBase::ApplyChanges(const vector<pair<Change*, Script*>>& changes,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	for (auto& [change, ownerScript] : changes) {
		if (!change) continue;
		if (ownerScript) {
			getValues.push_back([ownerScript](const string& v) {
				return ownerScript->GetValue(v);
			});
		}
		if (change->GetCondition().EvaluateBool(getValues)) {
			vector<Event*> events;
			for (auto e : global->GetMap()->ApplyChange(change, getValues)) events.push_back(e);
			for (auto e : global->GetPopulace()->ApplyChange(global->GetMap(), global->GetPlayer(), global->GetTraffic(), change, getValues)) events.push_back(e);
			for (auto e : global->GetSociety()->ApplyChange(change, getValues)) events.push_back(e);
			for (auto e : global->GetStory()->ApplyChange(change, getValues, ownerScript)) events.push_back(e);
			for (auto e : global->GetIndustry()->ApplyChange(change, getValues)) events.push_back(e);
			for (auto e : global->GetTraffic()->ApplyChange(change, getValues)) events.push_back(e);
			for (auto e : global->GetPlayer()->ApplyChange(change, getValues)) events.push_back(e);

			for (auto e : ApplyChange(change, getValues, ownerScript)) events.push_back(e);

			for (auto event : events) {
				Script* local = Story::CreateLocal(event, getValues);
				MatchEvent(event, ownerScript, getValues);
				if (local) { getValues.pop_back(); delete local; }
				delete event;
			}
		}
		if (ownerScript) getValues.pop_back();
	}
}

vector<Event*> AStoryBase::ApplyChange(const Change* change,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues,
	Script* ownerScript) {
	vector<Event*> result;
	auto type = change->GetType();

	if (type == "global_message") {
		auto obj = dynamic_cast<const GlobalMessageChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to GlobalMessageChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetMessage());
		ScriptMessage(UTF8_TO_TCHAR(ToString(condition.EvaluateValue(getValues)).data()));
	}
	else if (type == "remove_option") {
		auto obj = dynamic_cast<const RemoveOptionChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to RemoveOptionChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetName());
		string name = ToString(condition.EvaluateValue(getValues));
		condition.ParseCondition(obj->GetOption());
		string option = ToString(condition.EvaluateValue(getValues));
		RemoveOption(UTF8_TO_TCHAR(name.data()), UTF8_TO_TCHAR(option.data()));
	}
	else if (type == "open_shop") {
		auto obj = dynamic_cast<const OpenShopChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to OpenShopChange.\n");
		}

		Condition condition;
		condition.ParseCondition(obj->GetSaler());
		string name = ToString(condition.EvaluateValue(getValues));
		auto saler = global->GetPopulace()->GetCitizen(name);
		if (!saler) {
			debugf("Warning: Saler %s not found.\n", name.data());
		}
		else if (saler->GetCurrentRoom()) {
			auto component = saler->GetCurrentRoom()->GetParentComponent();
			if (component) {
				TArray<FItem> items;
				auto values = component->GetScript()->GetValues("system\\.storage\\..*");
				for (auto [product, amount] : values) {
					items.Add(FItem(UTF8_TO_TCHAR(product.data()), 0.f, get<double>(amount)));
				}
				OpenShop(items);
			}
		}
	}
	else if (type == "start_puzzle") {
		auto obj = dynamic_cast<const StartPuzzleChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to StartPuzzleChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetPuzzle());
		string puzzle = ToString(condition.EvaluateValue(getValues));
		StartPuzzle(UTF8_TO_TCHAR(puzzle.data()));
	}
	else if(type == "teleport_player") {
		auto obj = dynamic_cast<const TeleportPlayerChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to TeleportPlayerChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetDestination());
		string destination = ToString(condition.EvaluateValue(getValues));
		auto room = global->GetMap()->LocateRoom(destination);
		if (!room) {
			debugf("Warning: Destination room %s not found.\n", destination.data());
		}
		else {
			auto pos = room->GetPosition(room->GetSizeX() / 2.f, room->GetSizeY() / 2.f);
			FVector location(pos.first, pos.second, room->GetLayer() * room->GetParentBuilding()->GetHeight());
			global->SetLocation(location);
		}
	}
	else if (type == "npc_navigate") {
		auto obj = dynamic_cast<const NPCNavigateChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to NPCNavigateChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetName());
		string name = ToString(condition.EvaluateValue(getValues));
		condition.ParseCondition(obj->GetDestination());
		string destination = ToString(condition.EvaluateValue(getValues));
		auto person = global->GetPopulace()->GetCitizen(name);
		if (!person) {
			debugf("Warning: Target citizen %s not found.\n", name.data());
		}
		else {
			auto commute = person->GetCurrentCommute();
			if (!commute) {
				debugf("Warning: No commute path available for npc_navigate (%s).\n", name.data());
			}
			else {
				auto& paths = commute->GetPaths();
				TArray<FVector> nodes;
				if (!paths.empty()) {
					auto& [firstConnection, firstReversed] = paths.front();
					Node firstPoint = firstReversed ? firstConnection->GetEnd() : firstConnection->GetStart();
					nodes.Add(1000.f * FVector(firstPoint.GetX(), firstPoint.GetY(), firstPoint.GetZ()));
					for (auto& [connection, reversed] : paths) {
						Node nextPoint = reversed ? connection->GetStart() : connection->GetEnd();
						nodes.Add(1000.f * FVector(nextPoint.GetX(), nextPoint.GetY(), nextPoint.GetZ()));
					}
				}

				if (global->GetPopulaceActor()->NpcNavigate(
					UTF8_TO_TCHAR(name.data()), nodes, UTF8_TO_TCHAR(destination.data()))) {
					commute->StartVisible();
				}
			}
		}
	}
	else if(type == "enter_battle") {
		auto obj = dynamic_cast<const EnterBattleChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to EnterBattleChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetEnemy());
		string enemy = ToString(condition.EvaluateValue(getValues));
		EnterBattle(this, UTF8_TO_TCHAR(enemy.data()));
	}
	else if (type == "enter_vehicle") {
		auto obj = dynamic_cast<const EnterVehicleChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to EnterVehicleChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetVehicle());
		string vehicle = ToString(condition.EvaluateValue(getValues));
		global->GetTrafficActor()->EnterVehicle(UTF8_TO_TCHAR(vehicle.data()));
	}
	else if (type == "leave_vehicle") {
		auto obj = dynamic_cast<const LeaveVehicleChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to LeaveVehicleChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetVehicle());
		string vehicle = ToString(condition.EvaluateValue(getValues));
		global->GetTrafficActor()->LeaveVehicle(UTF8_TO_TCHAR(vehicle.data()));
	}
	else if (type == "launch_elevator") {
		auto obj = dynamic_cast<const LaunchElevatorChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to LaunchElevatorChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetBuilding());
		string buildingName = ToString(condition.EvaluateValue(getValues));
		condition.ParseCondition(obj->GetElevator());
		string elevatorName = ToString(condition.EvaluateValue(getValues));
		condition.ParseCondition(obj->GetCommand());
		string command = ToString(condition.EvaluateValue(getValues));

		int target = 0;
		for (size_t i = 0; i < command.size(); i++) {
			if ((command[i] == 'b' || command[i] == 'B') && i + 1 < command.size() && isdigit((unsigned char)command[i + 1])) {
				target = -stoi(command.substr(i + 1));
				break;
			}
			if (command[i] == '-' && i + 1 < command.size() && isdigit((unsigned char)command[i + 1])) {
				target = -stoi(command.substr(i + 1));
				break;
			}
			if (isdigit((unsigned char)command[i])) {
				target = stoi(command.substr(i));
				break;
			}
		}

		auto buildingActor = global->GetBuildingActor();
		AActor* buildingInstance = buildingActor->GetInstance(UTF8_TO_TCHAR(buildingName.data()));
		buildingActor->LaunchElevator(buildingInstance, UTF8_TO_TCHAR(elevatorName.data()), target);
	}
	else if (type == "play_video") {
		auto obj = dynamic_cast<const PlayVideoChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to PlayVideoChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetPath());
		string path = ToString(condition.EvaluateValue(getValues));
		PlayVideo(UTF8_TO_TCHAR(path.data()));
	}
	else if (type == "play_bgm") {
		auto obj = dynamic_cast<const PlayBgmChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to PlayBgmChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetBgm());
		string bgm = ToString(condition.EvaluateValue(getValues));
		PlayBgm(UTF8_TO_TCHAR(bgm.data()), obj->GetLoop());
	}
	else if (type == "stop_bgm") {
		auto obj = dynamic_cast<const StopBgmChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to StopBgmChange.\n");
		}
		StopBgm();
	}
	else if (type == "for_range") {
		auto obj = dynamic_cast<const ForRangeChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to ForRangeChange.\n");
		}
		auto toInt = [](const ValueType& v) -> int {
			return visit([](const auto& val) -> int {
				using T = decay_t<decltype(val)>;
				if constexpr (is_same_v<T, int>) return val;
				else if constexpr (is_same_v<T, double>) return (int)val;
				else if constexpr (is_same_v<T, bool>) return val ? 1 : 0;
				else return 0;
			}, v);
		};
		Condition cond;
		cond.ParseCondition(obj->GetFrom());
		int from = toInt(cond.EvaluateValue(getValues));
		cond.ParseCondition(obj->GetTo());
		int to = toInt(cond.EvaluateValue(getValues));
		cond.ParseCondition(obj->GetStep());
		int step = toInt(cond.EvaluateValue(getValues));
		if (step == 0) step = 1;

		string varName = obj->GetVar();
		for (int i = from; step > 0 ? i <= to : i >= to; i += step) {
			ValueType loopVal = i;
			auto loopGetValue = [&varName, &loopVal](const string& name) -> pair<bool, ValueType> {
				if (name == varName) return { true, loopVal };
				return { false, ValueType{} };
			};
			vector<function<pair<bool, ValueType>(const string&)>> innerGetValues;
			innerGetValues.push_back(loopGetValue);
			innerGetValues.insert(innerGetValues.end(), getValues.begin(), getValues.end());

			ApplyChanges(obj->GetChanges(), innerGetValues, ownerScript);
		}
	}
	else if (type == "change_time") {
		auto obj = dynamic_cast<const ChangeTimeChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to ChangeTimeChange.\n");
		}

		Condition deltaCondition;
		deltaCondition.ParseCondition(obj->GetDelta());
		Time delta(ToString(deltaCondition.EvaluateValue(getValues)));

		auto player = global->GetPlayer();
		Time startTime = *player->GetTime();
		Time endTime = startTime + delta;

		// 按日期差判断中间跨越了多少天，跨天时逐天在23:59:59收尾、次日00:00:01开局各手动模拟一次Tick
		int daySpan = Time::DaysBetween(startTime, endTime);
		if (daySpan > 0) {
			Time cursor = startTime;
			for (int i = 0; i < daySpan; i++) {
				Time endOfDay = cursor;
				endOfDay.SetTime(23, 59, 59, 999);
				SimulateDayBoundary(endOfDay);

				cursor.AddDays(1);
				cursor.SetTime(0, 0, 1, 0);
				SimulateDayBoundary(cursor);
			}
		}

		// 无论是否跨天，最后都要落到目标时刻并强制执行完此刻之前到期的所有计时器，不留到之后按正常帧率节流慢慢补
		SimulateDayBoundary(endTime);
	}
	return result;
}

void AStoryBase::FinishSection() {
	interacting = false;
}

bool AStoryBase::SelectOption(FString selected) {
	auto section = dialogQueue.front();
	auto options = section.GetOptions();
	Script* ownerScript = section.GetOwnerScript();

	auto story = global->GetStory();
	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};
	if (ownerScript && ownerScript != story->GetScript()) {
		getValues.push_back([ownerScript](const string& name) -> pair<bool, ValueType> {
			return ownerScript->GetValue(name);
		});
	}

	for(auto option : options) {
		if (UTF8_TO_TCHAR(option.GetOption().data()) == selected) {
			global->GetStory()->AddTalk("", TCHAR_TO_UTF8(*selected));
			dialogQueue.pop_front();
			auto dialogs = option.GetDialogs();
			if (dialogs.size() > 0) {
				for (int i = dialogs.size() - 1; i >= 0; i--) {
					if (!dialogs[i]->GetCondition().EvaluateBool(getValues))continue;
					AddFront(dialogs[i], ownerScript, getValues);
				}
			}
			auto changes = option.GetChanges();
			ApplyChanges(changes, getValues, ownerScript);
			interacting = false;
			return true;
		}
	}

	return false;
}

TArray<FString> AStoryBase::GetOptions(FString name) {
	TArray<FString> options;
	auto populace = global->GetPopulace();
	auto citizen = populace->GetCitizen(TCHAR_TO_UTF8(*name));
	if (citizen) {
		for (auto option : citizen->GetOptions()) {
			options.Add(UTF8_TO_TCHAR(option.data()));
		}
		return options;
	}
	auto vehicle = global->GetTraffic()->GetVehicle(TCHAR_TO_UTF8(*name));
	if (vehicle) {
		for (auto option : vehicle->GetOptions()) {
			options.Add(UTF8_TO_TCHAR(option.data()));
		}
		return options;
	}

	string nameStr = TCHAR_TO_UTF8(*name);
	auto sep = nameStr.find(' ');
	if (sep != string::npos) {
		string buildingName = nameStr.substr(0, sep);
		string elevatorName = nameStr.substr(sep + 1);
		auto b = global->GetMap()->GetBuilding(buildingName);
		if (b) {
			for (auto cabin : b->GetCabins()) {
				auto selfName = cabin->GetScript()->GetValue("self.name");
				if (!selfName.first || !holds_alternative<string>(selfName.second)) continue;
				if (get<string>(selfName.second) == elevatorName) {
					for (auto option : cabin->GetOptions()) {
						options.Add(UTF8_TO_TCHAR(option.data()));
					}
					return options;
				}
			}
		}
	}

	THROW_EXCEPTION(InvalidArgumentException, string("Citizen, vehicle or cabin not found: ") + TCHAR_TO_UTF8(*name) + ".\n");
}

TArray<FString> AStoryBase::GetGlobals() {
	TArray<FString> options;
	auto populace = global->GetPopulace();
	for (auto option : populace->GetGlobalOptions()) {
		options.Add(UTF8_TO_TCHAR(option.data()));
	}
	return options;
}

void AStoryBase::GetTask(FString& task) {
	auto story = global->GetStory();
	task = UTF8_TO_TCHAR(story->GetScript()->GetTask().data());
}

FString AStoryBase::GetHistory() {
	string result;
	for (auto& [speaker, content] : global->GetStory()->GetHistory()) {
		if (!speaker.empty()) {
			result += speaker + ": " + content + "\n";
		} else {
			result += content + "\n";
		}
	}
	return UTF8_TO_TCHAR(result.data());
}

void AStoryBase::SetStatus(const FString& name, const FString& destination) {
	auto citizen = global->GetPopulace()->GetCitizen(TCHAR_TO_UTF8(*name));
	if (!citizen) return;
	auto room = global->GetMap()->LocateRoom(TCHAR_TO_UTF8(*destination));
	if (!room) return;
	citizen->SetStatus(room, false);
}

void AStoryBase::GameStart() {
	auto story = global->GetStory();
	auto event = new GameStartEvent();

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};
	MatchEvent(event, story->GetScript(), getValues);

	auto zones = global->GetMap()->GetZones();
	for (auto [_, z] : zones) {
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return z->GetScript()->GetValue(name);
			});
		MatchEvent(event, z->GetScript(), getValues);
		getValues.pop_back();
		for (auto [__, b] : z->GetBuildings()) {
			getValues.push_back(
				[&](const string& name) -> pair<bool, ValueType> {
					return b->GetScript()->GetValue(name);
				});
			MatchEvent(event, b->GetScript(), getValues);
			getValues.pop_back();
		}
	}

	auto buildings = global->GetMap()->GetBuildings();
	for (auto [_, b] : buildings) {
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return b->GetScript()->GetValue(name);
			});
		MatchEvent(event, b->GetScript(), getValues);
		getValues.pop_back();
		for (auto cabin : b->GetCabins()) {
			if (!cabin->GetScript()) continue;
			getValues.push_back(
				[&](const string& name) -> pair<bool, ValueType> {
					return cabin->GetScript()->GetValue(name);
				});
			MatchEvent(event, cabin->GetScript(), getValues);
			getValues.pop_back();
		}
	}

	for (auto [_, z] : zones) {
		for (auto [__, b] : z->GetBuildings()) {
			for (auto cabin : b->GetCabins()) {
				if (!cabin->GetScript()) continue;
				getValues.push_back(
					[&](const string& name) -> pair<bool, ValueType> {
						return cabin->GetScript()->GetValue(name);
					});
				MatchEvent(event, cabin->GetScript(), getValues);
				getValues.pop_back();
			}
		}
	}

	auto citizens = global->GetPopulace()->GetCitizens();
	for (auto citizen : citizens) {
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return citizen->GetScheduler()->GetScript()->GetValue(name);
			});
		MatchEvent(event, citizen->GetScheduler()->GetScript(), getValues);
		getValues.pop_back();
		for (auto job : citizen->GetJobs()) {
			getValues.push_back(
				[&](const string& name) -> pair<bool, ValueType> {
					return job->GetScript()->GetValue(name);
				});
			MatchEvent(event, job->GetScript(), getValues);
			getValues.pop_back();
		}
	}

	auto organizations = global->GetSociety()->GetOrganizations();
	for (auto organization : organizations) {
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return organization->GetScript()->GetValue(name);
			});
		MatchEvent(event, organization->GetScript(), getValues);
		getValues.pop_back();
	}

	auto vehicles = global->GetTraffic()->GetVehicles();
	for (auto vehicle : vehicles) {
		if (!vehicle->GetScript()) continue;
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return vehicle->GetScript()->GetValue(name);
			});
		MatchEvent(event, vehicle->GetScript(), getValues);
		getValues.pop_back();
	}

	delete event;
}

Cabin* AStoryBase::FindCabin(const string& name) {
	auto map = global->GetMap();
	for (auto& [_, building] : map->GetBuildings()) {
		for (auto cabin : building->GetCabins()) {
			if (cabin->GetName() == name) return cabin;
		}
	}
	for (auto& [_, zone] : map->GetZones()) {
		for (auto& [__, building] : zone->GetBuildings()) {
			for (auto cabin : building->GetCabins()) {
				if (cabin->GetName() == name) return cabin;
			}
		}
	}
	return nullptr;
}

void AStoryBase::ProcessExpiredTimers(int maxCount) {
	auto story = global->GetStory();
	auto now = global->GetPlayer()->GetTime();

	// 计时器按到达时间维护成小顶堆，这里直接从堆顶取最多maxCount个已到时的计时器，不用遍历全部计时器
	auto expired = story->PopExpiredTimers(*now, maxCount);

	for (auto& [name, category, label] : expired) {
		auto event = new TimeUpEvent(name);

		vector<function<pair<bool, ValueType>(const string&)>> getValues = {
			[&](const string& valueName) -> pair<bool, ValueType> {
				return story->GetScript()->GetValue(valueName);
			}
		};

		// 按计时器记录的类型与名称精确匹配脚本，不再广播给全部脚本
		if (category == "global") {
			MatchEvent(event, story->GetScript(), getValues);
		}
		else if (category == "zone") {
			auto zone = global->GetMap()->GetZone(label);
			if (zone) {
				getValues.push_back(
					[&](const string& valueName) -> pair<bool, ValueType> {
						return zone->GetScript()->GetValue(valueName);
					});
				MatchEvent(event, zone->GetScript(), getValues);
			}
		}
		else if (category == "building") {
			auto building = global->GetMap()->GetBuilding(label);
			if (building) {
				getValues.push_back(
					[&](const string& valueName) -> pair<bool, ValueType> {
						return building->GetScript()->GetValue(valueName);
					});
				MatchEvent(event, building->GetScript(), getValues);
			}
		}
		else if (category == "citizen") {
			auto citizen = global->GetPopulace()->GetCitizen(label);
			if (citizen) {
				getValues.push_back(
					[&](const string& valueName) -> pair<bool, ValueType> {
						return citizen->GetScheduler()->GetScript()->GetValue(valueName);
					});
				MatchEvent(event, citizen->GetScheduler()->GetScript(), getValues);
				getValues.pop_back();
				for (auto job : citizen->GetJobs()) {
					getValues.push_back(
						[&](const string& valueName) -> pair<bool, ValueType> {
							return job->GetScript()->GetValue(valueName);
						});
					MatchEvent(event, job->GetScript(), getValues);
					getValues.pop_back();
				}
			}
		}
		else if (category == "elevator") {
			auto cabin = FindCabin(label);
			if (cabin && cabin->GetScript()) {
				getValues.push_back(
					[&](const string& valueName) -> pair<bool, ValueType> {
						return cabin->GetScript()->GetValue(valueName);
					});
				MatchEvent(event, cabin->GetScript(), getValues);
			}
		}
		else if (category == "vehicle") {
			auto vehicle = global->GetTraffic()->GetVehicle(label);
			if (vehicle && vehicle->GetScript()) {
				getValues.push_back(
					[&](const string& valueName) -> pair<bool, ValueType> {
						return vehicle->GetScript()->GetValue(valueName);
					});
				MatchEvent(event, vehicle->GetScript(), getValues);
			}
		}
		else if (category == "organization") {
			auto organization = global->GetSociety()->GetOrganization(label);
			if (organization && organization->GetScript()) {
				getValues.push_back(
					[&](const string& valueName) -> pair<bool, ValueType> {
						return organization->GetScript()->GetValue(valueName);
					});
				MatchEvent(event, organization->GetScript(), getValues);
			}
		}
		else {
			debugf("Warning: Unknown timer category '%s' for timer %s.\n", category.data(), name.data());
		}

		delete event;
	}
}

void AStoryBase::SimulateDayBoundary(const Time& moment) {
	auto player = global->GetPlayer();
	player->SetTime(moment);

	auto story = global->GetStory();
	story->Tick(player);

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& v) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(v);
		}
	};

	// 与GlobalBase::Tick里applyAndFree的写法一致：应用职业/组织与市民日程节点产生的变化后释放
	auto applyAndFree = [&](vector<pair<Change*, Script*>> changes) {
		if (!changes.empty() && story->GetScript()) {
			ApplyChanges(changes, getValues);
		}
		for (auto& [c, s] : changes) delete c;
	};
	applyAndFree(global->GetSociety()->Tick(player, story, global->GetImplement(), UNLIMITED_TIMERS));
	applyAndFree(global->GetPopulace()->Tick(global->GetMap(), story, player, global->GetImplement(), UNLIMITED_TIMERS));

	global->GetIndustry()->Tick(player);

	ProcessExpiredTimers(UNLIMITED_TIMERS);
}

void AStoryBase::ScriptMessage(FString message) {
	auto story = global->GetStory();
	auto event = new ScriptMessageEvent(TCHAR_TO_UTF8(*message));

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};
	MatchEvent(event, story->GetScript(), getValues);

	auto zones = global->GetMap()->GetZones();
	for (auto [_, z] : zones) {
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return z->GetScript()->GetValue(name);
			});
		MatchEvent(event, z->GetScript(), getValues);
		getValues.pop_back();
		for (auto [__, b] : z->GetBuildings()) {
			getValues.push_back(
				[&](const string& name) -> pair<bool, ValueType> {
					return b->GetScript()->GetValue(name);
				});
			MatchEvent(event, b->GetScript(), getValues);
			getValues.pop_back();
		}
	}

	auto buildings = global->GetMap()->GetBuildings();
	for (auto [_, b] : buildings) {
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return b->GetScript()->GetValue(name);
			});
		MatchEvent(event, b->GetScript(), getValues);
		getValues.pop_back();
	}

	auto citizens = global->GetPopulace()->GetCitizens();
	for (auto citizen : citizens) {
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return citizen->GetScheduler()->GetScript()->GetValue(name);
			});
		MatchEvent(event, citizen->GetScheduler()->GetScript(), getValues);
		getValues.pop_back();
		for (auto job : citizen->GetJobs()) {
			getValues.push_back(
				[&](const string& name) -> pair<bool, ValueType> {
					return job->GetScript()->GetValue(name);
				});
			MatchEvent(event, job->GetScript(), getValues);
			getValues.pop_back();
		}
	}

	auto vehicles = global->GetTraffic()->GetVehicles();
	for (auto vehicle : vehicles) {
		if (!vehicle->GetScript()) continue;
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return vehicle->GetScript()->GetValue(name);
			});
		MatchEvent(event, vehicle->GetScript(), getValues);
		getValues.pop_back();
	}

	delete event;
}

void AStoryBase::OptionDialog(FString name, FString option) {
	auto story = global->GetStory();
	auto event = new OptionDialogEvent(TCHAR_TO_UTF8(*name), TCHAR_TO_UTF8(*option));

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};
	Script* local = Story::CreateLocal(event, getValues);
	MatchEvent(event, story->GetScript(), getValues);

	auto citizen = global->GetPopulace()->GetCitizen(TCHAR_TO_UTF8(*name));
	if (citizen) {
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return citizen->GetScheduler()->GetScript()->GetValue(name);
			});
		MatchEvent(event, citizen->GetScheduler()->GetScript(), getValues);
		getValues.pop_back();
		for (auto job : citizen->GetJobs()) {
			getValues.push_back(
				[&](const string& name) -> pair<bool, ValueType> {
					return job->GetScript()->GetValue(name);
				});
			MatchEvent(event, job->GetScript(), getValues);
			getValues.pop_back();
		}
	}

	auto vehicle = global->GetTraffic()->GetVehicle(TCHAR_TO_UTF8(*name));
	if (vehicle) {
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return vehicle->GetScript()->GetValue(name);
			});
		MatchEvent(event, vehicle->GetScript(), getValues);
		getValues.pop_back();
	}

	string nameStr = TCHAR_TO_UTF8(*name);
	auto sep2 = nameStr.find(' ');
	if (sep2 != string::npos) {
		string buildingName = nameStr.substr(0, sep2);
		string elevatorName = nameStr.substr(sep2 + 1);
		auto b = global->GetMap()->GetBuilding(buildingName);
		if (b) {
			for (auto cabin : b->GetCabins()) {
				auto selfName = cabin->GetScript()->GetValue("self.name");
				if (!selfName.first || !holds_alternative<string>(selfName.second)) continue;
				if (get<string>(selfName.second) == elevatorName) {
					getValues.push_back(
						[&](string varName) -> pair<bool, ValueType> {
							return cabin->GetScript()->GetValue(varName);
						});
					MatchEvent(event, cabin->GetScript(), getValues);
					getValues.pop_back();
				}
			}
		}
	}

	delete local;
	delete event;
}

void AStoryBase::GlobalDialog(FString name, FString option) {
	auto story = global->GetStory();
	auto event = new GlobalDialogEvent(TCHAR_TO_UTF8(*name), TCHAR_TO_UTF8(*option));

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};
	Script* local = Story::CreateLocal(event, getValues);
	MatchEvent(event, story->GetScript(), getValues);
	if (local) { getValues.pop_back(); delete local; }

	delete event;
}

void AStoryBase::SpeakingFinish(FString label) {
	auto story = global->GetStory();
	auto event = new SpeakingFinishEvent(TCHAR_TO_UTF8(*label));

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};
	MatchEvent(event, story->GetScript(), getValues);

	auto citizens = global->GetPopulace()->GetCitizens();
	for (auto citizen : citizens) {
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return citizen->GetScheduler()->GetScript()->GetValue(name);
			});
		MatchEvent(event, citizen->GetScheduler()->GetScript(), getValues);
		getValues.pop_back();
		for (auto job : citizen->GetJobs()) {
			getValues.push_back(
				[&](const string& name) -> pair<bool, ValueType> {
					return job->GetScript()->GetValue(name);
				});
			MatchEvent(event, job->GetScript(), getValues);
			getValues.pop_back();
		}
	}

	delete event;
}

void AStoryBase::BattleWin(FString enemy) {
	auto story = global->GetStory();
	auto event = new BattleWinEvent(TCHAR_TO_UTF8(*enemy));

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};
	MatchEvent(event, story->GetScript(), getValues);

	auto citizens = global->GetPopulace()->GetCitizens();
	for (auto citizen : citizens) {
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return citizen->GetScheduler()->GetScript()->GetValue(name);
			});
		MatchEvent(event, citizen->GetScheduler()->GetScript(), getValues);
		getValues.pop_back();
		for (auto job : citizen->GetJobs()) {
			getValues.push_back(
				[&](const string& name) -> pair<bool, ValueType> {
					return job->GetScript()->GetValue(name);
				});
			MatchEvent(event, job->GetScript(), getValues);
			getValues.pop_back();
		}
	}

	delete event;
}

void AStoryBase::BattleLose(FString enemy) {
	auto story = global->GetStory();
	auto event = new BattleLoseEvent(TCHAR_TO_UTF8(*enemy));

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};
	MatchEvent(event, story->GetScript(), getValues);

	auto citizens = global->GetPopulace()->GetCitizens();
	for (auto citizen : citizens) {
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return citizen->GetScheduler()->GetScript()->GetValue(name);
			});
		MatchEvent(event, citizen->GetScheduler()->GetScript(), getValues);
		getValues.pop_back();
		for (auto job : citizen->GetJobs()) {
			getValues.push_back(
				[&](const string& name) -> pair<bool, ValueType> {
					return job->GetScript()->GetValue(name);
				});
			MatchEvent(event, job->GetScript(), getValues);
			getValues.pop_back();
		}
	}

	delete event;
}

void AStoryBase::PuzzleResult(int result) {
	auto story = global->GetStory();
	auto event = new PuzzleResultEvent(result);

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};
	Script* local = Story::CreateLocal(event, getValues);
	MatchEvent(event, story->GetScript(), getValues);

	auto citizens = global->GetPopulace()->GetCitizens();
	for (auto citizen : citizens) {
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return citizen->GetScheduler()->GetScript()->GetValue(name);
			});
		MatchEvent(event, citizen->GetScheduler()->GetScript(), getValues);
		getValues.pop_back();
		for (auto job : citizen->GetJobs()) {
			getValues.push_back(
				[&](const string& name) -> pair<bool, ValueType> {
					return job->GetScript()->GetValue(name);
				});
			MatchEvent(event, job->GetScript(), getValues);
			getValues.pop_back();
		}
	}

	delete local;
	delete event;
}

void AStoryBase::NpcArrive(const FString& name, const FString& destination) {
	auto story = global->GetStory();

	auto event = new NpcArriveEvent(TCHAR_TO_UTF8(*name), TCHAR_TO_UTF8(*destination));

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& n) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(n);
		}
	};
	MatchEvent(event, story->GetScript(), getValues);

	auto citizen = global->GetPopulace()->GetCitizen(TCHAR_TO_UTF8(*name));
	if (citizen) {
		auto schedulerScript = citizen->GetScheduler()->GetScript();
		getValues.push_back(
			[&](const string& n) -> pair<bool, ValueType> {
				return schedulerScript->GetValue(n);
			});
		MatchEvent(event, schedulerScript, getValues);
		getValues.pop_back();

		for (auto job : citizen->GetJobs()) {
			auto jobScript = job->GetScript();
			getValues.push_back(
				[&](const string& n) -> pair<bool, ValueType> {
					return jobScript->GetValue(n);
				});
			MatchEvent(event, jobScript, getValues);
			getValues.pop_back();
		}
	}

	delete event;
}

