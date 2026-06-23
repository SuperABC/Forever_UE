#include "config.h"

#include "utility.h"
#include "error.h"
#include "json.h"

#include <fstream>
#include <filesystem>
#include <cctype>


using namespace std;

typedef void* (*GetModFunc)();

unordered_map<string, vector<string>> Config::dllPaths = {};
unordered_map<string, unordered_map<string, bool>> Config::modEnables = {};
vector<string> Config::resourcePaths = {};
unordered_map<string, vector<string>> Config::layoutPaths = {};
unordered_map<string, unordered_map<string, string>> Config::scriptPaths = {};
unordered_map<string, vector<string>> Config::actionPaths = {};
unordered_map<string, vector<string>> Config::pluginPaths = {};
unordered_set<string> Config::storyPaths = {};

bool CheckFileFormat(const filesystem::path& filePath, const string& format) {
	if (!filesystem::is_regular_file(filePath))
		return false;
	string ext = filePath.extension().string();
	for (char& c : ext) {
		c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
	}
	return ext == format;
}

void Config::ReadConfig(const string& path) {
	dllPaths.clear();
	modEnables.clear();
	resourcePaths.clear();
	layoutPaths.clear();
	scriptPaths.clear();
	actionPaths.clear();
	pluginPaths.clear();
	storyPaths.clear();

	JsonReader reader;
	JsonValue root;

	ifstream fin(path);
	if (!fin.is_open()) {
		THROW_EXCEPTION(IOException, "Failed to open file: " + path + ".\n");
	}
	if (reader.Parse(fin, root)) {
		for (auto dllPath : root["dll_paths"]) {
			AddDllPath(dllPath.AsString());
		}

		CheckMod("terrain", "empty", false);
		for (auto terrainMod : root["terrain_mods"]) {
			CheckMod("terrain", terrainMod.AsString(), true);
		}
		CheckMod("roadnet", "empty", false);
		CheckMod("roadnet", root["roadnet_mod"].AsString(), true);
		CheckMod("zone", "empty", false);
		for (auto zoneMod : root["zone_mods"]) {
			CheckMod("zone", zoneMod.AsString(), true);
		}
		CheckMod("building", "empty", false);
		for (auto buildingMod : root["building_mods"]) {
			CheckMod("building", buildingMod.AsString(), true);
		}
		CheckMod("component", "empty", false);
		for (auto componentMod : root["component_mods"]) {
			CheckMod("component", componentMod.AsString(), true);
		}
		CheckMod("room", "empty", false);
		for (auto roomMod : root["room_mods"]) {
			CheckMod("room", roomMod.AsString(), true);
		}
		CheckMod("asset", "empty", false);
		for (auto assetMod : root["asset_mods"]) {
			CheckMod("asset", assetMod.AsString(), true);
		}
		CheckMod("name", "empty", false);
		CheckMod("name", root["name_mod"].AsString(), true);
		CheckMod("scheduler", "empty", false);
		for (auto schedulerMod : root["scheduler_mods"]) {
			CheckMod("scheduler", schedulerMod.AsString(), true);
		}
		CheckMod("job", "empty", false);
		for (auto jobMod : root["job_mods"]) {
			CheckMod("job", jobMod.AsString(), true);
		}
		CheckMod("calendar", "empty", false);
		for (auto calendarMod : root["calendar_mods"]) {
			CheckMod("calendar", calendarMod.AsString(), true);
		}
		CheckMod("organization", "empty", false);
		for (auto organizationMod : root["organization_mods"]) {
			CheckMod("organization", organizationMod.AsString(), true);
		}
		CheckMod("script", "empty", false);
		for (auto scriptMod : root["script_mods"]) {
			CheckMod("script", scriptMod.AsString(), true);
		}
		CheckMod("product", "empty", false);
		for (auto productMod : root["product_mods"]) {
			CheckMod("product", productMod.AsString(), true);
		}
		CheckMod("storage", "empty", false);
		for (auto storageMod : root["storage_mods"]) {
			CheckMod("storage", storageMod.AsString(), true);
		}
		CheckMod("manufacture", "empty", false);
		for (auto manufactureMod : root["manufacture_mods"]) {
			CheckMod("manufacture", manufactureMod.AsString(), true);
		}
		CheckMod("route", "empty", false);
		for (auto routeMod : root["route_mods"]) {
			CheckMod("route", routeMod.AsString(), true);
		}
		CheckMod("station", "empty", false);
		for (auto stationMod : root["station_mods"]) {
			CheckMod("station", stationMod.AsString(), true);
		}
		CheckMod("vehicle", "empty", false);
		for (auto vehicleMod : root["vehicle_mods"]) {
			CheckMod("vehicle", vehicleMod.AsString(), true);
		}
		CheckMod("puzzle", "empty", false);
		for (auto puzzleMod : root["puzzle_mods"]) {
			CheckMod("puzzle", puzzleMod.AsString(), true);
		}
		CheckMod("app", "empty", false);
		for (auto appMod : root["app_mods"]) {
			CheckMod("app", appMod.AsString(), true);
		}

		for (auto resourcePath : root["resource_paths"]) {
			AddResourcePath(resourcePath.AsString());
		}

		for (auto storyPath : root["story_paths"]) {
			AddScript(storyPath.AsString());
		}
	}
	else {
		fin.close();
		THROW_EXCEPTION(JsonFormatException, "Json syntax error: " + reader.GetErrorMessages() + ".\n");
	}
	fin.close();
}

