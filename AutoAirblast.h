#pragma once
#pragma once
#include "SDK.h"
#include "Triggerbot.h"

class CAutoAirblast
{
public:

	void Run(CBaseEntity * g_pLocalPlayer, CUserCmd * g_pUserCmd);
};

extern CAutoAirblast gBlast;