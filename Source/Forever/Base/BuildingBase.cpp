#include "BuildingBase.h"

#include "GlobalBase.h"
#include "RoomBase.h"
#include "StoryBase.h"

#include "map/map.h"
#include "map/block.h"
#include "map/roadnet.h"
#include "map/zone.h"
#include "map/building.h"
#include "map/room.h"
#include "story/story.h"
#include "story/script.h"
#include "story/event.h"


using namespace std;

ABuildingBase::ABuildingBase() {
	PrimaryActorTick.bCanEverTick = true;
}

ABuildingBase::~ABuildingBase() {

}

void ABuildingBase::BeginPlay() {
	Super::BeginPlay();
}

void ABuildingBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (!global) return;

	FVector location = FVector(0.f, 0.f, 0.f);
	global->GetLocation(location);
	location /= 1000.f;

	TArray<FBuilding> buildings;
	auto map = global->GetMap();
	if (!map) return;
	auto roadnet = map->GetRoadnet();
	if (!roadnet) return;
	auto blocks = roadnet->GetBlocks();
	for (auto block : blocks) {
		FVector blockLocation = FVector(block->GetPosX(), block->GetPosY(), 0.f);
		if ((location - blockLocation).Size() > 64.f) {
			continue;
		}
		auto blockBuildings = block->GetBuildings();
		for (auto& [blockBuildingName, building] : blockBuildings) {
			// 跳过面积为零的建筑，此类建筑无有效占地，不需要生成实例
			if (building->GetAcreage() <= 0.f)continue;
			if (buildingInstances.find(blockBuildingName) != buildingInstances.end()) {
				continue;
			}
			FBuilding buildingInfo;
			buildingInfo.name = UTF8_TO_TCHAR(blockBuildingName.data());
			auto construction = building->GetConstruction();
			auto center = block->GetPosition(
				building->GetPosX() - building->GetSizeX() / 2.f + construction.GetPosX(),
				building->GetPosY() - building->GetSizeY() / 2.f + construction.GetPosY());
			buildingInfo.center = FVector(center.first, center.second,
				building->GetHeight() * (building->GetLayers() - building->GetBasements()) / 2.f);
			if ((location - buildingInfo.center).Size() > 32.f) {
				continue;
			}
			buildingInfo.size = FVector(construction.GetSizeX(), construction.GetSizeY(),
				building->GetHeight() * (building->GetLayers() + building->GetBasements()));
			buildingInfo.rotation = block->GetRotation();
			ConstructBuilding(building, buildingInfo);
			buildings.Add(buildingInfo);
		}
		auto blockZones = block->GetZones();
		for (auto& [_, zone] : blockZones) {
			for (auto& [zoneBuildingName, building] : zone->GetBuildings()) {
				// 跳过面积为零的建筑，此类建筑无有效占地，不需要生成实例
				if (building->GetAcreage() <= 0.f)continue;
				if (buildingInstances.find(zoneBuildingName) != buildingInstances.end()) {
					continue;
				}
				FBuilding buildingInfo;
				buildingInfo.name = UTF8_TO_TCHAR(zoneBuildingName.data());
				auto construction = building->GetConstruction();
				auto center = block->GetPosition(
					zone->GetPosX() - zone->GetSizeX() / 2 + building->GetPosX() - building->GetSizeX() / 2.f + construction.GetPosX(),
					zone->GetPosY() - zone->GetSizeY() / 2 + building->GetPosY() - building->GetSizeY() / 2.f + construction.GetPosY());
				buildingInfo.center = FVector(center.first, center.second,
					building->GetHeight() * (building->GetLayers() - building->GetBasements()) / 2.f);
				if ((location - buildingInfo.center).Size() > 32.f) {
					continue;
				}
				buildingInfo.size = FVector(construction.GetSizeX(), construction.GetSizeY(),
					building->GetHeight() * (building->GetLayers() + building->GetBasements()));
				buildingInfo.rotation = block->GetRotation();
				ConstructBuilding(building, buildingInfo);
				buildings.Add(buildingInfo);
			}
		}
	}
	UpdateBuilding(buildings);
}

