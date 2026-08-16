#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


// 仓库Mod基类
class StorageMod {
public:
	/*
	* 构造仓库
	*/
	StorageMod();

	/*
	* 析构仓库
	*/
	virtual ~StorageMod();

	/*
	* Override
	* 仓库静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 仓库动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 仓库实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 设置仓库参数
	*/
	COSTOM_INIT virtual void SetProperty() = 0;

	// 支持产品类型
	std::vector<std::string> categories;

	// 容积率
	float density;

	// 是否接入系统物流输入
	bool accept;

	// 是否接入系统物流输出
	bool provide;

};

class StorageFactory {
public:
	/*
	* 构造仓库工厂,预留暂存区容量
	*/
	StorageFactory();

	/*
	* 注册仓库
	* @id: 仓库类型
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterStorage(const std::string& id,
		std::function<StorageMod* ()> creator, std::function<void(StorageMod*)> deleter);

	/*
	* 将暂存数据合并进正式注册表
	*/
	void MergeTemp();

	/*
	* 清空暂存数据
	*/
	void CleanTemp();

	/*
	* 清空所有注册
	*/
	void RemoveAll();

	/*
	* 创建仓库
	* @id: 仓库类型
	*/
	StorageMod* CreateStorage(const std::string& id) const;

	/*
	* 检查仓库是否已注册
	* @id: 仓库类型
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置仓库启用配置
	* @name: 仓库类型
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 析构仓库
	* @storageMod: 待析构的仓库
	*/
	void DestroyStorage(StorageMod* storageMod) const;

private:
	// 暂存本轮注册写入的数据,避免跨模块直接写正式成员
	struct Temp {
		// 注册表
		std::unordered_map<
			std::string,
			std::pair<std::function<StorageMod* ()>, std::function<void(StorageMod*)>>
		> registries;
	};

	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<StorageMod* ()>, std::function<void(StorageMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;

	// 暂存数据
	Temp temp;

};
