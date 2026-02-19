#pragma once

#include "person.h"
#include "name.h"

#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>


class Map;
class Story;

class Populace {
public:
	Populace();
	~Populace();

	// 设置资源路径
	void SetResourcePath(std::string path);

	// 读取Mods
	void InitNames(std::unordered_map<std::string, HMODULE>& modHandles);

	// 读取配置文件
	void ReadConfigs(std::string path) const;

	// 初始化全部人口
	void Init(int accomodation, std::vector<std::string> nameholders, Time *time);

	// 释放空间
	void Destroy();

	// 时钟前进
	void Tick(int day, int hour, int min, int sec);

	// 输出当前人口
	void Print() const;

	// 保存/加载人口
	void Load(std::string path);
	void Save(std::string path) const;

private:
	// 生成市民
	void GenerateCitizens(int num, std::vector<std::string> nameholders, Time* time);
	void GenerateEducations(Time* time);
	void GenerateEmotions(Time* time);
	void GenerateJobs();

	// 资源路径
	std::string resourcePath;

	// Mod管理
	static NameFactory* nameFactory;

	// 市民管理
	Name* names = nullptr;
	std::vector<Person*> citizens;
	std::unordered_map<std::string, int> ids;
};

