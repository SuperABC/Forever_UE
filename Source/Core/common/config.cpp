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
unordered_map<string, unordered_map<string, string>> Config::jobPaths = {};
unordered_map<string, vector<string>> Config::characterPaths = {};
unordered_map<string, vector<string>> Config::actionPaths = {};
string Config::scriptPath = "";

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
	jobPaths.clear();
	characterPaths.clear();
	actionPaths.clear();
	scriptPath = "";

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

		for (auto terrainMod : root["terrain_mods"]) {
			CheckMod("terrain", terrainMod.AsString(), true);
		}
		CheckMod("roadnet", root["roadnet_mod"].AsString(), true);
		for (auto zoneMod : root["zone_mods"]) {
			CheckMod("zone", zoneMod.AsString(), true);
		}
		for (auto buildingMod : root["building_mods"]) {
			CheckMod("building", buildingMod.AsString(), true);
		}
		for (auto componentMod : root["component_mods"]) {
			CheckMod("component", componentMod.AsString(), true);
		}
		for (auto roomMod : root["room_mods"]) {
			CheckMod("room", roomMod.AsString(), true);
		}
		for (auto assetMod : root["asset_mods"]) {
			CheckMod("asset", assetMod.AsString(), true);
		}
		CheckMod("name", root["name_mod"].AsString(), true);
		for (auto schedulerMod : root["scheduler_mods"]) {
			CheckMod("scheduler", schedulerMod.AsString(), true);
		}
		for (auto jobMod : root["job_mods"]) {
			CheckMod("job", jobMod.AsString(), true);
		}
		for (auto calendarMod : root["calendar_mods"]) {
			CheckMod("calendar", calendarMod.AsString(), true);
		}
		for (auto organizationMod : root["organization_mods"]) {
			CheckMod("organization", organizationMod.AsString(), true);
		}
		for (auto eventMod : root["event_mods"]) {
			CheckMod("event", eventMod.AsString(), true);
		}
		for (auto changeMod : root["change_mods"]) {
			CheckMod("change", changeMod.AsString(), true);
		}
		for (auto productMod : root["product_mods"]) {
			CheckMod("product", productMod.AsString(), true);
		}
		for (auto storageMod : root["storage_mods"]) {
			CheckMod("storage", storageMod.AsString(), true);
		}
		for (auto manufactureMod : root["manufacture_mods"]) {
			CheckMod("manufacture", manufactureMod.AsString(), true);
		}
		for (auto routeMod : root["route_mods"]) {
			CheckMod("route", routeMod.AsString(), true);
		}
		for (auto stationMod : root["station_mods"]) {
			CheckMod("station", stationMod.AsString(), true);
		}
		for (auto vehicleMod : root["vehicle_mods"]) {
			CheckMod("vehicle", vehicleMod.AsString(), true);
		}
		for (auto skillMod : root["skill_mods"]) {
			CheckMod("skill", skillMod.AsString(), true);
		}

		for (auto resourcePath : root["resource_paths"]) {
			AddResourcePath(resourcePath.AsString());
		}

		SetScript(root["story_path"].AsString());
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
	root["roadnet_mods"] = roadnets;

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
	root["name_mods"] = names;

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

	JsonValue events = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["event"]) {
		if (modEnable.second) {
			JsonValue event = JsonValue(modEnable.first);
			events.append(event);
		}
	}
	root["event_mods"] = events;

	JsonValue changes = JsonValue(DATA_ARRAY);
	for (auto modEnable : modEnables["change"]) {
		if (modEnable.second) {
			JsonValue change = JsonValue(modEnable.first);
			changes.append(change);
		}
	}
	root["change_mods"] = changes;

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
	root["dll_paths"] = resources;

	JsonValue story = JsonValue(scriptPath);
	root["story_path"] = story;

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
}

vector<string> Config::GetMods() {
	vector<string> mods;
	for (auto dllPath : dllPaths) {
		for (auto mod : dllPath.second) {
			mods.push_back(mod);
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

	dllPaths[path].clear();

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

unordered_map<string, string> Config::GetJobs() {
	unordered_map<string, string> jobs;

	for (auto& jobPath : jobPaths) {
		jobs.insert(jobPath.second.begin(), jobPath.second.end());
	}

	return jobs;
}

vector<string> Config::GetCharacters() {
	vector<string> characters;

	for (auto& characterPath : characterPaths) {
		characters.insert(characters.end(), characterPath.second.begin(), characterPath.second.end());
	}

	return characters;
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

	resourcePaths.push_back(path);
	layoutPaths[path].clear();
	jobPaths[path].clear();
	characterPaths[path].clear();
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
			else if (ext == ".job") {
				filesystem::path p(fullPath);
				string filename = p.filename().string();
				string basename = filename.substr(0, filename.length() - 4);
				jobPaths[path][basename] = fullPath;
			}
			else if (ext == ".character") {
				characterPaths[path].push_back(fullPath);
			}
			else if (ext == ".action") {
				actionPaths[path].push_back(fullPath);
			}
		}
	}
}

void Config::RemoveResourcePath(const string& path) {
	layoutPaths.erase(path);
	jobPaths.erase(path);
	characterPaths.erase(path);
	actionPaths.erase(path);
}

string Config::GetScript() {
	return scriptPath;
}

void Config::SetScript(const string& path) {
	scriptPath = path;
}
