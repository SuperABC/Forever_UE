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
		CheckMod("skill", "empty", false);
		for (auto skillMod : root["skill_mods"]) {
			CheckMod("skill", skillMod.AsString(), true);
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
	for (auto dllPath : dllPaths) {
		JsonValue dll = JsonValue(dllPath.first);
		dlls.append(dll);
	}
	root["dll_paths"] = dlls;

	JsonValue terrains = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["terrain"]) {
		if (modEnable.second) {
			JsonValue terrain = JsonValue(modEnable.first);
			terrains.append(terrain);
		}
	}
	root["terrain_mods"] = terrains;

	JsonValue roadnets = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["roadnet"]) {
		if (modEnable.second) {
			JsonValue roadnet = JsonValue(modEnable.first);
			roadnets.append(roadnet);
		}
	}
	if (roadnets.size() > 1) {
		debugf("Warning: Selected more than one roadnet mods.\n");
	}
	root["roadnet_mod"] = roadnets[0];

	JsonValue zones = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["zone"]) {
		if (modEnable.second) {
			JsonValue zone = JsonValue(modEnable.first);
			zones.append(zone);
		}
	}
	root["zone_mods"] = zones;

	JsonValue buildings = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["building"]) {
		if (modEnable.second) {
			JsonValue building = JsonValue(modEnable.first);
			buildings.append(building);
		}
	}
	root["building_mods"] = buildings;

	JsonValue components = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["component"]) {
		if (modEnable.second) {
			JsonValue component = JsonValue(modEnable.first);
			components.append(component);
		}
	}
	root["component_mods"] = components;

	JsonValue rooms = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["room"]) {
		if (modEnable.second) {
			JsonValue room = JsonValue(modEnable.first);
			rooms.append(room);
		}
	}
	root["room_mods"] = rooms;

	JsonValue assets = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["asset"]) {
		if (modEnable.second) {
			JsonValue asset = JsonValue(modEnable.first);
			assets.append(asset);
		}
	}
	root["asset_mods"] = assets;

	JsonValue names = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["name"]) {
		if (modEnable.second) {
			JsonValue name = JsonValue(modEnable.first);
			names.append(name);
		}
	}
	if (names.size() > 1) {
		debugf("Warning: Selected more than one name mods.\n");
	}
	root["name_mod"] = names[0];

	JsonValue schedulers = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["scheduler"]) {
		if (modEnable.second) {
			JsonValue scheduler = JsonValue(modEnable.first);
			schedulers.append(scheduler);
		}
	}
	root["scheduler_mods"] = schedulers;

	JsonValue jobs = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["job"]) {
		if (modEnable.second) {
			JsonValue job = JsonValue(modEnable.first);
			jobs.append(job);
		}
	}
	root["job_mods"] = jobs;

	JsonValue calendars = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["calendar"]) {
		if (modEnable.second) {
			JsonValue calendar = JsonValue(modEnable.first);
			calendars.append(calendar);
		}
	}
	root["calendar_mods"] = calendars;

	JsonValue organizations = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["organization"]) {
		if (modEnable.second) {
			JsonValue organization = JsonValue(modEnable.first);
			organizations.append(organization);
		}
	}
	root["organization_mods"] = organizations;

	JsonValue scripts = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["script"]) {
		if (modEnable.second) {
			JsonValue script = JsonValue(modEnable.first);
			scripts.append(script);
		}
	}
	root["script_mods"] = scripts;

	JsonValue products = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["product"]) {
		if (modEnable.second) {
			JsonValue product = JsonValue(modEnable.first);
			products.append(product);
		}
	}
	root["product_mods"] = products;

	JsonValue storages = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["storage"]) {
		if (modEnable.second) {
			JsonValue storage = JsonValue(modEnable.first);
			storages.append(storage);
		}
	}
	root["storage_mods"] = storages;

	JsonValue manufactures = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["manufacture"]) {
		if (modEnable.second) {
			JsonValue manufacture = JsonValue(modEnable.first);
			manufactures.append(manufacture);
		}
	}
	root["manufacture_mods"] = manufactures;

	JsonValue routes = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["route"]) {
		if (modEnable.second) {
			JsonValue route = JsonValue(modEnable.first);
			routes.append(route);
		}
	}
	root["route_mods"] = routes;

	JsonValue stations = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["station"]) {
		if (modEnable.second) {
			JsonValue station = JsonValue(modEnable.first);
			stations.append(station);
		}
	}
	root["station_mods"] = stations;

	JsonValue vehicles = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["vehicle"]) {
		if (modEnable.second) {
			JsonValue vehicle = JsonValue(modEnable.first);
			vehicles.append(vehicle);
		}
	}
	root["vehicle_mods"] = vehicles;

	JsonValue skills = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["skill"]) {
		if (modEnable.second) {
			JsonValue skill = JsonValue(modEnable.first);
			skills.append(skill);
		}
	}
	root["skill_mods"] = skills;

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
	for (auto dllPath : dllPaths) {
		paths.push_back(dllPath.first);
	}
	return paths;
}

