#include "SDK.h"
#include "Client.h"
#include "Util.h"
#include "Aimbot.h"
#include "Triggerbot.h"
#include "HvH.h"
#include "ESP.h"
#include "Misc.h"
#include "Backtrack.h"
#include "AutoAirblast.h"
#include "SFreezing.h"
#include "Sticky.h"
#include "CMat.h"
#include "CDrawManager.h"
#include <intrin.h>

CGlobalzz g;

Vector qLASTTICK;

bool Key(INT vKey)
{
	if (GetAsyncKeyState(vKey) & 1)
		return true;

	return false;
}
int WarpCharge;

//============================================================================================
bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand)
{
	VMTManager& hook = VMTManager::GetHook(ClientMode); //Get a pointer to the instance of your VMTManager with the function GetHook.
	bool bReturn = hook.GetMethod<bool(__thiscall*)(PVOID, float, CUserCmd*)>(gOffsets.iCreateMoveOffset)(ClientMode, input_sample_frametime, pCommand); //Call the original.
	try
	{
		if (!pCommand->command_number)
			return false;

		CBaseEntity* pLocal = GetBaseEntity(me);

		if (!pLocal)
		{
			WarpCharge = 0;
			return bReturn;
		}

		g.local = reinterpret_cast<CBaseEntity*>(gInts.EntList->GetClientEntity(gInts.Engine->GetLocalPlayer()));
		g.cmd = pCommand; g.original_cmd = *pCommand;

		if (!g.local->IsAlive() || g.local->IsDormant() || !gInts.Engine->IsInGame() || !gInts.Engine->IsConnected() || gInts.Engine->IsDrawingLoadingImage()) WarpCharge = 0;

		auto base = reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) - sizeof(uintptr_t);
		bool& bSendPacket = *(***reinterpret_cast<bool****>(base) - 1);

		if (pLocal->IsAlive())
		{
			if (gMisc.flag.value)
			{
				auto netchan = gInts.Engine->GetNetChannelInfo();
				if (netchan->m_nChokedPackets < (int)gMisc.flagamount.value)
					bSendPacket = false;
				else
					bSendPacket = true;
			}
		}

		g.isfiring = (pCommand->buttons & IN_ATTACK);

		gMisc.Run(pLocal, pCommand);
		gAim.Run(pLocal, pCommand);
		gHvH.Run(pLocal, pCommand, bSendPacket);
		gTrigger.Run(pLocal, pCommand);
		gSequenceFreezing.Run(pLocal, pCommand);
		if (GAME_TF2)
		{
			gBlast.Run(pLocal, pCommand);
			gSticky.Run(pLocal, pCommand);
		}
		backtrack::do_backtrack();
	}
	catch (...)
	{
		Log::Fatal("Failed Hooked_CreateMove");
	}
	qLASTTICK = pCommand->viewangles;
	return false/*bReturn*/;
}

