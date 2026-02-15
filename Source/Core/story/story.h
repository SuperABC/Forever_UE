#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>


class Story {
public:
	Story();
	~Story();

	// 设置资源路径
	void SetResourcePath(std::string path);

	// 读取Mods

	// 读取配置文件
	void ReadConfigs(std::string path) const;

	// 初始化全部剧情
	void Init();

	// 释放空间
	void Destroy();

	// 时钟前进
	void Tick(int day, int hour, int min, int sec);

	// 输出当前剧情
	void Print() const;

	// 保存/读取剧情
	void Load(std::string path);
	void Save(std::string path) const;

private:
	// 资源路径
	std::string resourcePath;

	// Mod管理

};