vector<string> Config::GetMods() {
	vector<string> mods;
	unordered_set<string> ids;
	for (auto dllPath : dllPaths) {
		for (auto mod : dllPath.second) {
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
				auto getTerrainFunc = (GetModFunc)GetProcAddress(modHandle, "GetModTerrains");
				if (getTerrainFunc) {
					loaded = true;
					auto terrains = *(vector<string>*)getTerrainFunc();
					for (auto terrain : terrains) {
						modEnables["terrain"].insert({ terrain, false });
					}
				}

				auto getRoadnetFunc = (GetModFunc)GetProcAddress(modHandle, "GetModRoadnets");
				if (getRoadnetFunc) {
					loaded = true;
					auto roadnets = *(vector<string>*)getRoadnetFunc();
					for (auto roadnet : roadnets) {
						modEnables["roadnet"].insert({ roadnet, false });
					}
				}

				auto getZoneFunc = (GetModFunc)GetProcAddress(modHandle, "GetModZones");
				if (getZoneFunc) {
					loaded = true;
					auto zones = *(vector<string>*)getZoneFunc();
					for (auto zone : zones) {
						modEnables["zone"].insert({ zone, false });
					}
				}

				auto getBuildingFunc = (GetModFunc)GetProcAddress(modHandle, "GetModBuildings");
				if (getBuildingFunc) {
					loaded = true;
					auto buildings = *(vector<string>*)getBuildingFunc();
					for (auto building : buildings) {
						modEnables["building"].insert({ building, false });
					}
				}

				auto getComponentFunc = (GetModFunc)GetProcAddress(modHandle, "GetModComponents");
				if (getComponentFunc) {
					loaded = true;
					auto components = *(vector<string>*)getComponentFunc();
					for (auto component : components) {
						modEnables["component"].insert({ component, false });
					}
				}

				auto getRoomFunc = (GetModFunc)GetProcAddress(modHandle, "GetModRooms");
				if (getRoomFunc) {
					loaded = true;
					auto rooms = *(vector<string>*)getRoomFunc();
					for (auto room : rooms) {
						modEnables["room"].insert({ room, false });
					}
				}

				auto getAssetFunc = (GetModFunc)GetProcAddress(modHandle, "GetModAssets");
				if (getAssetFunc) {
					loaded = true;
					auto assets = *(vector<string>*)getAssetFunc();
					for (auto asset : assets) {
						modEnables["asset"].insert({ asset, false });
					}
				}

				auto getNameFunc = (GetModFunc)GetProcAddress(modHandle, "GetModNames");
				if (getNameFunc) {
					loaded = true;
					auto names = *(vector<string>*)getNameFunc();
					for (auto name : names) {
						modEnables["name"].insert({ name, false });
					}
				}

				auto getSchedulerFunc = (GetModFunc)GetProcAddress(modHandle, "GetModSchedulers");
				if (getSchedulerFunc) {
					loaded = true;
					auto schedulers = *(vector<string>*)getSchedulerFunc();
					for (auto scheduler : schedulers) {
						modEnables["scheduler"].insert({ scheduler, false });
					}
				}

				auto getJobFunc = (GetModFunc)GetProcAddress(modHandle, "GetModJobs");
				if (getJobFunc) {
					loaded = true;
					auto jobs = *(vector<string>*)getJobFunc();
					for (auto job : jobs) {
						modEnables["job"].insert({ job, false });
					}
				}

				auto getCalendarFunc = (GetModFunc)GetProcAddress(modHandle, "GetModCalendars");
				if (getCalendarFunc) {
					loaded = true;
					auto calendars = *(vector<string>*)getCalendarFunc();
					for (auto calendar : calendars) {
						modEnables["calendar"].insert({ calendar, false });
					}
				}

				auto getOrganizationFunc = (GetModFunc)GetProcAddress(modHandle, "GetModOrganizations");
				if (getOrganizationFunc) {
					loaded = true;
					auto organizations = *(vector<string>*)getOrganizationFunc();
					for (auto organization : organizations) {
						modEnables["organization"].insert({ organization, false });
					}
				}

				auto getEventFunc = (GetModFunc)GetProcAddress(modHandle, "GetModEvents");
				if (getEventFunc) {
					loaded = true;
					auto events = *(vector<string>*)getEventFunc();
					for (auto event : events) {
						modEnables["event"].insert({ event, false });
					}
				}

				auto getChangeFunc = (GetModFunc)GetProcAddress(modHandle, "GetModChanges");
				if (getChangeFunc) {
					loaded = true;
					auto changes = *(vector<string>*)getChangeFunc();
					for (auto change : changes) {
						modEnables["change"].insert({ change, false });
					}
				}

				auto getProductFunc = (GetModFunc)GetProcAddress(modHandle, "GetModProducts");
				if (getProductFunc) {
					loaded = true;
					auto products = *(vector<string>*)getProductFunc();
					for (auto product : products) {
						modEnables["product"].insert({ product, false });
					}
				}

				auto getStorageFunc = (GetModFunc)GetProcAddress(modHandle, "GetModStorages");
				if (getStorageFunc) {
					loaded = true;
					auto storages = *(vector<string>*)getStorageFunc();
					for (auto storage : storages) {
						modEnables["storage"].insert({ storage, false });
					}
				}

				auto getManufactureFunc = (GetModFunc)GetProcAddress(modHandle, "GetModManufactures");
				if (getManufactureFunc) {
					loaded = true;
					auto manufactures = *(vector<string>*)getManufactureFunc();
					for (auto manufacture : manufactures) {
						modEnables["manufacture"].insert({ manufacture, false });
					}
				}

				auto getRouteFunc = (GetModFunc)GetProcAddress(modHandle, "GetModRoutes");
				if (getRouteFunc) {
					loaded = true;
					auto routes = *(vector<string>*)getRouteFunc();
					for (auto route : routes) {
						modEnables["route"].insert({ route, false });
					}
				}

				auto getStationFunc = (GetModFunc)GetProcAddress(modHandle, "GetModStations");
				if (getStationFunc) {
					loaded = true;
					auto stations = *(vector<string>*)getStationFunc();
					for (auto station : stations) {
						modEnables["station"].insert({ station, false });
					}
				}

				auto getVehicleFunc = (GetModFunc)GetProcAddress(modHandle, "GetModVehicles");
				if (getVehicleFunc) {
					loaded = true;
					auto vehicles = *(vector<string>*)getVehicleFunc();
					for (auto vehicle : vehicles) {
						modEnables["vehicle"].insert({ vehicle, false });
					}
				}

				auto getSkillFunc = (GetModFunc)GetProcAddress(modHandle, "GetModSkills");
				if (getSkillFunc) {
					loaded = true;
					auto skills = *(vector<string>*)getSkillFunc();
					for (auto skill : skills) {
						modEnables["skill"].insert({ skill, false });
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
	for (auto mod : dllPaths[path]) {
		bool duplicated = false;
		for (auto dllPath : dllPaths) {
			if (dllPath.first == path)continue;
			for (auto dll : dllPath.second) {
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
						auto getTerrainFunc = (GetModFunc)GetProcAddress(modHandle, "GetModTerrains");
						if (getTerrainFunc) {
							auto terrains = *(vector<string>*)getTerrainFunc();
							for (auto terrain : terrains) {
								modEnables["terrain"].erase(terrain);
							}
						}

						auto getRoadnetFunc = (GetModFunc)GetProcAddress(modHandle, "GetModRoadnets");
						if (getRoadnetFunc) {
							auto roadnets = *(vector<string>*)getRoadnetFunc();
							for (auto roadnet : roadnets) {
								modEnables["roadnet"].erase(roadnet);
							}
						}

						auto getZoneFunc = (GetModFunc)GetProcAddress(modHandle, "GetModZones");
						if (getZoneFunc) {
							auto zones = *(vector<string>*)getZoneFunc();
							for (auto zone : zones) {
								modEnables["zone"].erase(zone);
							}
						}

						auto getBuildingFunc = (GetModFunc)GetProcAddress(modHandle, "GetModBuildings");
						if (getBuildingFunc) {
							auto buildings = *(vector<string>*)getBuildingFunc();
							for (auto building : buildings) {
								modEnables["building"].erase(building);
							}
						}

						auto getComponentFunc = (GetModFunc)GetProcAddress(modHandle, "GetModComponents");
						if (getComponentFunc) {
							auto components = *(vector<string>*)getComponentFunc();
							for (auto component : components) {
								modEnables["component"].erase(component);
							}
						}

						auto getRoomFunc = (GetModFunc)GetProcAddress(modHandle, "GetModRooms");
						if (getRoomFunc) {
							auto rooms = *(vector<string>*)getRoomFunc();
							for (auto room : rooms) {
								modEnables["room"].erase(room);
							}
						}

						auto getAssetFunc = (GetModFunc)GetProcAddress(modHandle, "GetModAssets");
						if (getAssetFunc) {
							auto assets = *(vector<string>*)getAssetFunc();
							for (auto asset : assets) {
								modEnables["asset"].erase(asset);
							}
						}

						auto getNameFunc = (GetModFunc)GetProcAddress(modHandle, "GetModNames");
						if (getNameFunc) {
							auto names = *(vector<string>*)getNameFunc();
							for (auto name : names) {
								modEnables["name"].erase(name);
							}
						}

						auto getSchedulerFunc = (GetModFunc)GetProcAddress(modHandle, "GetModSchedulers");
						if (getSchedulerFunc) {
							auto schedulers = *(vector<string>*)getSchedulerFunc();
							for (auto scheduler : schedulers) {
								modEnables["scheduler"].erase(scheduler);
							}
						}

						auto getJobFunc = (GetModFunc)GetProcAddress(modHandle, "GetModJobs");
						if (getJobFunc) {
							auto jobs = *(vector<string>*)getJobFunc();
							for (auto job : jobs) {
								modEnables["job"].erase(job);
							}
						}

						auto getCalendarFunc = (GetModFunc)GetProcAddress(modHandle, "GetModCalendars");
						if (getCalendarFunc) {
							auto calendars = *(vector<string>*)getCalendarFunc();
							for (auto calendar : calendars) {
								modEnables["calendar"].erase(calendar);
							}
						}

						auto getOrganizationFunc = (GetModFunc)GetProcAddress(modHandle, "GetModOrganizations");
						if (getOrganizationFunc) {
							auto organizations = *(vector<string>*)getOrganizationFunc();
							for (auto organization : organizations) {
								modEnables["organization"].erase(organization);
							}
						}

						auto getEventFunc = (GetModFunc)GetProcAddress(modHandle, "GetModEvents");
						if (getEventFunc) {
							auto events = *(vector<string>*)getEventFunc();
							for (auto event : events) {
								modEnables["event"].erase(event);
							}
						}

						auto getChangeFunc = (GetModFunc)GetProcAddress(modHandle, "GetModChanges");
						if (getChangeFunc) {
							auto changes = *(vector<string>*)getChangeFunc();
							for (auto change : changes) {
								modEnables["change"].erase(change);
							}
						}

						auto getProductFunc = (GetModFunc)GetProcAddress(modHandle, "GetModProducts");
						if (getProductFunc) {
							auto products = *(vector<string>*)getProductFunc();
							for (auto product : products) {
								modEnables["product"].erase(product);
							}
						}

						auto getStorageFunc = (GetModFunc)GetProcAddress(modHandle, "GetModStorages");
						if (getStorageFunc) {
							auto storages = *(vector<string>*)getStorageFunc();
							for (auto storage : storages) {
								modEnables["storage"].erase(storage);
							}
						}

						auto getManufactureFunc = (GetModFunc)GetProcAddress(modHandle, "GetModManufactures");
						if (getManufactureFunc) {
							auto manufactures = *(vector<string>*)getManufactureFunc();
							for (auto manufacture : manufactures) {
								modEnables["manufacture"].erase(manufacture);
							}
						}

						auto getRouteFunc = (GetModFunc)GetProcAddress(modHandle, "GetModRoutes");
						if (getRouteFunc) {
							auto routes = *(vector<string>*)getRouteFunc();
							for (auto route : routes) {
								modEnables["route"].erase(route);
							}
						}

						auto getStationFunc = (GetModFunc)GetProcAddress(modHandle, "GetModStations");
						if (getStationFunc) {
							auto stations = *(vector<string>*)getStationFunc();
							for (auto station : stations) {
								modEnables["station"].erase(station);
							}
						}

						auto getVehicleFunc = (GetModFunc)GetProcAddress(modHandle, "GetModVehicles");
						if (getVehicleFunc) {
							auto vehicles = *(vector<string>*)getVehicleFunc();
							for (auto vehicle : vehicles) {
								modEnables["vehicle"].erase(vehicle);
							}
						}

						auto getSkillFunc = (GetModFunc)GetProcAddress(modHandle, "GetModSkills");
						if (getSkillFunc) {
							auto skills = *(vector<string>*)getSkillFunc();
							for (auto skill : skills) {
								modEnables["skill"].erase(skill);
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
	for (auto modEnable : modEnables[type]) {
		if (modEnable.second) {
			enables.push_back(modEnable.first);
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

std::string Config::GetScript(const std::string& name) {
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
}

unordered_set<string> Config::GetStories() {
	return storyPaths;
}

void Config::AddScript(const std::string& path) {
	storyPaths.insert(path);
}

void Config::RemoveScript(const std::string& path) {
	storyPaths.erase(path);
}

