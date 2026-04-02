#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"


class Change;
class Player {
public:
	Player();
	~Player();

	// 读取配置文件
	void LoadConfigs() const;

	// 应用剧情变化
	void ApplyChange(Change* change,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues);

	// 获取时钟
	Time* GetTime();

private:
	// 时钟
	OBJECT_HOLDER Time* time;
};

