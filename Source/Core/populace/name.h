#pragma once

#include "name_mod.h"

#include <unordered_set>


// 子类注册函数
typedef void (*RegisterModNamesFunc)(NameFactory* factory);

// 姓名实体
class Name {
public:
	// 禁止默认构造
	Name() = delete;

	// 通过类型从工厂构造
	Name(NameFactory* factory, const std::string& component);

	// 析构姓名
	~Name();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 从姓名中获取姓
	virtual void GetSurname(std::function<void(const std::string&)> set,
		const std::string& name);

	// 全随机生成姓名
	virtual void GenerateName(std::function<void(const std::string&)> set,
		bool male = true, bool female = true, bool neutral = true);

	// 给定姓随机生成名
	virtual void GenerateName(std::function<void(const std::string&)> set,
		const std::string& surname, bool male = true, bool female = true, bool neutral = true);

	// 占用名字
	bool ReserveName(const std::string& name);

	// 使用名字
	bool RegisterName(const std::string& name);

private:
	// 模组对象
	OBJECT_HOLDER NameMod* mod;

	// 工厂
	NameFactory* factory;

	// 姓名类型
	std::string type;

	// 姓名名称
	std::string name;

	// 姓名名称
	std::unordered_set<std::string> reserve;

	// 姓名名称
	std::unordered_set<std::string> roll;
};

// 空姓名
class EmptyName : public NameMod {
public:
	EmptyName();
	virtual ~EmptyName();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void GetSurname(std::function<void(const std::string&)> set,
		const std::string& name);

	virtual void GenerateName(std::function<void(const std::string&)> set,
		bool male = true, bool female = true, bool neutral = true);

	virtual void GenerateName(std::function<void(const std::string&)> set,
		const std::string& surname, bool male = true, bool female = true, bool neutral = true);
private:
	static int count;

	int id;
	std::string name;
};

