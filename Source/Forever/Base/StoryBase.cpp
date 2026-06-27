#include "StoryBase.h"

#include "GlobalBase.h"
#include "BuildingBase.h"
#include "PopulaceBase.h"
#include "TrafficBase.h"

#include "common/trivial.h"
#include "map/map.h"
#include "map/block.h"
#include "map/zone.h"
#include "map/building.h"
#include "map/room.h"
#include "map/component.h"
#include "populace/populace.h"
#include "populace/person.h"
#include "populace/scheduler.h"
#include "society/society.h"
#include "society/job.h"
#include "story/story.h"
#include "story/script.h"
#include "story/event.h"
#include "story/dialog.h"
#include "story/change.h"
#include "industry/industry.h"
#include "traffic/traffic.h"
#include "traffic/vehicle.h"
#include "player/player.h"


using namespace std;

AStoryBase::AStoryBase() {
	PrimaryActorTick.bCanEverTick = true;
	interacting = false;
}

AStoryBase::~AStoryBase() {

}

void AStoryBase::BeginPlay() {
	Super::BeginPlay();
}

void AStoryBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (interacting == false && dialogQueue.size() > 0) {
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
			global->GetStory()->AddTalk(speaker, content);
			UpdateDialog(UTF8_TO_TCHAR(speaker.data()), UTF8_TO_TCHAR(content.data()), UTF8_TO_TCHAR(label.data()));
		}
	}
}

void AStoryBase::SetGlobal(AGlobalBase* g) {
	this->global = g;
}

void AStoryBase::AddFront(Dialog* dialog, Script* ownerScript, const vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	for(int i = dialog->GetDialogs().size() - 1; i >= 0; i--) {
		auto section = dialog->GetDialogs()[i];
		section.SetOwnerScript(ownerScript);
		section.EvaluateText(getValues);
		dialogQueue.push_front(section);
	}
}

void AStoryBase::AddBack(Dialog* dialog, Script* ownerScript, const vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
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

	try {
		vector<ScriptAction> actions;
		auto pres = script->PreTrigger(event);
		actions.insert(actions.end(), pres.begin(), pres.end());
		auto matches = script->MatchEvent(event, getValues);
		actions.insert(actions.end(), matches.begin(), matches.end());
		auto posts = script->PostTrigger(event);
		actions.insert(actions.end(), posts.begin(), posts.end());

		for (auto action : actions) {
			visit([&](auto* ptr) {
				if constexpr (is_same_v<decltype(ptr), Dialog*>) {
					auto* dialog = dynamic_cast<Dialog*>(ptr);
					if (dialog->GetCondition().EvaluateBool(getValues)) {
						AddBack(dialog, script, getValues);
					}
				}
				else if constexpr (is_same_v<decltype(ptr), Change*>) {
					auto* change = dynamic_cast<Change*>(ptr);
					if (change->GetCondition().EvaluateBool(getValues)) {
						global->GetMap()->ApplyChange(change, getValues);
						global->GetPopulace()->ApplyChange(global->GetMap(), change, getValues);
						global->GetSociety()->ApplyChange(change, getValues);
						global->GetStory()->ApplyChange(change, getValues, script);
						global->GetIndustry()->ApplyChange(change, getValues);
						global->GetTraffic()->ApplyChange(change, getValues);
						global->GetPlayer()->ApplyChange(change, getValues);

						ApplyChange(change, getValues, script);
					}
				}
			}, action);
		}
	}
	catch (ExceptionBase& e) {
		UE_LOGFMT(LogTemp, Log, "Exception: {0}", FString(UTF8_TO_TCHAR(e.GetDetailedInfo().data())));
	}
}

