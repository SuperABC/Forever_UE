#pragma once

#include "story/story.h"

#include "condition.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include <string>
#include <vector>
#include <functional>


class Industry {
public:
	Industry();
	~Industry();

	// 读取配置文件
	void LoadConfigs() const;

	// 应用剧情变化
	void ApplyChange(Change* change,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues);

private:
};