void Config::WriteConfig(const string& path) {
	StyledWriter writer;
	JsonValue root(DATA_OBJECT);

	JsonValue dlls = JsonValue(DATA_ARRAY);
	for (const auto& [dllDir, _] : dllPaths) {
		JsonValue dll = JsonValue(dllDir);
		dlls.append(dll);
	}
	root["dll_paths"] = dlls;

	JsonValue terrains = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["terrain"]) {
		if (modEnabled) {
			JsonValue terrain = JsonValue(modName);
			terrains.append(terrain);
		}
	}
	root["terrain_mods"] = terrains;

	JsonValue roadnets = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["roadnet"]) {
		if (modEnabled) {
			JsonValue roadnet = JsonValue(modName);
			roadnets.append(roadnet);
		}
	}
	if (roadnets.size() > 1) {
		debugf("Warning: Selected more than one roadnet mods.\n");
	}
	root["roadnet_mod"] = roadnets[0];

	JsonValue zones = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["zone"]) {
		if (modEnabled) {
			JsonValue zone = JsonValue(modName);
			zones.append(zone);
		}
	}
	root["zone_mods"] = zones;

	JsonValue buildings = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["building"]) {
		if (modEnabled) {
			JsonValue building = JsonValue(modName);
			buildings.append(building);
		}
	}
	root["building_mods"] = buildings;

	JsonValue components = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["component"]) {
		if (modEnabled) {
			JsonValue component = JsonValue(modName);
			components.append(component);
		}
	}
	root["component_mods"] = components;

	JsonValue rooms = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["room"]) {
		if (modEnabled) {
			JsonValue room = JsonValue(modName);
			rooms.append(room);
		}
	}
	root["room_mods"] = rooms;

	JsonValue assets = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["asset"]) {
		if (modEnabled) {
			JsonValue asset = JsonValue(modName);
			assets.append(asset);
		}
	}
	root["asset_mods"] = assets;

	JsonValue names = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["name"]) {
		if (modEnabled) {
			JsonValue name = JsonValue(modName);
			names.append(name);
		}
	}
	if (names.size() > 1) {
		debugf("Warning: Selected more than one name mods.\n");
	}
	root["name_mod"] = names[0];

	JsonValue schedulers = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["scheduler"]) {
		if (modEnabled) {
			JsonValue scheduler = JsonValue(modName);
			schedulers.append(scheduler);
		}
	}
	root["scheduler_mods"] = schedulers;

	JsonValue jobs = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["job"]) {
		if (modEnabled) {
			JsonValue job = JsonValue(modName);
			jobs.append(job);
		}
	}
	root["job_mods"] = jobs;

	JsonValue calendars = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["calendar"]) {
		if (modEnabled) {
			JsonValue calendar = JsonValue(modName);
			calendars.append(calendar);
		}
	}
	root["calendar_mods"] = calendars;

	JsonValue organizations = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["organization"]) {
		if (modEnabled) {
			JsonValue organization = JsonValue(modName);
			organizations.append(organization);
		}
	}
	root["organization_mods"] = organizations;

	JsonValue scripts = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["script"]) {
		if (modEnabled) {
			JsonValue script = JsonValue(modName);
			scripts.append(script);
		}
	}
	root["script_mods"] = scripts;

	JsonValue products = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["product"]) {
		if (modEnabled) {
			JsonValue product = JsonValue(modName);
			products.append(product);
		}
	}
	root["product_mods"] = products;

	JsonValue storages = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["storage"]) {
		if (modEnabled) {
			JsonValue storage = JsonValue(modName);
			storages.append(storage);
		}
	}
	root["storage_mods"] = storages;

	JsonValue manufactures = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["manufacture"]) {
		if (modEnabled) {
			JsonValue manufacture = JsonValue(modName);
			manufactures.append(manufacture);
		}
	}
	root["manufacture_mods"] = manufactures;

	JsonValue routes = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["route"]) {
		if (modEnabled) {
			JsonValue route = JsonValue(modName);
			routes.append(route);
		}
	}
	root["route_mods"] = routes;

	JsonValue stations = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["station"]) {
		if (modEnabled) {
			JsonValue station = JsonValue(modName);
			stations.append(station);
		}
	}
	root["station_mods"] = stations;

	JsonValue vehicles = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["vehicle"]) {
		if (modEnabled) {
			JsonValue vehicle = JsonValue(modName);
			vehicles.append(vehicle);
		}
	}
	root["vehicle_mods"] = vehicles;

	JsonValue puzzles = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["puzzle"]) {
		if (modEnabled) {
			JsonValue puzzle = JsonValue(modName);
			puzzles.append(puzzle);
		}
	}
	root["puzzle_mods"] = puzzles;

	JsonValue apps = JsonValue(DATA_ARRAY);
	for (const auto& [modName, modEnabled] : modEnables["app"]) {
		if (modEnabled) {
			JsonValue app = JsonValue(modName);
			apps.append(app);
		}
	}
	root["app_mods"] = apps;

	JsonValue resources = JsonValue(DATA_ARRAY);
	for (auto resourcePath : resourcePaths) {
		JsonValue resource = JsonValue(resourcePath);
		resources.append(resource);
	}
	root["resource_paths"] = resources;

	JsonValue stories = JsonValue(DATA_ARRAY);
	for (auto storyPath : storyPaths) {
		JsonValue story = JsonValue(storyPath);
		stories.append(story);
	}
	root["story_paths"] = stories;

	ofstream fout(path);
	if (!fout.is_open()) {
		THROW_EXCEPTION(IOException, "Failed to open file: " + path + ".\n");
	}
	auto json = writer.Write(root);
	fout << json;
	fout.close();
}

