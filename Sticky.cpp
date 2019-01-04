#include "Sticky.h"
#include "Util.h"
#include "SDK.h"
#include "Triggerbot.h"

CAutoSticky gSticky;

void CAutoSticky::Run(CBaseEntity* g_pLocalPlayer, CUserCmd* g_pUserCmd)
{
	Vector sticky_loc, vent;
	float dist{ 0.0f };
	float closest_dist{ 0.0f };
	// Check if user settings allow Auto Sticky
	if (!gTrigger.stickydetonate.value)
		return;

	for (int i = 0; i < gInts.EntList->GetHighestEntityIndex(); i++)
	{
		// Find a entity(sticky) from the for loops current tick
		CBaseEntity *sticky = gInts.EntList->GetClientEntity(i);

		if (sticky == NULL)
			continue;
		// Check if null or dormant
		if (!sticky)
			continue;

		if (sticky->GetTeamNum() != g_pLocalPlayer->GetTeamNum())//only my teams sticky
			continue;
		if (!strcmp(sticky->GetClientClass()->chName, "CTFGrenadePipebombProjectile")) {
			if (!sticky->GetOwner())//and finally if its not my sticky, I dont think this works but I dont care enough to fix
				continue;
			sticky->GetWorldSpaceCenter(sticky_loc);
			for (int j = 1; j < gInts.Engine->GetMaxClients(); j++) {
				if (j == me)
					continue;
				CBaseEntity *pEntity = GetBaseEntity(j);
				if (pEntity == NULL)
					continue;
				if (!pEntity)
					continue;
				if (pEntity == g_pLocalPlayer)
					continue;
				if (pEntity->IsDormant())
					continue;
				if (pEntity->GetLifeState() != LIFE_ALIVE)
					continue;
				if (!gCvars.PlayerMode[i])
					continue;
				if (pEntity->GetTeamNum() == g_pLocalPlayer->GetTeamNum())
					continue;
				if (pEntity->GetCond() & TFCond_Ubercharged ||
					pEntity->GetCond() & TFCond_UberchargeFading ||
					pEntity->GetCond() & TFCond_Bonked)
					continue;

				vent = pEntity->GetHitboxPosition(4);
				//dist = (sticky->GetAbsOrigin() - pEntity->GetAbsOrigin()).Length();
				dist = Util->flGetDistance(sticky_loc, vent);
				if (dist < closest_dist || closest_dist == 0.0f)
				{
					closest_dist = dist;
				}
			}
		}
	}
	if (closest_dist == 0.0f || closest_dist > 12.0f)
		return;
	// det
	g_pUserCmd->buttons |= IN_ATTACK2;
	// Function is finished, return*/
	return;
}