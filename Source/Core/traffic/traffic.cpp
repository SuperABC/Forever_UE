#include "traffic.h"
#include "utility.h"
#include "error.h"
#include "json.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <random>


using namespace std;

Traffic::Traffic() {

}

Traffic::~Traffic() {

}

void Traffic::SetResourcePath(string path) {
	resourcePath = path;
}

void Traffic::Init() {

}

void Traffic::ReadConfigs(string path) const {
	path = resourcePath + path;
	if (!filesystem::exists(path)) {
		THROW_EXCEPTION(IOException, "Path does not exist: " + path + ".\n");
	}

	JsonReader reader;
	JsonValue root;

	ifstream fin(path);
	if (!fin.is_open()) {
		THROW_EXCEPTION(IOException, "Failed to open file: " + path + ".\n");
	}
	if (reader.Parse(fin, root)) {

	}
	else {
		fin.close();
		THROW_EXCEPTION(JsonFormatException, "Json syntax error: " + reader.GetErrorMessages() + ".\n");
	}
	fin.close();
}

void Traffic::Destroy() {

}

void Traffic::Tick(int day, int hour, int min, int sec) {

}

void Traffic::Print() const {

}

void Traffic::Load(string path) {

}

void Traffic::Save(string path) const {

}

void Traffic::ApplyChange(Change* change, Story* story,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {

}


