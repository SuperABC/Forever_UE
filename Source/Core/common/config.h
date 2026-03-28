#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <windows.h>


class Config {
public:
	// 读取配置文件
	static void ReadConfig(const std::string& path);

	// 存储配置文件
	static void WriteConfig(const std::string& path);

	// 获取配置mod路径
	static std::vector<std::string> GetDllPaths();

	// 获取所有dll绝对路径
	static std::vector<std::string> GetMods();

	// 添加dllPath并识别其中包含的mod文件路径
	static void AddDllPath(const std::string& path);

	// 移除dllPath及对应的mod文件路径
	static void RemoveDllPath(const std::string& path);

	// 获取一类mod的启用情况
	static std::unordered_map<std::string, bool> GetChecks(const std::string& type);

	// 设置mod是否启用
	static void CheckMod(const std::string& type, const std::string& mod, bool enabled);

	// 获取一类所有启用mod
	static std::vector<std::string> GetEnables(const std::string& type);

	// 获取配置resource路径
	static std::vector<std::string> GetResourcePaths();

	// 获取所有layout路径
	static std::vector<std::string> GetLayouts();

	// 获取所有script路径
	static std::unordered_map<std::string, std::string> GetScripts();

	// 按名称查找script路径
	static std::string GetScript(const std::string& name);

	// 获取所有action路径
	static std::vector<std::string> GetActions();

	// 添加resourcePaths并识别其中包含的layout/script/action文件路径
	static void AddResourcePath(const std::string& path);

	// 移除resourcePaths及对应的layout/script/action文件路径
	static void RemoveResourcePath(const std::string& path);

	// 获取主剧情路径
	static std::unordered_set<std::string> GetStories();

	// 添加主剧情路径
	static void AddScript(const std::string& path);

	// 删除主剧情路径
	static void RemoveScript(const std::string& path);

private:
	// config path -> [dll paths]
	static std::unordered_map<std::string, std::vector<std::string>> dllPaths;

	// type -> [ { name, enabled } ]
	static std::unordered_map<std::string, std::unordered_map<std::string, bool>> modEnables;

	// [ resource path ]
	static std::vector<std::string> resourcePaths;

	// resource path -> [ layout path ]
	static std::unordered_map<std::string, std::vector<std::string>> layoutPaths;

	// resource path -> [ script name -> script path ]
	static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> scriptPaths;

	// resource path -> [ action path ]
	static std::unordered_map<std::string, std::vector<std::string>> actionPaths;

	// story path
	static std::unordered_set<std::string> storyPaths;
};

