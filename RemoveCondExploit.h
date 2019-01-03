#pragma once
#include "SDK.h"
#include "Util.h"
//a
class RemoveCondExploits
{
public:

	void Run(CBaseEntity * pLocal, CUserCmd * pCommand);

	void RemoveConds(CBaseEntity * local, int value = 900);

	void RemoveConds(CBaseEntity * local, CUserCmd * cmd, int value = 900, bool disableattack = true);

	bool insRezoomDepressed = false;
	bool insRezoomMain = false;
	float instant_rezoom_tick = 0;
	bool instant_rezoom_timer_active = false;
	int instant_rezoom_timer_step = 0;

};

extern RemoveCondExploits gCond;