void ABuildingBase::SetGlobal(AGlobalBase* g) {
	this->global = g;
}

void ABuildingBase::SetInstance(FString name, AActor* actor) {
	if (buildingInstances.find(TCHAR_TO_UTF8(*name)) == buildingInstances.end()) {
		buildingInstances[TCHAR_TO_UTF8(*name)] = actor;
	}
	else {
		THROW_EXCEPTION(RuntimeException, string("Duplicate building name: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
}

AActor* ABuildingBase::GetInstance(FString name) {
	auto it = buildingInstances.find(TCHAR_TO_UTF8(*name));
	if (it == buildingInstances.end()) return nullptr;
	return it->second;
}

void ABuildingBase::EnterBuilding(FString building) {
	if (!global) return;
	auto storyBase = global->GetStoryActor();
	auto story = global->GetStory();
	if (!story) return;
	auto storyScript = story->GetScript();
	if (!storyScript) return;
	storyScript->SetValue("system.player.building", TCHAR_TO_UTF8(*building));
	auto zone = storyScript->GetValue("player.zone").second;
	Event* event;
	if (holds_alternative<string>(zone)) {
		event = new EnterBuildingEvent(TCHAR_TO_UTF8(get<string>(zone).data()), TCHAR_TO_UTF8(*building));
	}
	else {
		event = new EnterBuildingEvent("", TCHAR_TO_UTF8(*building));
	}

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& name) -> pair<bool, ValueType> {
			return storyScript->GetValue(name);
		}
	};
	storyBase->MatchEvent(event, storyScript, getValues);

	auto b = global->GetMap()->GetBuilding(TCHAR_TO_UTF8(*building));
	if (b) {
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return b->GetScript()->GetValue(name);
			});
		storyBase->MatchEvent(event, b->GetScript(), getValues);
		getValues.pop_back();
	}
	else {
		for (auto [_, z] : global->GetMap()->GetZones()) {
			b = z->GetBuilding(TCHAR_TO_UTF8(*building));
			if (!b)continue;
			getValues.push_back(
				[&](const string& name) -> pair<bool, ValueType> {
					return b->GetScript()->GetValue(name);
				});
			storyBase->MatchEvent(event, b->GetScript(), getValues);
			getValues.pop_back();
			break;
		}
	}

	delete event;
}

void ABuildingBase::LeaveBuilding(FString building) {
	if (!global) return;
	auto storyBase = global->GetStoryActor();
	auto story = global->GetStory();
	if (!story) return;
	auto storyScript = story->GetScript();
	if (!storyScript) return;
	storyScript->SetValue("system.player.building", "");
	auto zone = storyScript->GetValue("player.zone").second;
	Event* event;
	if (holds_alternative<string>(zone)) {
		event = new LeaveBuildingEvent(TCHAR_TO_UTF8(get<string>(zone).data()), TCHAR_TO_UTF8(*building));
	}
	else {
		event = new LeaveBuildingEvent("", TCHAR_TO_UTF8(*building));
	}

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& name) -> pair<bool, ValueType> {
			return storyScript->GetValue(name);
		}
	};
	storyBase->MatchEvent(event, storyScript, getValues);

	auto b = global->GetMap()->GetBuilding(TCHAR_TO_UTF8(*building));
	if (b) {
		getValues.push_back(
			[&](const string& name) -> pair<bool, ValueType> {
				return b->GetScript()->GetValue(name);
			});
		storyBase->MatchEvent(event, b->GetScript(), getValues);
		getValues.pop_back();
	}
	else {
		for (auto [_, z] : global->GetMap()->GetZones()) {
			b = z->GetBuilding(TCHAR_TO_UTF8(*building));
			if (!b)continue;
			getValues.push_back(
				[&](const string& name) -> pair<bool, ValueType> {
					return b->GetScript()->GetValue(name);
				});
			storyBase->MatchEvent(event, b->GetScript(), getValues);
			getValues.pop_back();
			break;
		}
	}

	delete event;
}

