#include "Triggerbot.h"
#include "Util.h"

CTriggerbot gTrigger;

void CTriggerbot::Run(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	CBaseCombatWeapon* pWep = pLocal->GetActiveWeapon();

	if (pLocal->GetLifeState() != LIFE_ALIVE)
		return;

	if (backstab.value)
		if (pLocal->szGetClass() == "Spy" && pWep->GetSlot() == 2 && pWep->CanBackStab())
			pCommand->buttons |= IN_ATTACK;

	if (!enabled.value)
		return;

	if (!Util->IsKeyPressedMisc(key.value))
		return;

	Vector vAim;
	gInts.Engine->GetViewAngles(vAim); //We use getviewangles so that this can work with anti-aim when it gets added

	Vector vForward;
	AngleVectors(vAim, &vForward);
	//How far we wanna look forward, lower this value if needed


	auto pClass = pWep->GetItemDefinitionIndex();

	if (pWep->GetSlot() == 2)
		vForward = vForward * 8 + pLocal->GetEyePosition();
	else if (pLocal->szGetClass() == "Pyro" && pClass != 1178 && pWep->GetSlot() == 0)
		vForward = vForward * 17 + pLocal->GetEyePosition();
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

	if (cloaked.value && trace.m_pEnt->GetCond() & TFCond_Cloaked)
		return;

	if (headonly.value && trace.hitbox != 0)
		return;

	pCommand->buttons |= IN_ATTACK;
}