vector<string> Config::GetDllPaths() {
	vector<string> paths;
	for (const auto& [dllDir, _] : dllPaths) {
		paths.push_back(dllDir);
	}
	return paths;
}

vector<string> Config::GetMods() {
	vector<string> mods;
	unordered_set<string> ids;
	for (const auto& [_, dlls] : dllPaths) {
		for (const auto& mod : dlls) {
			if (ids.find(mod) == ids.end()) {
				mods.push_back(mod);
				ids.insert(mod);
			}
		}
	}
	return mods;
}

void Config::AddDllPath(const string& path) {
	filesystem::path dir(path);
	if (!filesystem::exists(dir) || !filesystem::is_directory(dir)) {
		debugf("Warning: Mod path does not exist.\n");
		return;
	}

	RemoveDllPath(path);

	for (const auto& entry : filesystem::recursive_directory_iterator(dir)) {
		if (CheckFileFormat(entry.path(), ".dll")) {
			string full = filesystem::absolute(entry.path()).string();

			bool loaded = false;
			auto modHandle = LoadLibraryA(full.data());
			if (modHandle) {
				auto getTerrainFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModTerrains"));
				if (getTerrainFunc) {
					loaded = true;
					auto terrains = *(vector<string>*)getTerrainFunc();
					for (auto terrain : terrains) {
						modEnables["terrain"].insert({ terrain, false });
					}
				}

				auto getRoadnetFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModRoadnets"));
				if (getRoadnetFunc) {
					loaded = true;
					auto roadnets = *(vector<string>*)getRoadnetFunc();
					for (auto roadnet : roadnets) {
						modEnables["roadnet"].insert({ roadnet, false });
					}
				}

				auto getZoneFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModZones"));
				if (getZoneFunc) {
					loaded = true;
					auto zones = *(vector<string>*)getZoneFunc();
					for (auto zone : zones) {
						modEnables["zone"].insert({ zone, false });
					}
				}

				auto getBuildingFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModBuildings"));
				if (getBuildingFunc) {
					loaded = true;
					auto buildings = *(vector<string>*)getBuildingFunc();
					for (auto building : buildings) {
						modEnables["building"].insert({ building, false });
					}
				}

				auto getComponentFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModComponents"));
				if (getComponentFunc) {
					loaded = true;
					auto components = *(vector<string>*)getComponentFunc();
					for (auto component : components) {
						modEnables["component"].insert({ component, false });
					}
				}

				auto getRoomFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModRooms"));
				if (getRoomFunc) {
					loaded = true;
					auto rooms = *(vector<string>*)getRoomFunc();
					for (auto room : rooms) {
						modEnables["room"].insert({ room, false });
					}
				}

				auto getAssetFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModAssets"));
				if (getAssetFunc) {
					loaded = true;
					auto assets = *(vector<string>*)getAssetFunc();
					for (auto asset : assets) {
						modEnables["asset"].insert({ asset, false });
					}
				}

				auto getNameFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModNames"));
				if (getNameFunc) {
					loaded = true;
					auto names = *(vector<string>*)getNameFunc();
					for (auto name : names) {
						modEnables["name"].insert({ name, false });
					}
				}

				auto getSchedulerFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModSchedulers"));
				if (getSchedulerFunc) {
					loaded = true;
					auto schedulers = *(vector<string>*)getSchedulerFunc();
					for (auto scheduler : schedulers) {
						modEnables["scheduler"].insert({ scheduler, false });
					}
				}

				auto getJobFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModJobs"));
				if (getJobFunc) {
					loaded = true;
					auto jobs = *(vector<string>*)getJobFunc();
					for (auto job : jobs) {
						modEnables["job"].insert({ job, false });
					}
				}

				auto getCalendarFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModCalendars"));
				if (getCalendarFunc) {
					loaded = true;
					auto calendars = *(vector<string>*)getCalendarFunc();
					for (auto calendar : calendars) {
						modEnables["calendar"].insert({ calendar, false });
					}
				}

				auto getOrganizationFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModOrganizations"));
				if (getOrganizationFunc) {
					loaded = true;
					auto organizations = *(vector<string>*)getOrganizationFunc();
					for (auto organization : organizations) {
						modEnables["organization"].insert({ organization, false });
					}
				}

				auto getScriptFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModScripts"));
				if (getScriptFunc) {
					loaded = true;
					auto scripts = *(vector<string>*)getScriptFunc();
					for (auto script : scripts) {
						modEnables["script"].insert({ script, false });
					}
				}

				auto getProductFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModProducts"));
				if (getProductFunc) {
					loaded = true;
					auto products = *(vector<string>*)getProductFunc();
					for (auto product : products) {
						modEnables["product"].insert({ product, false });
					}
				}

				auto getStorageFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModStorages"));
				if (getStorageFunc) {
					loaded = true;
					auto storages = *(vector<string>*)getStorageFunc();
					for (auto storage : storages) {
						modEnables["storage"].insert({ storage, false });
					}
				}

				auto getManufactureFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModManufactures"));
				if (getManufactureFunc) {
					loaded = true;
					auto manufactures = *(vector<string>*)getManufactureFunc();
					for (auto manufacture : manufactures) {
						modEnables["manufacture"].insert({ manufacture, false });
					}
				}

				auto getRouteFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModRoutes"));
				if (getRouteFunc) {
					loaded = true;
					auto routes = *(vector<string>*)getRouteFunc();
					for (auto route : routes) {
						modEnables["route"].insert({ route, false });
					}
				}

				auto getStationFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModStations"));
				if (getStationFunc) {
					loaded = true;
					auto stations = *(vector<string>*)getStationFunc();
					for (auto station : stations) {
						modEnables["station"].insert({ station, false });
					}
				}

				auto getVehicleFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModVehicles"));
				if (getVehicleFunc) {
					loaded = true;
					auto vehicles = *(vector<string>*)getVehicleFunc();
					for (auto vehicle : vehicles) {
						modEnables["vehicle"].insert({ vehicle, false });
					}
				}

				auto getPuzzleFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModPuzzles"));
				if (getPuzzleFunc) {
					loaded = true;
					auto puzzles = *(vector<string>*)getPuzzleFunc();
					for (auto puzzle : puzzles) {
						modEnables["puzzle"].insert({ puzzle, false });
					}
				}

				auto getAppFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModApps"));
				if (getAppFunc) {
					loaded = true;
					auto apps = *(vector<string>*)getAppFunc();
					for (auto app : apps) {
						modEnables["app"].insert({ app, false });
					}
				}

				FreeLibrary(modHandle);
			}

			if (loaded) {
				dllPaths[path].push_back(full);
			}
		}
	}
}

