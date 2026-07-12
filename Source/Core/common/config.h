#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <filesystem>
#include <windows.h>


class Config {
public:
	/*
	* 从文件读取配置，清空并重建所有配置数据
	* @path: 配置文件路径
	*/
	static void ReadConfig(const std::string& path);

	/*
	* 将当前配置序列化并写入文件
	* @path: 目标配置文件路径
	*/
	static void WriteConfig(const std::string& path);

	/*
	* 获取已注册的所有mod目录路径
	* @return: mod目录路径列表
	*/
	static std::vector<std::string> GetDllPaths();

	/*
	* 获取所有已发现的dll绝对路径
	* @return: dll绝对路径列表
	*/
	static std::vector<std::string> GetMods();

	/*
	* 添加mod目录并扫描其中的dll，识别并注册mod
	* @path: mod目录路径
	*/
	static void AddDllPath(const std::string& path);

	/*
	* 移除mod目录及对应的mod注册信息
	* @path: 要移除的mod目录路径
	*/
	static void RemoveDllPath(const std::string& path);

	/*
	* 获取指定类型的所有mod及其启用状态
	* @type: mod类型名称
	* @return: mod名称到启用状态的映射
	*/
	static std::unordered_map<std::string, bool> GetChecks(const std::string& type);

	/*
	* 设置指定类型下某mod的启用状态
	* @type: mod类型名称
	* @mod: mod名称
	* @enabled: true为启用，false为禁用
	*/
	static void CheckMod(const std::string& type, const std::string& mod, bool enabled);

	/*
	* 获取指定类型下所有已启用的mod名称列表
	* @type: mod类型名称
	* @return: 已启用mod名称列表
	*/
	static std::vector<std::string> GetEnables(const std::string& type);

	/*
	* 获取已注册的所有resource目录路径
	* @return: resource目录路径列表
	*/
	static std::vector<std::string> GetResourcePaths();

	/*
	* 获取所有已发现的layout文件绝对路径
	* @return: layout文件路径列表
	*/
	static std::vector<std::string> GetLayouts();

	/*
	* 获取所有已发现的script文件（名称到路径的映射）
	* @return: script名称到路径的映射表
	*/
	static std::unordered_map<std::string, std::string> GetScripts();

	/*
	* 按名称查找script文件路径
	* @name: script名称（不含扩展名）
	* @return: 对应的绝对路径，未找到返回空字符串
	*/
	static std::string GetScript(const std::string& name);

	/*
	* 获取所有已发现的action文件绝对路径
	* @return: action文件路径列表
	*/
	static std::vector<std::string> GetActions();

	/*
	* 获取所有已发现的uplugin文件绝对路径
	* @return: uplugin文件路径列表
	*/
	static std::vector<std::string> GetPlugins();

	/*
	* 添加resource目录并扫描其中的layout/script/action/uplugin文件
	* @path: resource目录路径
	*/
	static void AddResourcePath(const std::string& path);

	/*
	* 移除resource目录及其下所有已注册的资源文件路径
	* @path: 要移除的resource目录路径
	*/
	static void RemoveResourcePath(const std::string& path);

	/*
	* 获取所有已注册的主剧情路径集合
	* @return: 主剧情路径集合
	*/
	static std::unordered_set<std::string> GetStories();

	/*
	* 添加主剧情路径
	* @path: 主剧情文件路径
	*/
	static void AddScript(const std::string& path);

	/*
	* 删除主剧情路径
	* @path: 要删除的主剧情文件路径
	*/
	static void RemoveScript(const std::string& path);

private:
	/*
	* 检查文件是否为指定扩展名格式（大小写不敏感）
	* @filePath: 文件路径
	* @format: 目标扩展名
	* @return: 匹配返回true，否则false
	*/
	static bool CheckFileFormat(const std::filesystem::path& filePath, const std::string& format);

	// mod目录路径 -> [dll绝对路径列表]
	static std::unordered_map<std::string, std::vector<std::string>> dllPaths;

	// mod类型 -> { mod名称 -> 是否启用 }
	static std::unordered_map<std::string, std::unordered_map<std::string, bool>> modEnables;

	// 已注册的resource目录路径列表
	static std::vector<std::string> resourcePaths;

	// resource目录路径 -> [layout文件路径列表]
	static std::unordered_map<std::string, std::vector<std::string>> layoutPaths;

	// resource目录路径 -> { script名称 -> script文件路径 }
	static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> scriptPaths;

	// resource目录路径 -> [action文件路径列表]
	static std::unordered_map<std::string, std::vector<std::string>> actionPaths;

	// resource目录路径 -> [uplugin文件路径列表]
	static std::unordered_map<std::string, std::vector<std::string>> pluginPaths;

	// 主剧情文件路径集合
	static std::unordered_set<std::string> storyPaths;
};
