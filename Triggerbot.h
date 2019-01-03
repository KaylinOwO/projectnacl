#pragma once
#include "SDK.h"

class CTriggerbot
{
public:

	void Run(CBaseEntity* pLocal, CUserCmd* pCommand);

	Checkbox enabled = Checkbox("Enabled");
	Listbox key = Listbox("Key", { "None", "Mouse 3", "Mouse 4", "Mouse 5", "Shift", "Alt", "F" }, 1);
	Checkbox headonly = Checkbox("Head Only");
	Checkbox cloaked = Checkbox("Ignore Cloaked");

	Checkbox backstab = Checkbox("Auto Backstab");
	Checkbox airblast = Checkbox("Auto Airblast");
	Checkbox airblastrage = Checkbox("Rage Mode");
	//Checkbox waitforcharge = Checkbox("Wait For Charge");
};

extern CTriggerbot gTrigger;