void Config::RemoveDllPath(const string& path) {
	for (const auto& mod : dllPaths[path]) {
		bool duplicated = false;
		for (const auto& [dllDir, dlls] : dllPaths) {
			if (dllDir == path)continue;
			for (const auto& dll : dlls) {
				if (mod == dll) {
					duplicated = true;
					break;
				}
			}
			if (duplicated)break;
		}
		if (!duplicated) {
			filesystem::path dir(path);

			if (!filesystem::exists(dir) || !filesystem::is_directory(dir)) {
				debugf("Warning: Mod path does not exist when deleting.\n");
				continue;
			}

			for (const auto& entry : filesystem::recursive_directory_iterator(dir)) {
				if (CheckFileFormat(entry.path(), ".dll")) {
					string full = filesystem::absolute(entry.path()).string();

					auto modHandle = LoadLibraryA(full.data());
					if (modHandle) {
						auto getTerrainFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModTerrains"));
						if (getTerrainFunc) {
							auto terrains = *(vector<string>*)getTerrainFunc();
							for (auto terrain : terrains) {
								modEnables["terrain"].erase(terrain);
							}
						}

						auto getRoadnetFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModRoadnets"));
						if (getRoadnetFunc) {
							auto roadnets = *(vector<string>*)getRoadnetFunc();
							for (auto roadnet : roadnets) {
								modEnables["roadnet"].erase(roadnet);
							}
						}

						auto getZoneFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModZones"));
						if (getZoneFunc) {
							auto zones = *(vector<string>*)getZoneFunc();
							for (auto zone : zones) {
								modEnables["zone"].erase(zone);
							}
						}

						auto getBuildingFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModBuildings"));
						if (getBuildingFunc) {
							auto buildings = *(vector<string>*)getBuildingFunc();
							for (auto building : buildings) {
								modEnables["building"].erase(building);
							}
						}

						auto getComponentFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModComponents"));
						if (getComponentFunc) {
							auto components = *(vector<string>*)getComponentFunc();
							for (auto component : components) {
								modEnables["component"].erase(component);
							}
						}

						auto getRoomFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModRooms"));
						if (getRoomFunc) {
							auto rooms = *(vector<string>*)getRoomFunc();
							for (auto room : rooms) {
								modEnables["room"].erase(room);
							}
						}

						auto getAssetFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModAssets"));
						if (getAssetFunc) {
							auto assets = *(vector<string>*)getAssetFunc();
							for (auto asset : assets) {
								modEnables["asset"].erase(asset);
							}
						}

						auto getNameFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModNames"));
						if (getNameFunc) {
							auto names = *(vector<string>*)getNameFunc();
							for (auto name : names) {
								modEnables["name"].erase(name);
							}
						}

						auto getSchedulerFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModSchedulers"));
						if (getSchedulerFunc) {
							auto schedulers = *(vector<string>*)getSchedulerFunc();
							for (auto scheduler : schedulers) {
								modEnables["scheduler"].erase(scheduler);
							}
						}

						auto getJobFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModJobs"));
						if (getJobFunc) {
							auto jobs = *(vector<string>*)getJobFunc();
							for (auto job : jobs) {
								modEnables["job"].erase(job);
							}
						}

						auto getCalendarFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModCalendars"));
						if (getCalendarFunc) {
							auto calendars = *(vector<string>*)getCalendarFunc();
							for (auto calendar : calendars) {
								modEnables["calendar"].erase(calendar);
							}
						}

						auto getOrganizationFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModOrganizations"));
						if (getOrganizationFunc) {
							auto organizations = *(vector<string>*)getOrganizationFunc();
							for (auto organization : organizations) {
								modEnables["organization"].erase(organization);
							}
						}

						auto getScriptFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModScripts"));
						if (getScriptFunc) {
							auto scripts = *(vector<string>*)getScriptFunc();
							for (auto script : scripts) {
								modEnables["script"].erase(script);
							}
						}

						auto getChangeFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModChanges"));
						if (getChangeFunc) {
							auto changes = *(vector<string>*)getChangeFunc();
							for (auto change : changes) {
								modEnables["change"].erase(change);
							}
						}

						auto getProductFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModProducts"));
						if (getProductFunc) {
							auto products = *(vector<string>*)getProductFunc();
							for (auto product : products) {
								modEnables["product"].erase(product);
							}
						}

						auto getStorageFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModStorages"));
						if (getStorageFunc) {
							auto storages = *(vector<string>*)getStorageFunc();
							for (auto storage : storages) {
								modEnables["storage"].erase(storage);
							}
						}

						auto getManufactureFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModManufactures"));
						if (getManufactureFunc) {
							auto manufactures = *(vector<string>*)getManufactureFunc();
							for (auto manufacture : manufactures) {
								modEnables["manufacture"].erase(manufacture);
							}
						}

						auto getRouteFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModRoutes"));
						if (getRouteFunc) {
							auto routes = *(vector<string>*)getRouteFunc();
							for (auto route : routes) {
								modEnables["route"].erase(route);
							}
						}

						auto getStationFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModStations"));
						if (getStationFunc) {
							auto stations = *(vector<string>*)getStationFunc();
							for (auto station : stations) {
								modEnables["station"].erase(station);
							}
						}

						auto getVehicleFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModVehicles"));
						if (getVehicleFunc) {
							auto vehicles = *(vector<string>*)getVehicleFunc();
							for (auto vehicle : vehicles) {
								modEnables["vehicle"].erase(vehicle);
							}
						}

						auto getPuzzleFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModPuzzles"));
						if (getPuzzleFunc) {
							auto puzzles = *(vector<string>*)getPuzzleFunc();
							for (auto puzzle : puzzles) {
								modEnables["puzzle"].erase(puzzle);
							}
						}

						auto getAppFunc = reinterpret_cast<GetModFunc>(GetProcAddress(modHandle, "GetModApps"));
						if (getAppFunc) {
							auto apps = *(vector<string>*)getAppFunc();
							for (auto app : apps) {
								modEnables["app"].erase(app);
							}
						}

						FreeLibrary(modHandle);
					}
				}
			}
		}
	}


	dllPaths.erase(path);
}

