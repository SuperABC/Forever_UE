#pragma once

#include "terrain.h"

#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>

#define BLOCK_SIZE 256


class Element {
public:
	Element();
	~Element();

	//获取/设置地形名称
	std::string GetTerrain() const;
	bool SetTerrain(std::string terrain);

	//获取/设置园区标识
	std::string GetZone() const;
	bool SetZone(std::string zone);

	//获取/设置建筑标识
	std::string GetBuilding() const;
	bool SetBuilding(std::string building);

private:
	// 基础属性
	std::string terrain = "plain";
	std::string zone;
	std::string building;
};

class Block {
public:
	Block() = delete;
	Block(int x, int y);
	~Block();

	//获取/设置地形名称
	std::string GetTerrain(int x, int y) const;
	bool SetTerrain(int x, int y, std::string terrain);

	//检查全局坐标是否在地块内
	bool CheckXY(int x, int y) const;

	//获取地块内某全局坐标的信息
	std::shared_ptr<Element> GetElement(int x, int y);

private:
	// 基础内容
	int offsetX, offsetY;
	std::vector<std::vector<std::shared_ptr<Element>>> elements;
};

class Map {
public:
	Map();
	~Map();

	// 设置资源路径
	void SetResourcePath(std::string path);

	// 读取Mods
	void InitTerrains(std::unordered_map<std::string, HMODULE>& modHandles);

	// 读取配置文件
	void ReadConfigs(std::string path) const;

	// 初始化全部地图
	int Init(int blockX, int blockY);

	// 释放空间
	void Destroy();

	// 时钟前进
	void Tick(int day, int hour, int min, int sec);

	// 输出当前地图
	void Print() const;

	// 保存/加载地图
	void Load(std::string path);
	void Save(std::string path) const;

	// 获取地图尺寸
	std::pair<int, int> GetSize() const;

	// 检查全局坐标是否在地图内
	bool CheckXY(int x, int y) const;

	// 获取地块
	std::shared_ptr<Block> GetBlock(int x, int y) const;

	// 获取元素
	std::shared_ptr<Element> GetElement(int x, int y) const;

	// 获取/设置地形名称
	std::string GetTerrain(int x, int y) const;
	bool SetTerrain(int x, int y, std::string terrain);

private:
	// 资源路径
	std::string resourcePath;

	// Mod管理
	static TerrainFactory* terrainFactory;

	// 基础内容
	int width = 0, height = 0;
	std::vector<std::vector<std::shared_ptr<Block>>> blocks;

};

