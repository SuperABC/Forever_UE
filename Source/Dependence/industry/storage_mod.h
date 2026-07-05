#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class StorageMod {
public:
	/*
	* 构造仓库模组基类
	*/
	StorageMod();

	/*
	* 析构仓库模组基类
	*/
	virtual ~StorageMod();

	/*
	* Override
	* 仓库静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 仓库动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 仓库实例名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 设置仓库参数
	*/
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
	/*
	* 注册仓库
	* @id: 仓库静态类型标识
	* @creator: 创建函数
	* @deleter: 析构函数
	*/
	void RegisterStorage(const std::string& id,
		std::function<StorageMod* ()> creator, std::function<void(StorageMod*)> deleter);

	/*
	* 清空启用配置
	*/
	void RemoveAll();

	/*
	* 创建仓库实例
	* @id: 仓库静态类型标识
	*/
	StorageMod* CreateStorage(const std::string& id) const;

	/*
	* 检查是否已注册
	* @id: 仓库静态类型标识
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置启用配置
	* @name: 仓库静态类型标识
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 析构仓库实例
	* @storageMod: 待析构的仓库模组
	*/
	void DestroyStorage(StorageMod* storageMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<StorageMod* ()>, std::function<void(StorageMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;

};
