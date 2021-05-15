#include "SDK.h"
#include "Panels.h"
#include "CDrawManager.h"
#include "Util.h"
#include "ESP.h"
#include "Menu.h"
#include "CDraw.h"
#include "Styles.h"
#include "Misc.h"
#include "CMat.h"

CScreenSize gScreenSize;
//===================================================================================
void __fastcall Hooked_PaintTraverse( PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	try
	{
		CBaseEntity* pLocal = gInts.EntList->GetClientEntity(me);
		auto crosshair = gInts.cvar->FindVar("crosshair");
		if (!strcmp("HudScope", gInts.Panels->GetName(vguiPanel)) && gESP.noscope.value)
		{
				if (pLocal->GetCond() & tf_cond::TFCond_Zoomed)
				{
					crosshair->SetValue(0);
				}
				else
				{
					crosshair->SetValue(1);
				}

			return;
		}
		VMTManager& hook = VMTManager::GetHook(pPanels); 
		hook.GetMethod<void(__thiscall*)(PVOID, unsigned int, bool, bool)>(gOffsets.iPaintTraverseOffset)(pPanels, vguiPanel, forceRepaint, allowForce); //Call the original.
		static unsigned int vguiFocusOverlayPanel;
		WindowStyle* style = new DefaultStyle();

		if (vguiFocusOverlayPanel == NULL)
		{											//FocusOverlayPanel
			const char* szName = gInts.Panels->GetName(vguiPanel);
			if( szName[0] == 'F' && szName[5] == 'O' &&  szName[12] == 'P' )
			{
				vguiFocusOverlayPanel = vguiPanel;
				Intro();
			}
		}

		if (vguiFocusOverlayPanel == vguiPanel )
		{
			if (gMisc.cscreenshots.value && gInts.Engine->IsTakingScreenshot() || gMisc.cscreenshots.value && GetAsyncKeyState(VK_F12) || gMisc.cscreenshots.value && GetAsyncKeyState(VK_SNAPSHOT))
			    return;

			if (!gInts.Engine->IsInGame() && !gMenu.enabled)
				style->WaterMark("Project NaCl", 20, 20);

			gInts.Panels->SetTopmostPopup(vguiFocusOverlayPanel, true);

			CScreenSize newSize;
			gInts.Engine->GetScreenSize(newSize.iScreenWidth, newSize.iScreenWidth);
			if (newSize.iScreenWidth != gScreenSize.iScreenWidth || newSize.iScreenHeight != gScreenSize.iScreenHeight)
			{
				gDraw.Reload();
				gDrawManager.Reload();
			}

	

			if (GAME_TF2)
			{
				if (gMisc.purebypass.value || gESP.player_enabled.value)
				{
					bool check = false;
					if (!gInts.Engine->IsInGame() && check == false)
					{
						void** pure_addr = nullptr;
						if (!pure_addr)
						{
							pure_addr = *reinterpret_cast<void***>(gSignatures.GetEngineSignature("A1 ? ? ? ? 56 33 F6 85 C0") + 1);
						} *
							pure_addr = (void*)0;

						check = true;
					}
				}
			}

			int iWidth, iHeight; //Resolution fix, so this can work in Fullscreen
			gInts.Engine->GetScreenSize(iWidth, iHeight);
			if (gScreenSize.iScreenWidth != iWidth || gScreenSize.iScreenHeight != iHeight)
				gInts.Engine->GetScreenSize(gScreenSize.iScreenWidth, gScreenSize.iScreenHeight);

			if (pLocal)
				gESP.Run(pLocal);

			gMenu.GetInput();
			gMenu.Draw();
			gInts.Panels->SetMouseInputEnabled(vguiPanel, gMenu.enabled);
			gMenu.EndInput();

			if (pLocal && GAME_TF2)
			{
				if (gInts.Engine->IsInGame() && gInts.Engine->IsConnected())
				{
					if (!gESP.noscope.value)
						return;

					if (pLocal->GetLifeState() == LIFE_ALIVE && pLocal->GetCond() & tf_cond::TFCond_Zoomed)
					{
						int width, height;
						gInts.Engine->GetScreenSize(width, height);
						gDrawManager.DrawLine(width / 2, 0, width / 2, height, Color(0, 0, 0, 255));
						gDrawManager.DrawLine(0, height / 2, width, height / 2, Color(0, 0, 0, 255));
					}

				}
			}
		}
	}
	catch(...)
	{
		Log::Fatal("Failed PaintTraverse");
	}
}
//===================================================================================
void Intro( void )
{
	try
	{
		gDrawManager.Initialize(); //Initalize the drawing class.
		gDraw.Init();
		if (GAME_TF2)
			gMat.Initialize();
		else
			gMat.InititalizeOther();
		InitTextures();
		gNetVars.Initialize();
		gMenu.CreateGUI();
	}
	catch(...)
	{
		Log::Fatal("Failed Intro");
	}
}
