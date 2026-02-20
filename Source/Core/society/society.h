#pragma once

#include "calendar.h"
#include "organization.h"
#include "../map/map.h"
#include "../populace/populace.h"

#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>


class Society {
public:
	Society();
	~Society();

	// 设置资源路径
	void SetResourcePath(std::string path);

	// 读取Mods
	void InitCalendars(std::unordered_map<std::string, HMODULE>& modHandles);
	void InitOrganizations(std::unordered_map<std::string, HMODULE>& modHandles);

	// 初始化全部组织
	void Init(Map* map, Populace* populace, Time* time);

	// 读取配置文件
	void ReadConfigs(std::string path) const;

	// 释放空间
	void Destroy();

	// 时钟前进
	void Tick(int day, int hour, int min, int sec);

	// 输出当前组织
	void Print() const;

	// 保存/加载组织
	void Load(std::string path);
	void Save(std::string path) const;

private:
	// 资源路径
	std::string resourcePath;

	// Mod管理
	static CalendarFactory* calendarFactory;
	static OrganizationFactory* organizationFactory;

	std::vector<Organization*> organizations;
};

