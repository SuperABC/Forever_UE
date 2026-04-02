#include "player.h"

#include "skill.h"

using namespace std;

Player::Player() :
	time(nullptr) {
	time = new Time();
}

Player::~Player() {
	delete time;
}

void Player::LoadConfigs() const {
	//skillFactory->RemoveAll();

	//auto skills = Config::GetEnables("skill");
	//for (auto skill : skills) {
	//	skillFactory->SetConfig(skill, true);
	//}
}

void Player::ApplyChange(Change* change,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {

}

Time* Player::GetTime() {
	return time;
}
