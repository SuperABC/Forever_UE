#include "player.h"
#include "utility.h"
#include "error.h"
#include "json.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <random>


using namespace std;

Player::Player() {
	time = new Time();
}

Player::~Player() {
	delete time;
}

void Player::SetResourcePath(string path) {
	resourcePath = path;
}

void Player::Init() {

}

void Player::ReadConfigs(string path) const {
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

void Player::Destroy() {

}

void Player::Tick(int day, int hour, int min, int sec) {

}

void Player::Print() const {

}

void Player::Load(string path) {

}

void Player::Save(string path) const {

}

void Player::ApplyChange(Change* change, Story* story,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {

}

Time* Player::GetTime() {
	return time;
}

