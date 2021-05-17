#include "Misc.h"
#include "Util.h"
#include "Aimbot.h"
#include "Client.h"
#include <chrono>
#include <sstream>

CMisc gMisc;

namespace Global {
	CBaseEntity*	Global::Local;
	CUserCmd*		Global::Command;
	CUserCmd		Global::OGCommand;
}

std::string repeat(int n, const char* str)
{
	std::ostringstream os;
	for (int i = 0; i < n; i++)
		os << str;
	return os.str();
}

bool CMisc::CanShoot()
{
	CBaseCombatWeapon *pWeapon = g.local->GetActiveWeaponOther();
	if (!pWeapon)
		return false;
	if (!g.local->IsAlive())
		return false;

	return (pWeapon->GetNextPrimaryAttack() < g.local->TickBase() * gInts.globals->interval_per_tick);
}

void CMisc::AutoPistol(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	if (gInts.Engine->GetAppId() != 440) //super advanced auto pistol code :O
	{
		static bool check = false;
		CBaseCombatWeapon *pWeapon = pLocal->GetActiveWeaponOther();
		if (CanShoot())
		{
			if (pCommand->buttons & IN_ATTACK)
			{
				check = true;
				{
					static bool flipFlop = true;
					if (flipFlop) { pCommand->buttons |= IN_ATTACK; }
					else { pCommand->buttons &= (~IN_ATTACK); }
					flipFlop = !flipFlop;
				}
			}
			else
			{
				check = false;
			}
		}
	}
}

void CMisc::Run(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	if (serverlag.value && Util->IsKeyPressedMisc(serverlag_key.value))
		for (int i = 0; i < (int)serverlag_value.value; i++)
			gInts.Engine->ServerCmd("use", false);

	if (airstuck.value && Util->IsKeyPressedMisc(airstuck_key.value) && !(pCommand->buttons & IN_ATTACK))
	{
		pCommand->tick_count = INT_MAX;
		pCommand->viewangles.z = FLT_MAX; //This is a ghetto way of fixing the camera jittering, idk if this is how most people do it, I just stumbled upon it by accident.
	}

	if (!pLocal->IsAlive())
		return;

	if (GAME_CSS && gAim.antirecoil.value)
	{
		Vector AimPunch = pLocal->GetVecPunchAngle();
		pCommand->viewangles -= (AimPunch * 2.f);
	}

	if (gAim.autopistol.value)
		AutoPistol(pLocal, pCommand);

	if (GAME_TF2 && tauntslide.value) //This is for some reason broken, I have an idea why just haven't got to trying to fix it.
	{
		if ((pLocal->GetCond() & tf_cond::TFCond_Taunting))
		{

			float forward = 0.0f, side = 0.0f;

			if (pCommand->buttons & IN_FORWARD) { forward += 450; }
			if (pCommand->buttons  & IN_BACK) { forward -= 450; }
			if (pCommand->buttons  & IN_MOVELEFT) { side -= 450; }
			if (pCommand->buttons  & IN_MOVERIGHT) { side += 450; }

			pCommand->forwardmove = forward;
			pCommand->sidemove = side;

			Vector viewangles;
			gInts.Engine->GetViewAngles(viewangles);

			pCommand->viewangles.y = viewangles[1];
			Global::OGCommand.viewangles.y = viewangles[1];
		}
	}

	if (GAME_TF2 && speedcrouch.value && !(pCommand->buttons & IN_ATTACK) && (pCommand->buttons & IN_DUCK)) // who changed my comment >:(
	{
		Vector vLocalAngles = pCommand->viewangles;
		float speed = pCommand->forwardmove;
		if (fabs(speed) > 0.0f) {
			pCommand->forwardmove = -speed;
			pCommand->sidemove = 0.0f;
			pCommand->viewangles.y = vLocalAngles.y;
			pCommand->viewangles.y -= 180.0f;
			if (pCommand->viewangles.y < -180.0f) pCommand->viewangles.y += 360.0f;
			pCommand->viewangles.z = 90.0f;
		}
	}

	if (!(pLocal->GetFlags() & FL_ONGROUND) && pCommand->buttons & IN_JUMP)
	{
		//Autostrafe	
		if (astrafer.value)
			if (pCommand->mousedx > 1 || pCommand->mousedx < -1)  //> 1 < -1 so we have some wiggle room
				pCommand->sidemove = pCommand->mousedx > 1 ? 450.f : -450.f;

		//Bunnyhop
		if (bhop.value)
			pCommand->buttons &= ~IN_JUMP;
	}

	if (nspam.value)
	{
		PVOID kv = Util->InitKeyValue();
		if (kv != NULL)
		{
			NoisemakerSpam(kv);
			gInts.Engine->ServerCmdKeyValues(kv);
		}
	}

	if (GAME_TF2 && nopush.value)
	{
		ConVar* tf_avoidteammates_pushaway = gInts.cvar->FindVar("tf_avoidteammates_pushaway");
		if (tf_avoidteammates_pushaway->GetInt() == 1)
			tf_avoidteammates_pushaway->SetValue(0);
	}

}
		  //Could be much simpler, but I don't want keyvals class
void CMisc::NoisemakerSpam(PVOID kv) //Credits gir https://www.unknowncheats.me/forum/team-fortress-2-a/141108-infinite-noisemakers.html
{
	char chCommand[30] = "use_action_slot_item_server";
	typedef int(__cdecl* HashFunc_t)(const char*, bool);
	static DWORD dwHashFunctionLocation = gSignatures.GetClientSignature("FF 15 ? ? ? ? 83 C4 08 89 06 8B C6");
	static HashFunc_t s_pfGetSymbolForString = (HashFunc_t)**(PDWORD*)(dwHashFunctionLocation + 0x2);
	*(PDWORD)((DWORD)kv + 0x4) = 0;
	*(PDWORD)((DWORD)kv + 0x8) = 0;
	*(PDWORD)((DWORD)kv + 0xC) = 0;
	*(PDWORD)((DWORD)kv + 0x10) = /*0x10000*/0xDEADBEEF;
	*(PDWORD)((DWORD)kv + 0x14) = 0;
	*(PDWORD)((DWORD)kv + 0x18) = 0; //Extra one the game isn't doing, but if you don't zero this out, the game crashes.
	*(PDWORD)((DWORD)kv + 0x1C) = 0;
	*(PDWORD)((DWORD)kv + 0) = s_pfGetSymbolForString(chCommand, 1);
}

//pAAs
void CMisc::AntiAim()
{

}
