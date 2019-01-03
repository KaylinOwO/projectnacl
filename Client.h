#pragma once

#include "SDK.h"

bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand);
int __fastcall Hooked_KeyEvent(PVOID CHLClient, int edx, int eventcode, int keynum, const char *currentBinding);
void __fastcall FrameStageNotifyThink(PVOID CHLClient, void *_this, ClientFrameStage_t Stage);
//void __fastcall FrameStageNotify(PVOID CHLClient, void *_this, ClientFrameStage_t Stage);