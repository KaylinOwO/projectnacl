#include "SDK.h"
#include "Client.h"
#include "Panels.h"
#include "Menu.h"
#include "CMat.h"
#include <Windows.h>
#pragma comment(lib, "Winmm.lib")

float SteamAPI_ServerNonLan = false;

CGlobalVariables gCvars;
CInterfaces gInts;

CreateInterface_t EngineFactory = NULL;
CreateInterface_t ClientFactory = NULL;
CreateInterface_t VGUIFactory = NULL;
CreateInterface_t VGUI2Factory = NULL;
CreateInterface_t CvarFactory = NULL;
CreateInterfaceFn SteamFactory = NULL;
CreateInterfaceFn MaterialSystemFactory = NULL;

ITFMatchGroupDescription* GetMatchGroupDescription(int& idx) //credits blackfire62
{
	typedef ITFMatchGroupDescription* (_cdecl* GetFn)(int&);
	static GetFn Get = (GetFn)(gSignatures.GetClientSignature("55 8B EC 8B 45 08 8B 00 83 F8 FF"));
	return Get(idx);
}

DWORD WINAPI dwMainThread( LPVOID lpArguments )
{
	if (gInts.Client == NULL)
	{
		float IsOnlineServer = false;
	    if (SteamAPI_ServerNonLan)
		    IsOnlineServer = true;
		else
		    IsOnlineServer = false;

		if (IsOnlineServer)
		    return 0;

		gOffsets.GetOffsets();

		VMTBaseManager* clientHook = new VMTBaseManager();
		VMTBaseManager* clientModeHook = new VMTBaseManager();
		VMTBaseManager* panelHook = new VMTBaseManager();
		VMTBaseManager* defRenderHook = new VMTBaseManager();

		ClientFactory = ( CreateInterfaceFn ) GetProcAddress( gSignatures.GetModuleHandleSafe( "client.dll" ), "CreateInterface" );
		EngineFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("engine.dll"), "CreateInterface");
		VGUIFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vguimatsurface.dll"), "CreateInterface");
		CvarFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vstdlib.dll"), "CreateInterface");
		SteamFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("SteamClient.dll"), "CreateInterface");
		MaterialSystemFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("MaterialSystem.dll"), "CreateInterface");

		gInts.Client = ( CHLClient* )ClientFactory( "VClient017", NULL);
		gInts.EntList = ( CEntList* ) ClientFactory( "VClientEntityList003", NULL );
		gInts.EntList2 = (CEntList2*)ClientFactory("VClientEntityList003", NULL);
		gInts.Engine = ( EngineClient* ) EngineFactory( "VEngineClient013", NULL );
		gInts.Surface = ( ISurface* ) VGUIFactory( "VGUI_Surface030", NULL );
		gInts.RenderView = (CRenderView*)EngineFactory("VEngineRenderView014", NULL);
		gInts.EngineTrace = ( IEngineTrace* ) EngineFactory( "EngineTraceClient003", NULL );
		gInts.ModelInfo = ( IVModelInfo* ) EngineFactory( "VModelInfoClient006", NULL );
		gInts.cvar = (ICvar*)CvarFactory("VEngineCvar004", NULL);
		if (GAME_TF2)
			gInts.MatSystem = (CMaterialSystem*)MaterialSystemFactory("VMaterialSystem081", NULL);
		else
			gInts.MatSystemOther = (CMaterialSystemOther*)MaterialSystemFactory("VMaterialSystem080", NULL);
		gInts.ModelRender = (CModelRender*)EngineFactory("VEngineModel016", NULL);
		gInts.EventManager = (IGameEventManager2*)EngineFactory("GAMEEVENTSMANAGER002", NULL);
		gInts.steamclient = (ISteamClient017*)SteamFactory("SteamClient017", NULL);

		HSteamPipe hNewPipe = gInts.steamclient->CreateSteamPipe();
		HSteamUser hNewUser = gInts.steamclient->ConnectToGlobalUser(hNewPipe);

		gInts.steamfriends = reinterpret_cast<ISteamFriends002 *>(gInts.steamclient->GetISteamFriends(hNewUser, hNewPipe, STEAMFRIENDS_INTERFACE_VERSION_002));
		gInts.steamuser = reinterpret_cast<ISteamUser017 *>(gInts.steamclient->GetISteamUser(hNewUser, hNewPipe, STEAMUSER_INTERFACE_VERSION_017));

		XASSERT(gInts.Client);
		XASSERT(gInts.EntList);
		XASSERT(gInts.EntList2);
		XASSERT(gInts.Engine);
		XASSERT(gInts.Surface);
		XASSERT(gInts.EngineTrace);
		XASSERT(gInts.ModelInfo);
		XASSERT(gInts.cvar);
		XASSERT(gInts.ModelRender);
		XASSERT(gInts.EventManager);
		XASSERT(gInts.steamclient);
		XASSERT(gInts.steamfriends);
		XASSERT(gInts.steamuser);
		gInts.Engine->ClientCmd_Unrestricted("toggleconsole");
		gInts.cvar->ConsoleColorPrintf(Color(253, 153, 32, 255), "NaCl Injected\n");
		gInts.cvar->ConsoleColorPrintf(Color(15, 150, 150, 255), "Current Name: %s\n", gInts.steamfriends->GetPersonaName());


		if( !gInts.Panels )
		{
			VGUI2Factory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vgui2.dll"), "CreateInterface");
			gInts.Panels = ( IPanel* ) VGUI2Factory( "VGUI_Panel009", NULL );
			XASSERT( gInts.Panels );

			if( gInts.Panels )
			{
				panelHook->Init(gInts.Panels);
				panelHook->HookMethod(&Hooked_PaintTraverse, gOffsets.iPaintTraverseOffset);
				panelHook->Rehook();
			}
		}

		if (GAME_TF2)
		{
			for (int i = 0; i < 12; i++) //Code by blackfire62 - Competitive Convar Bypass
			{
				ITFMatchGroupDescription* desc = GetMatchGroupDescription(i);

				if (!desc || desc->m_iID > 9) //ID's over 9 are invalid
					continue;

				if (desc->m_bForceCompetitiveSettings)
				{
					desc->m_bForceCompetitiveSettings = false;
				}
			}
		}

		gInts.globals = *reinterpret_cast<CGlobals **>(gSignatures.GetEngineSignature("A1 ? ? ? ? 8B 11 68") + 8);
		XASSERT(gInts.globals);

		DWORD dwClientModeAddress;
		if (GAME_TF2)
			dwClientModeAddress = gSignatures.GetClientSignature("8B 0D ? ? ? ? 8B 02 D9 05");
		else
			dwClientModeAddress = gSignatures.GetClientSignature("8B 0D ? ? ? ? 8B 01 5D FF 60 28 CC");
		
		XASSERT(dwClientModeAddress);
		gInts.ClientMode = **(ClientModeShared***)(dwClientModeAddress + 2);
		LOGDEBUG("g_pClientModeShared_ptr client.dll+0x%X", (DWORD)gInts.ClientMode - dwClientBase);

		clientHook->Init(gInts.Client);
		clientHook->HookMethod(&Hooked_KeyEvent, gOffsets.iKeyEventOffset);
		clientHook->Rehook();
		//clientHook->HookMethod(&FrameStageNotify, 35);
	//	clientHook->Rehook();

		clientModeHook->Init(gInts.ClientMode);
		clientModeHook->HookMethod(&Hooked_CreateMove, gOffsets.iCreateMoveOffset); //ClientMode create move is called inside of CHLClient::CreateMove, and thus no need for hooking WriteUserCmdDelta.
		clientModeHook->Rehook();

		clientHook->HookMethod(&FrameStageNotifyThink, 35);
		clientHook->Rehook();

		defRenderHook->Init(gInts.ModelRender);
		defRenderHook->HookMethod(&Hooked_DrawModelExecute, 19);
		defRenderHook->Rehook();

		DWORD CL_Move_ = gSignatures.GetEngineSignature("55 8B EC 83 EC ? 83 3D ? ? ? ? 02 0F 8C ? ? 00 00 E8 ? ? ? 00 84 C0");
		CL_Move_Detour.Init(CL_Move_, (void*)Hooked_CL_Move);

		HWND thisWindow;
		while (!(thisWindow = FindWindow("Valve001", NULL)))
			Sleep(500);
		gMenu.windowProc = (WNDPROC)SetWindowLongPtr(thisWindow, GWLP_WNDPROC, (LONG_PTR)&Hooked_WndProc);
	}
	return 0; //The thread has been completed, and we do not need to call anything once we're done. The call to Hooked_PaintTraverse is now our main thread.
}


BOOL APIENTRY DllMain(HMODULE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		Log::Init(hInstance);
		CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)dwMainThread, NULL, 0, NULL ); 
	}
	return true;
}
