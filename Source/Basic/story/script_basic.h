#pragma once

#include "script_mod.h"


// NPC脚本
class NPCScript : public ScriptMod {
public:
	NPCScript();
	virtual ~NPCScript();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static bool MainStory();

	virtual void SetScript();

	virtual void PreTrigger(const Event* event);

	virtual void PostTrigger(const Event* event);

private:
	static int count;

	int id;
	std::string name;
};
