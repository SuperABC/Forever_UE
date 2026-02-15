#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>


class Traffic {
public:
	Traffic();
	~Traffic();

	// 设置资源路径
	void SetResourcePath(std::string path);

	// 读取Mods

	// 初始化全部交通
	void Init();

	// 读取配置文件
	void ReadConfigs(std::string path) const;

	// 释放空间
	void Destroy();

	// 时钟前进
	void Tick(int day, int hour, int min, int sec);

	// 输出当前交通
	void Print() const;

	// 保存/加载交通
	void Load(std::string path);
	void Save(std::string path) const;

private:
	// 资源路径
	std::string resourcePath;

	// Mod管理

};

