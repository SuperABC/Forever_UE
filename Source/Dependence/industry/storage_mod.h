#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class StorageMod {
public:
	// 无构造
	StorageMod();

	// 无析构
	virtual ~StorageMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 设置参数
	COSTOM_INIT virtual void SetProperty() = 0;

	// 支持产品类型
	std::vector<std::string> catagories;

	// 容积率
	float density;

	// 是否接入系统物流输入
	bool accept;

	// 是否接入系统物流输出
	bool provide;

};

class StorageFactory {
public:
	// 注册仓库
	void RegisterStorage(const std::string& id,
		std::function<StorageMod* ()> creator, std::function<void(StorageMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建仓库
	StorageMod* CreateStorage(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 析构仓库
	void DestroyStorage(StorageMod* terrain) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<StorageMod* ()>, std::function<void(StorageMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