float ABuildingBase::GetRotation(FACE_DIRECTION direction) {
	switch (direction) {
	case FACE_EAST:
		return 270.f;
	case FACE_WEST:
		return 90.f;
	case FACE_NORTH:
		return 180.f;
	case FACE_SOUTH:
		return 0.f;
	}
	return 0.f;
}

void ABuildingBase::ConstructBuilding(Building* building, FBuilding& info) {
	auto roomBase = global->GetRoomActor();
	roomBase->AddBuilding(building->GetName(), building);

	auto ResolveTexture = [&](string raw, const char* fallback) -> FString {
		if (raw.empty()) return FString(fallback);
		return FString(raw.data());
	};
	FString wallMaterial = ResolveTexture(building->GetWallTexture(),
		"/Game/Asset/Materials/White.White");
	FString ceilingMaterial = ResolveTexture(building->GetCeilingTexture(),
		"/Game/Asset/Materials/White.White");
	FString groundMaterial = ResolveTexture(building->GetGroundTexture(),
		"/Game/Asset/Materials/White.White");

	auto construction = building->GetConstruction();
	FVector bias = -FVector(construction.GetSizeX() / 2.f, construction.GetSizeY() / 2.f, 0.f);
	for (int i = -building->GetBasements(); i < building->GetLayers(); i++) {
		auto stairs = building->GetFloor(i)->GetStairs();
		for (auto stair : stairs) {
			if (stair.GetDirection() == FACE_WEST || stair.GetDirection() == FACE_EAST) {
				info.meshes.Add(
					FMesh(FVector(stair.GetPosX(), stair.GetPosY(), building->GetHeight() * i) + bias,
						FVector(stair.GetSizeY(), stair.GetSizeX(), building->GetHeight()), GetRotation(stair.GetDirection()),
						FString("/Game/Asset/Meshes/Stair.Stair")));
			}
			else {
				info.meshes.Add(
					FMesh(FVector(stair.GetPosX(), stair.GetPosY(), building->GetHeight() * i) + bias,
						FVector(stair.GetSizeX(), stair.GetSizeY(), building->GetHeight()), GetRotation(stair.GetDirection()),
						FString("/Game/Asset/Meshes/Stair.Stair")));
			}
			ConstructQuad(
				FVector(stair.GetPosX(), stair.GetPosY(), (i + 0.5f) * building->GetHeight()) + bias,
				FVector(stair.GetSizeX(), stair.GetSizeY(), building->GetHeight()),
				{ stair.GetWall(0), stair.GetWall(1), stair.GetWall(2), stair.GetWall(3) },
				{}, {},
				wallMaterial, info.walls, info.meshes);
		}
		auto elevators = building->GetFloor(i)->GetElevators();
		for (auto elevator : elevators) {
			ConstructQuad(
				FVector(elevator.GetPosX(), elevator.GetPosY(), (i + 0.5f) * building->GetHeight()) + bias,
				FVector(elevator.GetSizeX(), elevator.GetSizeY(), building->GetHeight()),
				{ elevator.GetWall(0), elevator.GetWall(1), elevator.GetWall(2), elevator.GetWall(3) },
				{}, {},
				wallMaterial, info.walls, info.meshes);
		}
		auto ramps = building->GetFloor(i)->GetRamps();
		for (auto ramp : ramps) {
			if (ramp.GetDirection() == FACE_WEST || ramp.GetDirection() == FACE_EAST) {
				info.meshes.Add(
					FMesh(FVector(ramp.GetPosX(), ramp.GetPosY(), building->GetHeight() * i) + bias,
						FVector(ramp.GetSizeY(), ramp.GetSizeX(), building->GetHeight()), GetRotation(ramp.GetDirection()),
						FString("/Game/Asset/Meshes/Ramp.Ramp")));
			}
			else {
				info.meshes.Add(
					FMesh(FVector(ramp.GetPosX(), ramp.GetPosY(), building->GetHeight() * i) + bias,
						FVector(ramp.GetSizeX(), ramp.GetSizeY(), building->GetHeight()), GetRotation(ramp.GetDirection()),
						FString("/Game/Asset/Meshes/Ramp.Ramp")));
			}
			ConstructQuad(
				FVector(ramp.GetPosX(), ramp.GetPosY(), (i + 0.5f) * building->GetHeight()) + bias,
				FVector(ramp.GetSizeX(), ramp.GetSizeY(), building->GetHeight()),
				{ ramp.GetWall(0), ramp.GetWall(1), ramp.GetWall(2), ramp.GetWall(3) },
				{}, {},
				wallMaterial, info.walls, info.meshes);
		}
		auto ceilings = building->GetFloor(i)->GetCeilings();
		for (auto ceiling : ceilings) {
			info.walls.Add(
				FWall(FVector(ceiling.GetPosX(), ceiling.GetPosY(), building->GetHeight() * (i + 1) - 0.01f) + bias,
					FVector(ceiling.GetSizeX(), ceiling.GetSizeY(), 0.02f),
					ceilingMaterial));
		}
		auto grounds = building->GetFloor(i)->GetGrounds();
		for (auto ground : grounds) {
			info.walls.Add(
				FWall(FVector(ground.GetPosX(), ground.GetPosY(), building->GetHeight() * i + 0.01f) + bias,
					FVector(ground.GetSizeX(), ground.GetSizeY(), 0.02f),
					groundMaterial));
		}
		auto corridors = building->GetFloor(i)->GetCorridors();
		for (auto corridor : corridors) {
			ConstructQuad(
				FVector(corridor.GetPosX(), corridor.GetPosY(), (i + 0.5f) * building->GetHeight()) + bias,
				FVector(corridor.GetSizeX(), corridor.GetSizeY(), building->GetHeight()),
				{ corridor.GetWall(0), corridor.GetWall(1), corridor.GetWall(2), corridor.GetWall(3) },
				corridor.GetDoors(), corridor.GetWindows(),
				wallMaterial, info.walls, info.meshes);
		}
	}

	for (auto cabin : building->GetCabins()) {
		int temp = cabin->GetTemp();
		auto elevator = building->GetFloor(temp)->GetElevators()[cabin->GetIdx()];

		FElevator elevatorInfo;
		elevatorInfo.name = UTF8_TO_TCHAR(cabin->GetName().data());
		elevatorInfo.center = FVector(elevator.GetPosX(), elevator.GetPosY(), 0.f) + bias;
		if (elevator.GetDirection() == FACE_WEST || elevator.GetDirection() == FACE_EAST) {
			elevatorInfo.size = FVector(elevator.GetSizeY(), elevator.GetSizeX(), building->GetHeight());
		}
		else {
			elevatorInfo.size = FVector(elevator.GetSizeX(), elevator.GetSizeY(), building->GetHeight());
		}
		elevatorInfo.rot = GetRotation(elevator.GetDirection());
		elevatorInfo.minFloor = cabin->GetMinFloor();
		elevatorInfo.maxFloor = cabin->GetMaxFloor();
		elevatorInfo.currentFloor = cabin->GetCurrentFloor();
		info.elevators.Add(elevatorInfo);
	}

	for (auto room : building->GetRooms()) {
		ConstructQuad(
			FVector(room->GetPosX(), room->GetPosY(), (room->GetLayer() + 0.5f) * building->GetHeight()) + bias,
			FVector(room->GetSizeX(), room->GetSizeY(), building->GetHeight()), { true, true, true, true },
			room->GetDoors(), room->GetWindows(),
			wallMaterial, info.walls, info.meshes);
	}
	for(auto &[decoration, transform] : building->GetDecorations()) {
		info.meshes.Add(
			FMesh(FVector(transform[0], transform[1], transform[2]) + bias,
				FVector(transform[3], transform[4], transform[5]),
				transform[8], FString(decoration.data())));
	}
}