unordered_map<string, bool> Config::GetChecks(const string& type) {
	return modEnables[type];
}

void Config::CheckMod(const string& type, const string& mod, bool enabled) {
	modEnables[type][mod] = enabled;
}

vector<string> Config::GetEnables(const string& type) {
	vector<string> enables;
	for (const auto& [modName, modEnabled] : modEnables[type]) {
		if (modEnabled) {
			enables.push_back(modName);
		}
	}
	return enables;
}

vector<string> Config::GetResourcePaths() {
	return resourcePaths;
}

vector<string> Config::GetLayouts() {
	vector<string> layouts;

	for (auto &layoutPath : layoutPaths) {
		layouts.insert(layouts.end(), layoutPath.second.begin(), layoutPath.second.end());
	}

	return layouts;
}

unordered_map<string, string> Config::GetScripts() {
	unordered_map<string, string> scripts;

	for (auto& scriptPath : scriptPaths) {
		scripts.insert(scriptPath.second.begin(), scriptPath.second.end());
	}

	return scripts;
}

string Config::GetScript(const string& name) {
	for (auto& scriptPath : scriptPaths) {
		if (scriptPath.second.find(name) != scriptPath.second.end()) {
			return scriptPath.second[name];
		}
	}

	return "";
}

vector<string> Config::GetActions() {
	vector<string> actions;

	for (auto& actionPath : actionPaths) {
		actions.insert(actions.end(), actionPath.second.begin(), actionPath.second.end());
	}

	return actions;
}

