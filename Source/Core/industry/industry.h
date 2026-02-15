#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>


class Industry {
public:
	Industry();
	~Industry();

	// 设置资源路径
	void SetResourcePath(std::string path);

	// 读取Mods

	// 初始化全部工业
	void Init();

	// 读取配置文件
	void ReadConfigs(std::string path) const;

	// 释放空间
	void Destroy();

	// 时钟前进
	void Tick(int day, int hour, int min, int sec);

	// 输出当前工业
	void Print() const;

	// 保存/加载工业
	void Load(std::string path);
	void Save(std::string path) const;

private:
	// 资源路径
	std::string resourcePath;

	// Mod管理

};

