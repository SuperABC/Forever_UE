#pragma once

#include "room_mod.h"


// 住宅房间
class ResidentialRoom : public RoomMod {
public:
	ResidentialRoom();
	virtual ~ResidentialRoom();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual void ConfigRoom();

private:
	static int count;

	int id;
	std::string name;
};