void AStoryBase::ApplyChange(Change* change,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues,
	Script* ownerScript) {
	auto type = change->GetType();

	if (type == "global_message") {
		auto obj = dynamic_cast<GlobalMessageChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to GlobalMessageChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetMessage());
		ScriptMessage(UTF8_TO_TCHAR(ToString(condition.EvaluateValue(getValues)).data()));
	}
	else if (type == "remove_option") {
		auto obj = dynamic_cast<RemoveOptionChange*>(change);
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
		auto obj = dynamic_cast<OpenShopChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to OpenShopChange.\n");
		}

		Condition condition;
		condition.ParseCondition(obj->GetSaler());
		string name = ToString(condition.EvaluateValue(getValues));
		auto saler = global->GetPopulace()->GetCitizen(name);
		if (saler->GetWork()) {
			auto component = saler->GetWork()->GetPosition()->GetParentComponent();
			TArray<FItem> items;
			auto values = component->GetScript()->GetValues("system\\.storage\\..*");
			for (auto [product, amount] : values) {
				items.Add(FItem(UTF8_TO_TCHAR(product.data()), 0.f, get<double>(amount)));
			}
			OpenShop(items);
		}
	}
	else if (type == "start_puzzle") {
		auto obj = dynamic_cast<StartPuzzleChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to StartPuzzleChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetPuzzle());
		string puzzle = ToString(condition.EvaluateValue(getValues));
		StartPuzzle(UTF8_TO_TCHAR(puzzle.data()));
	}
	else if(type == "teleport_player") {
		auto obj = dynamic_cast<TeleportPlayerChange*>(change);
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
		auto obj = dynamic_cast<NPCNavigateChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to NPCNavigateChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetName());
		string name = ToString(condition.EvaluateValue(getValues));
		auto person = global->GetPopulace()->GetCitizen(name);
		if (!person) {
			debugf("Warning: Target citizen %s not found.\n", name.data());
		}
		else {
			condition.ParseCondition(obj->GetDestination());
			string destination = ToString(condition.EvaluateValue(getValues));

			auto map = global->GetMap();
			Room* destRoom = map->LocateRoom(destination);
			Building* destBuilding = destRoom ? nullptr : map->LocateBuilding(destination);
			Zone* destZone = (destRoom || destBuilding) ? nullptr : map->LocateZone(destination);
			Block* destBlock = (destRoom || destBuilding || destZone) ? nullptr : map->LocateBlock(destination);

			Node* startNode = ResolveNavigationNode(person->GetCurrentRoom(), person->GetCurrentBuilding(),
				person->GetCurrentZone(), person->GetCurrentBlock());
			Node* endNode = ResolveNavigationNode(destRoom, destBuilding, destZone, destBlock);

			if (!startNode || !endNode) {
				debugf("Warning: Failed to resolve navigation node for npc_navigate (%s -> %s).\n",
					name.data(), destination.data());
			}
			else {
				auto path = map->AutoNavigation(startNode->GetId(), endNode->GetId());

				// AutoNavigation返回的Connection朝向与路径方向无关，按当前节点id逐段确定下一个端点
				TArray<FVector> nodes;
				nodes.Add(1000.f * FVector(startNode->GetX(), startNode->GetY(), startNode->GetZ()));
				int currentId = startNode->GetId();
				for (auto connection : path) {
					Node next = connection->GetStart().GetId() == currentId ? connection->GetEnd() : connection->GetStart();
					nodes.Add(1000.f * FVector(next.GetX(), next.GetY(), next.GetZ()));
					currentId = next.GetId();
				}

				global->GetPopulaceActor()->NpcNavigate(UTF8_TO_TCHAR(name.data()), nodes);
			}
		}
	}
	else if(type == "enter_battle") {
		auto obj = dynamic_cast<EnterBattleChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to EnterBattleChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetEnemy());
		string enemy = ToString(condition.EvaluateValue(getValues));
		EnterBattle(this, UTF8_TO_TCHAR(enemy.data()));
	}
	else if (type == "enter_vehicle") {
		auto obj = dynamic_cast<EnterVehicleChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to EnterVehicleChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetVehicle());
		string vehicle = ToString(condition.EvaluateValue(getValues));
		global->GetTrafficActor()->EnterVehicle(UTF8_TO_TCHAR(vehicle.data()));
	}
	else if (type == "leave_vehicle") {
		auto obj = dynamic_cast<LeaveVehicleChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to LeaveVehicleChange.\n");
		}
		Condition condition;
		condition.ParseCondition(obj->GetVehicle());
		string vehicle = ToString(condition.EvaluateValue(getValues));
		global->GetTrafficActor()->LeaveVehicle(UTF8_TO_TCHAR(vehicle.data()));
	}
	else if (type == "launch_elevator") {
		auto obj = dynamic_cast<LaunchElevatorChange*>(change);
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
	else if (type == "for_range") {
		auto obj = dynamic_cast<ForRangeChange*>(change);
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

			for (auto* innerChange : obj->GetChanges()) {
				if (innerChange->GetCondition().EvaluateBool(innerGetValues)) {
					global->GetMap()->ApplyChange(innerChange, innerGetValues);
					global->GetPopulace()->ApplyChange(global->GetMap(), innerChange, innerGetValues);
					global->GetSociety()->ApplyChange(innerChange, innerGetValues);
					global->GetStory()->ApplyChange(innerChange, innerGetValues, ownerScript);
					global->GetIndustry()->ApplyChange(innerChange, innerGetValues);
					global->GetTraffic()->ApplyChange(innerChange, innerGetValues);
					global->GetPlayer()->ApplyChange(innerChange, innerGetValues);
					ApplyChange(innerChange, innerGetValues, ownerScript);
				}
			}
		}
	}
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
			for (auto change : changes) {
				if (!change->GetCondition().EvaluateBool(getValues))continue;
				global->GetMap()->ApplyChange(change, getValues);
				global->GetPopulace()->ApplyChange(global->GetMap(), change, getValues);
				global->GetSociety()->ApplyChange(change, getValues);
				global->GetStory()->ApplyChange(change, getValues, ownerScript);
				global->GetIndustry()->ApplyChange(change, getValues);
				global->GetTraffic()->ApplyChange(change, getValues);
				global->GetPlayer()->ApplyChange(change, getValues);

				ApplyChange(change, getValues, ownerScript);
			}
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

	Script* local = new Script(Story::scriptFactory, "empty");
	local->SetValue("local.name", TCHAR_TO_UTF8(*name));
	local->SetValue("local.option", TCHAR_TO_UTF8(*option));
	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		},
		[&](const string& name) -> pair<bool, ValueType> {
			return local->GetValue(name);
		}
	};
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

	Script* local = new Script(Story::scriptFactory, "empty");
	local->SetValue("local.result", result);
	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		},
		[&](const string& name) -> pair<bool, ValueType> {
			return local->GetValue(name);
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

	delete local;
	delete event;
}

