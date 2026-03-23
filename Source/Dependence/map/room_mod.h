#pragma once

#include "../common/utility.h"
#include "../common/error.h"
#include "../common/geometry.h"

#include <string>
#include <functional>


class RoomMod : public Quad {
public:
	// 无构造
	RoomMod();

	// 无析构
	virtual ~RoomMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 配置房间属性
	virtual void ConfigRoom() = 0;

	// 是否可居住
	bool isResidential;

	// 是否可工作
	bool isWorkspace;

	// 是否含仓库
	bool isStorage;

	// 是否含工坊
	bool isManufacture;

	// 居住空间
	int residentialCapacity;

	// 工作空间
	int workspaceCapacity;

	// 仓库属性
	std::pair<std::string, float> storageConfig;

	// 工坊类型
	std::vector<std::string> manufactureTypes;
};

class RoomFactory {
public:
	// 注册房间
	void RegisterRoom(const std::string& id,
		std::function<RoomMod* ()> creator, std::function<void(RoomMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建房间
	RoomMod* CreateRoom(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 析构房间
	void DestroyRoom(RoomMod* terrain) const;

private:
	std::unordered_map<
		std::string,
		std::pair<std::function<RoomMod* ()>, std::function<void(RoomMod*)>>
	> registries;
	std::unordered_map<std::string, bool> configs;
};

