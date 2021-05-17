#pragma once

#include "SDK.h"
#include "Detours.h"

class CGlobalzz {
public:
	CBaseEntity* local{};
	CUserCmd* cmd{};
	CUserCmd original_cmd{};
	bool isfiring, silenttime;
	bool* sendpacket;
};

struct DrawModelState_t;
struct ModelRenderInfo_t;
typedef float matrix3x4[3][4];

typedef void(__thiscall *DrawModelExecuteFn)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*);

typedef void(*CL_Move_t)(float accumulated_extra_samples, bool bFinalTick);
extern DetourHook CL_Move_Detour;

bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand);
int __fastcall Hooked_KeyEvent(PVOID CHLClient, int edx, int eventcode, int keynum, const char *currentBinding);
void __fastcall FrameStageNotifyThink(PVOID CHLClient, void *_this, ClientFrameStage_t Stage);
void __stdcall Hooked_DrawModelExecute(const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld = NULL);
void Hooked_CL_Move(float accumulated_extra_samples, bool bFinalTick);
//void __fastcall FrameStageNotify(PVOID CHLClient, void *_this, ClientFrameStage_t Stage);

extern CGlobalzz g;