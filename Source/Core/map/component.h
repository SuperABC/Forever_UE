#pragma once

#include "component_base.h"


// 子类注册函数
typedef void (*RegisterModComponentsFunc)(ComponentFactory* factory);

// 默认住宅组合
class DefaultResidentialComponent : public Component {
public:
	DefaultResidentialComponent();
	virtual ~DefaultResidentialComponent();

	static std::string GetId();
	virtual std::string GetType() const override;
	virtual std::string GetName() const override;
};

// 默认工作组合
class DefaultWorkingComponent : public Component {
public:
	DefaultWorkingComponent();
	virtual ~DefaultWorkingComponent();

	static std::string GetId();
	virtual std::string GetType() const override;
	virtual std::string GetName() const override;
};