#include "Triggerbot.h"
#include "Util.h"

CTriggerbot gTrigger;

bool didHit = false;
int iDelay = 0;

void CTriggerbot::Run(CBaseEntity* pLocal, CUserCmd* pCommand)
{


	if (pLocal->GetLifeState() != LIFE_ALIVE)
		return;

	if (GAME_TF2)
	{
		CBaseCombatWeapon* pWep = pLocal->GetActiveWeapon();

		if (backstab.value)
			if (pLocal->szGetClass() == "Spy" && pWep->GetSlot() == 2 && pWep->CanBackStab())
				pCommand->buttons |= IN_ATTACK;
	}

	if (!enabled.value)
		return;

	if (!Util->IsKeyPressedMisc(key.value))
		return;

	Vector vAim;
	gInts.Engine->GetViewAngles(vAim); //We use getviewangles so that this can work with anti-aim

	Vector vForward;
	AngleVectors(vAim, &vForward);


	if (GAME_TF2)
	{
		CBaseCombatWeapon* pWep = pLocal->GetActiveWeapon();

		auto pClass = pWep->GetItemDefinitionIndex();

		if (pWep->GetSlot() == 2)
			vForward = vForward * 8 + pLocal->GetEyePosition();
		else if (pLocal->szGetClass() == "Pyro" && pClass != 1178 && pWep->GetSlot() == 0)
			vForward = vForward * 17 + pLocal->GetEyePosition();
		else
			vForward = vForward * 9999 + pLocal->GetEyePosition();
	}
	else
		vForward = vForward * 9999 + pLocal->GetEyePosition();



	Ray_t ray;
	trace_t trace;
	CTraceFilter filt;

	filt.pSkip = pLocal;

	ray.Init(pLocal->GetEyePosition(), vForward);
	gInts.EngineTrace->TraceRay(ray, 0x46004003, &filt, &trace);

	if (!trace.m_pEnt)
		return;

	if (trace.hitgroup < 1)
		return;

	if (GAME_TF2)
		if (cloaked.value && trace.m_pEnt->GetCond() & TFCond_Cloaked)
			return;
	
	if (trace.m_pEnt->GetTeamNum() == pLocal->GetTeamNum())
		return;

	if (headonly.value && trace.hitbox != 0)
		return;

	if (!delay.value)
		pCommand->buttons |= IN_ATTACK;
	else
	{
		if (trace.hitbox > -1)
			didHit = true;

		if (iDelay >= delay.value && didHit)
		{
			iDelay = 0;
			pCommand->buttons |= IN_ATTACK;
		}
		iDelay++;
	}
}
