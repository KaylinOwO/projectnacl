#pragma once
#include "SDK.h"

namespace Global {
	extern CBaseEntity*	Local;
	extern CUserCmd*	Command;
	extern CUserCmd		OGCommand;
}

class CMisc
{
public:

	void Run(CBaseEntity* pLocal, CUserCmd* pCommand);

	Checkbox bhop = Checkbox("Bunnyhop");
	Checkbox astrafer = Checkbox("Autostrafer");
	Checkbox nspam = Checkbox("Noisemaker Spam");
	Checkbox tauntslide = Checkbox("Taunt Slide");
	Checkbox speedcrouch = Checkbox("Speed Crouch");
	Checkbox nopush = Checkbox("No Push");
	Checkbox cscreenshots = Checkbox("Clean Screenshots");
	Checkbox purebypass = Checkbox("Pure Bypass");

	Checkbox flag = Checkbox("Fakelag");
	Slider flagamount = Slider("Fakelag Amount", 0, 0, 15, 1);

private:

	//Other funcs

	void NoisemakerSpam(PVOID kv);
	void AntiAim();
	void AntiAim2();
};

extern CMisc gMisc;