void ABuildingBase::ConstructQuad(FVector center, FVector size, const vector<bool>& directions,
	const unordered_map<FACE_DIRECTION, vector<pair<vector<float>, Quad>>>& doors,
	const unordered_map<FACE_DIRECTION, vector<pair<vector<float>, Quad>>>& windows,
	const FString& wallMaterial, TArray<FWall>& walls, TArray<FMesh>& meshes) {

	using MakeFn = function<FVector(float, float)>;

	auto processFace = [&](bool enabled, FACE_DIRECTION dir,
		float horizBase, float horizSpan, float vertSpan,
		MakeFn makePos, MakeFn makeSize) {
		if (!enabled) return;

		struct Opening { float x1, x2, y1, y2; };
		vector<Opening> openings;
		if (auto it = doors.find(dir); it != doors.end())
			for (auto& [p, _] : it->second)
				openings.push_back({ p[0]*horizSpan+p[1], p[4]*horizSpan+p[5], p[2]*vertSpan+p[3], p[6]*vertSpan+p[7] });
		if (auto it = windows.find(dir); it != windows.end())
			for (auto& [p, _] : it->second)
				openings.push_back({ p[0]*horizSpan+p[1], p[4]*horizSpan+p[5], p[2]*vertSpan+p[3], p[6]*vertSpan+p[7] });

		if (openings.empty()) {
			walls.Add(FWall(makePos(horizBase + horizSpan/2.f, center.Z), makeSize(horizSpan, vertSpan), wallMaterial));
			return;
		}

		sort(openings.begin(), openings.end(), [](const Opening& a, const Opening& b) { return a.x1 < b.x1; });

		float prevX = 0.f;
		for (auto& op : openings) {
			float segW = op.x1 - prevX;
			if (segW > 0.f)
				walls.Add(FWall(makePos(horizBase + prevX + segW/2.f, center.Z), makeSize(segW, vertSpan), wallMaterial));
			if (op.y1 > 0.f)
				walls.Add(FWall(makePos(horizBase + (op.x1+op.x2)/2.f, center.Z + vertSpan/2.f - op.y1/2.f), makeSize(op.x2-op.x1, op.y1), wallMaterial));
			float bH = vertSpan - op.y2;
			if (bH > 0.f)
				walls.Add(FWall(makePos(horizBase + (op.x1+op.x2)/2.f, center.Z - vertSpan/2.f + bH/2.f), makeSize(op.x2-op.x1, bH), wallMaterial));
			prevX = op.x2;
		}
		float segW = horizSpan - prevX;
		if (segW > 0.f)
			walls.Add(FWall(makePos(horizBase + prevX + segW/2.f, center.Z), makeSize(segW, vertSpan), wallMaterial));

		if (auto it = windows.find(dir); it != windows.end())
			for (auto& [p, _] : it->second) {
				float wx1 = p[0]*horizSpan+p[1], wx2 = p[4]*horizSpan+p[5];
				float wy1 = p[2]*vertSpan+p[3], wy2 = p[6]*vertSpan+p[7];
				meshes.Add(FMesh(
					makePos(horizBase + (wx1+wx2)/2.f, center.Z + vertSpan/2.f - (wy1+wy2)/2.f),
					makeSize(wx2-wx1, wy2-wy1),
					0.f,
					FString(TEXT("/Game/Asset/Meshes/Window.Window"))));
			}
	};

	processFace(directions[0], FACE_WEST,
		center.Y - size.Y/2.f, size.Y, size.Z,
		[&](float hc, float vc) { return FVector(center.X - size.X/2.f, hc, vc); },
		[](float hw, float vh) { return FVector(0.01f, hw, vh); });

	processFace(directions[1], FACE_EAST,
		center.Y - size.Y/2.f, size.Y, size.Z,
		[&](float hc, float vc) { return FVector(center.X + size.X/2.f, hc, vc); },
		[](float hw, float vh) { return FVector(0.01f, hw, vh); });

	processFace(directions[2], FACE_NORTH,
		center.X - size.X/2.f, size.X, size.Z,
		[&](float hc, float vc) { return FVector(hc, center.Y - size.Y/2.f, vc); },
		[](float hw, float vh) { return FVector(hw, 0.01f, vh); });

	processFace(directions[3], FACE_SOUTH,
		center.X - size.X/2.f, size.X, size.Z,
		[&](float hc, float vc) { return FVector(hc, center.Y + size.Y/2.f, vc); },
		[](float hw, float vh) { return FVector(hw, 0.01f, vh); });

}
