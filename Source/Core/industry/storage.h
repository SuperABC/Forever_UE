#pragma once

#include "industry/storage_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModStoragesFunc)(StorageFactory* factory);

// 仓库实体
class Storage {
public:
	/*
	* 禁止默认构造
	*/
	Storage() = delete;

	/*
	* 通过类型从工厂构造
	* @factory: 仓库工厂
	* @storage: 仓库静态类型标识
	*/
	Storage(StorageFactory* factory, const std::string& storage);

	/*
	* 析构仓库
	*/
	~Storage();

	/*
	* 获取类型
	*/
	std::string GetType() const;

	/*
	* 获取名称
	*/
	std::string GetName() const;

	/*
	* 获取所在房间
	*/
	Room* GetRoom() const;

	/*
	* 设置所在房间
	* @room: 所在房间
	*/
	void SetRoom(Room* room);

	/*
	* 设置参数
	* @acreage: 房间面积
	*/
	void SetProperty(float acreage);

	/*
	* 获取支持产品类型
	*/
	std::vector<std::string> GetCategories() const;

	/*
	* 获取是否接收产品
	*/
	bool GetAccept() const;

	/*
	* 设置是否接收产品
	* @accept: 是否接收
	*/
	void SetAccept(bool accept);

	/*
	* 获取是否提供产品
	*/
	bool GetProvide() const;

	/*
	* 设置是否提供产品
	* @provide: 是否提供
	*/
	void SetProvide(bool provide);

	/*
	* 获取一种存储产品
	* @type: 产品类型标识
	*/
	Product* GetProduct(const std::string& type) const;

	/*
	* 获取全部存储产品
	*/
	std::unordered_map<std::string, Product*> GetProducts() const;

	/*
	* 输入产品，返回实际输入量
	* @product: 产品类型标识
	* @amount: 期望输入量
	*/
	float InputProduct(const std::string& product, float amount);

	/*
	* 输出产品，返回实际输出量
	* @product: 产品类型标识
	* @amount: 期望输出量
	*/
	float OutputProduct(const std::string& product, float amount);

	/*
	* 获取上游仓库
	*/
	std::unordered_map<std::string, Storage*> GetUpstreams() const;

	/*
	* 连接上游仓库
	* @type: 产品类型标识
	* @storage: 上游仓库
	*/
	void ConnectUpstream(std::string type, Storage* storage);

	/*
	* 获取下游仓库
	*/
	std::unordered_map<std::string, Storage*> GetDownstreams() const;

	/*
	* 连接下游仓库
	* @type: 产品类型标识
	* @storage: 下游仓库
	*/
	void ConnectDownstream(std::string type, Storage* storage);

	/*
	* 获取容量
	*/
	float GetVolume() const;

	/*
	* 设置容量
	* @volume: 新容量
	*/
	void SetVolume(float volume);

	/*
	* 获取剩余空间
	*/
	float GetSpace() const;

	/*
	* 获取已占用空间
	*/
	float GetOccupied() const;

private:
	// 模组对象
	OBJECT_HOLDER StorageMod* mod;

	// 工厂
	StorageFactory* factory;

	// 仓库类型
	std::string type;

	// 仓库名称
	std::string name;

	// 所在房间
	Room* room;

	// 支持产品类型
	std::vector<std::string> categories;

	// 是否接入系统物流输入
	bool accept;

	// 是否接入系统物流输出
	bool provide;

	// 存放产品
	OBJECT_HOLDER std::unordered_map<std::string, Product*> products;

	// 系统物流输入方
	std::unordered_map<std::string, Storage*> upstreams;

	// 系统物流输出方
	std::unordered_map<std::string, Storage*> downstreams;

	// 仓库容量
	float volume;

};

// 空仓库
class EmptyStorage : public StorageMod {
public:
	/*
	* 构造空仓库
	*/
	EmptyStorage();

	/*
	* 析构空仓库
	*/
	virtual ~EmptyStorage();

	/*
	* Override
	* 仓库静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 仓库动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 仓库实例名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 设置仓库参数
	*/
	virtual void SetProperty() override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;

};
