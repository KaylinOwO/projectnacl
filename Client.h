#pragma once

#include "SDK.h"

struct DrawModelState_t;
struct ModelRenderInfo_t;
typedef float matrix3x4[3][4];

typedef void(__thiscall *DrawModelExecuteFn)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*);

bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand);
int __fastcall Hooked_KeyEvent(PVOID CHLClient, int edx, int eventcode, int keynum, const char *currentBinding);
void __fastcall FrameStageNotifyThink(PVOID CHLClient, void *_this, ClientFrameStage_t Stage);
void __stdcall Hooked_DrawModelExecute(const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld = NULL);
//void __fastcall FrameStageNotify(PVOID CHLClient, void *_this, ClientFrameStage_t Stage);