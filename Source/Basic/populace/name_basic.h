#pragma once

#include "name_mod.h"


// 中文姓名
class ChineseName : public NameMod {
public:
	ChineseName();
	virtual ~ChineseName();

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
private:
	void InitializeSurnames();
	void InitializeNames();

	std::vector<std::string> surnames;
	std::vector<std::string> maleNames;
	std::vector<std::string> femaleNames;
	std::vector<std::string> neutralNames;

	static int count;

	int id;
	std::string name;
};
