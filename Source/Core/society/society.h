#pragma once

#include "story/story.h"


class Society {
public:
	Society();
	~Society();

	// 读取配置文件
	void LoadConfigs() const;

	// 应用剧情变化
	void ApplyChange(Change* change,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues);

private:
};