void __fastcall FrameStageNotifyThink(PVOID CHLClient, void *_this, ClientFrameStage_t Stage)
{
	gESP.FrameStageNotify(Stage);

	if (Stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		if (GAME_TF2)
		{
			for (auto i = 1; i <= gInts.Engine->GetMaxClients(); i++)
			{
				CBaseEntity *entity = nullptr;
				player_info_t temp;

				if (!(entity = gInts.EntList->GetClientEntity(i)))
					continue;

				if (entity->IsDormant())
					continue;

				if (!gInts.Engine->GetPlayerInfo(i, &temp))
					continue;

				if (!entity->GetLifeState() == LIFE_ALIVE)
					continue;

				Vector vX = entity->GetAngles();
				Vector vY = entity->GetAnglesHTC();
				auto *WritePitch = reinterpret_cast<float*>(reinterpret_cast<DWORD>(entity) + gNetVars.get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]"));
				auto *WriteYaw = reinterpret_cast<float*>(reinterpret_cast<DWORD>(entity) + gNetVars.get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[1]"));

				bool resolver = false;

				if (Util->IsKeyPressedMisc(gCvars.aimbot_resolver_key))
				{
					resolver = !resolver;
				}
				if (gCvars.aimbot_resolver && resolver)
				{
					if (vX.x == -89.0f)
					{
						*WritePitch = 90.0f;
					}
					if (vX.x == 89.0f)
					{
						*WritePitch = -90.0f;
					}
				}
			}
		}
   }

	if (gInts.Engine->IsInGame() && Stage == FRAME_RENDER_START)
	{
			CBaseEntity *oEntity = gInts.EntList->GetClientEntity(gInts.Engine->GetLocalPlayer());

			int *Fov;
			int OldFov;

			int m_flFOVRate = 0xE5C;// Broken: nv.get_offset("DT_BasePlayer", "localdata", "m_flFOVRate");
			int &fovPtr = *(int*)(oEntity + gNetVars.get_offset("DT_BasePlayer", "m_iFOV")), defaultFov = *(int*)(oEntity + gNetVars.get_offset("DT_BasePlayer", "m_iDefaultFOV"));
			if (gESP.nozoom.value) //Thanks cademtz/Hold on! for this code, works amazingly aside from the inivisible sniper rifle xd
			{
				fovPtr = defaultFov;
				*(float*)(oEntity + m_flFOVRate) = 0;
			}

			if (gESP.thirdperson.value) //shows antiaims just fine
			{
				CBaseEntity *oEntity = gInts.EntList->GetClientEntity(gInts.Engine->GetLocalPlayer());

				auto *m_angEyeAngles = reinterpret_cast<float*>(reinterpret_cast<DWORD>(oEntity) + gNetVars.get_offset("DT_BasePlayer", "pl", "deadflag") + 8);

				auto *HTC = reinterpret_cast<float*>(reinterpret_cast<DWORD>(oEntity) + gNetVars.get_offset("DT_BasePlayer", "pl", "deadflag") + 4);

				*HTC = qLASTTICK.x;
				*m_angEyeAngles = qLASTTICK.y;

				ConVar* sv_cheats = gInts.cvar->FindVar("sv_cheats");
				if (sv_cheats->GetInt() == 0) sv_cheats->SetValue(1);
				ConVar* pThirdCamYaw = gInts.cvar->FindVar("cam_idealyaw");
				gInts.Engine->ClientCmd_Unrestricted("thirdperson");

				pThirdCamYaw->SetValue(0);
			}
			else if (!gESP.thirdperson.value)
			{
				gInts.Engine->ClientCmd_Unrestricted("firstperson");
			}
	}



	VMTManager &HTCCNKBRKYLC = VMTManager::GetHook(CHLClient);
	return HTCCNKBRKYLC.GetMethod<void(__fastcall *)(PVOID, void *, ClientFrameStage_t)>(35)(CHLClient, _this, Stage);
}

void __stdcall Hooked_DrawModelExecute(const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	VMTManager& hook = VMTManager::GetHook(gInts.ModelRender);
	const char* pszModelName = gInts.ModelInfo->GetModelName((DWORD*)pInfo.pModel);
	auto CallOriginal = hook.GetMethod<DrawModelExecuteFn>(19);
	try
	{
		if (pInfo.pModel)
			gESP.DrawModelExecute(state, pInfo, pCustomBoneToWorld, CallOriginal);

		if (gESP.hands_enabled.value == 1 && strstr(pszModelName, "arms"))
			return;
	}
	catch (...)
	{
		MessageBox(NULL, "Failed Hooked_DrawModelExecute()", "Error", MB_OK);
		return CallOriginal(gInts.ModelRender, state, pInfo, pCustomBoneToWorld);
	}
	CallOriginal(gInts.ModelRender, state, pInfo, pCustomBoneToWorld);
	gMat.ResetMaterial();
}
//============================================================================================

typedef void(__thiscall* FrameStageNotify)(void *, ClientFrameStage_t);


int __fastcall Hooked_KeyEvent(PVOID CHLClient, int edx, int eventcode, int keynum, const char *currentBinding)
{
	if (eventcode == 1)
	{
	/*	if (keynum == 72) //insert
		{
			gCheatMenu.bMenuActive = !gCheatMenu.bMenuActive;
		}

		if (gCheatMenu.bMenuActive)
		{
			if (keynum == 88 || keynum == 112) // Up
			{

				if (gCheatMenu.iMenuIndex > 0) gCheatMenu.iMenuIndex--;
				else gCheatMenu.iMenuIndex = gCheatMenu.iMenuItems - 1;
				return 0;

			}
			else if (keynum == 90 || keynum == 113) // Down
			{
				if (gCheatMenu.iMenuIndex < gCheatMenu.iMenuItems - 1) gCheatMenu.iMenuIndex++;
				else gCheatMenu.iMenuIndex = 0;
				return 0;

			}
			else if (keynum == 89 || keynum == 107) // Left
			{
				if (gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value)
				{
					gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] -= gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flStep;
					if (gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] < gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flMin)
						gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] = gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flMax;
				}
				return 0;

			}
			else if (keynum == 91 || keynum == 108) // Right
			{
				if (gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value)
				{
					gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] += gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flStep;
					if (gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] > gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flMax)
						gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] = gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flMin;
				}
				return 0;

			}



		}*/
	}

	VMTManager &hook = VMTManager::GetHook(CHLClient); // Get a pointer to the instance of your VMTManager with the function GetHook.
	return hook.GetMethod<int(__thiscall *)(PVOID, int, int, const char *)>(gOffsets.iKeyEventOffset)(CHLClient, eventcode, keynum, currentBinding); // Call the original.
}
void FastStop()
{
	// Get velocity
	Vector vel = g.local->GetVelocity();

	static auto sv_friction = gInts.cvar->FindVar("sv_friction");
	static auto sv_stopspeed = gInts.cvar->FindVar("sv_stopspeed");

	auto speed = vel.Length2D();
	auto friction = sv_friction->GetFloat();
	auto control = (speed < sv_stopspeed->GetFloat()) ? sv_stopspeed->GetFloat() : speed;
	auto drop = control * friction * gInts.globals->interval_per_tick;

	if (speed > drop - 1.0f)
	{
		Vector velocity = vel;
		Vector direction;
		VectorAngles(vel, direction);
		float speed = velocity.Length();

		direction.y = g.cmd->viewangles.y - direction.y;

		Vector forward;
		VectorAngles(direction, forward);
		Vector negated_direction = forward * -speed;

		g.cmd->forwardmove = negated_direction.x;
		g.cmd->sidemove = negated_direction.y;
	}
	else
	{
		g.cmd->forwardmove = g.cmd->sidemove = 0.0f;
	}
}

DetourHook CL_Move_Detour;


void Hooked_CL_Move(float accumulated_extra_samples, bool bFinalTick) {
	auto Original = (CL_Move_t)CL_Move_Detour.GetOriginalFunc();

	if ((gMisc.doubletap.value && (GetAsyncKeyState(VK_LBUTTON) || Util->IsKeyPressedMisc(gAim.key.value) && gAim.Autoshoot.value)) || (gMisc.warp.value && Util->IsKeyPressedMisc(gMisc.warp_key.value))) {
		for (int i = 0; i < WarpCharge; i++)
		{
			Original(accumulated_extra_samples, bFinalTick);
			WarpCharge--;
		}
	}

	if (Util->IsKeyPressedMisc(gMisc.warp_charge_key.value) && WarpCharge < gMisc.warp_value.value)
		WarpCharge++;
	else
		Original(accumulated_extra_samples, bFinalTick);

	CL_Move_Detour.RestorePatch();
}
