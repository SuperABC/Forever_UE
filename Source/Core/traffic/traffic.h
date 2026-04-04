#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"


class Traffic {
public:
	// 构造交通
	Traffic();

	// 析构交通
	~Traffic();

	// 读取配置文件
	void LoadConfigs() const;

	// 应用剧情变化
	void ApplyChange(Change* change,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues);

private:
};

