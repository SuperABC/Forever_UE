#include "player.h"
#include "utility.h"
#include "error.h"

using namespace std;

Player::Player() {

}

Player::~Player() {

}

void Player::LoadConfigs() const {
	//skillFactory->RemoveAll();

	//auto skills = Config::GetEnables("skill");
	//for (auto skill : skills) {
	//	skillFactory->SetConfig(skill, true);
	//}
}

void Player::ApplyChange(Change* change, Story* story,
	std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) {

}