vector<string> Config::GetPlugins() {
	vector<string> plugins;

	for (auto& pluginPath : pluginPaths) {
		plugins.insert(plugins.end(), pluginPath.second.begin(), pluginPath.second.end());
	}

	return plugins;
}

void Config::AddResourcePath(const string& path) {
	filesystem::path dir(path);
	if (!filesystem::exists(dir) || !filesystem::is_directory(dir)) {
		debugf("Warning: Resource path does not exist.\n");
		return;
	}

	bool duplicated = false;
	for (auto resourcePath : resourcePaths) {
		if (resourcePath == path) {
			duplicated = true;
			break;
		}
	}
	if(!duplicated)resourcePaths.push_back(path);
	layoutPaths[path].clear();
	scriptPaths[path].clear();
	actionPaths[path].clear();
	pluginPaths[path].clear();

	for (const auto& entry : filesystem::recursive_directory_iterator(dir)) {
		if (filesystem::is_regular_file(entry.path())) {
			string ext = entry.path().extension().string();
			for (char& c : ext) {
				c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
			}

			string fullPath = filesystem::absolute(entry.path()).string();

			if (ext == ".layout") {
				layoutPaths[path].push_back(fullPath);
			}
			else if (ext == ".script") {
				filesystem::path p(fullPath);
				string filename = p.filename().string();
				string basename = filename.substr(0, filename.length() - 7);
				scriptPaths[path][basename] = fullPath;
			}
			else if (ext == ".action") {
				actionPaths[path].push_back(fullPath);
			}
			else if (ext == ".uplugin") {
				pluginPaths[path].push_back(fullPath);
			}
		}
	}
}

void Config::RemoveResourcePath(const string& path) {
	for (auto it = resourcePaths.begin(); it != resourcePaths.end(); ) {
		if (*it == path) {
			it = resourcePaths.erase(it);
		}
		else {
			++it;
		}
	}
	layoutPaths.erase(path);
	scriptPaths.erase(path);
	actionPaths.erase(path);
	pluginPaths.erase(path);
}

unordered_set<string> Config::GetStories() {
	return storyPaths;
}

void Config::AddScript(const string& path) {
	storyPaths.insert(path);
}

void Config::RemoveScript(const string& path) {
	storyPaths.erase(path);
}

