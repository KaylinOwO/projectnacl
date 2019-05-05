#pragma once
#include "SDK.h"

class CTriggerbot
{
public:

	void Run(CBaseEntity* pLocal, CUserCmd* pCommand);

	Checkbox enabled = Checkbox("Enabled");
	Listbox key = Listbox("Key", { "None", "Mouse 3", "Mouse 4", "Mouse 5", "Shift", "Alt", "F", "E" }, 0);
	Checkbox headonly = Checkbox("Head Only");
	Slider delay = Slider("Delay", 0, 0, 20, 1);
	Checkbox cloaked = Checkbox("Ignore Cloaked");

	Checkbox backstab = Checkbox("Auto Backstab");
	Checkbox stickydetonate = Checkbox("Auto Sticky Detonate");
	Checkbox airblast = Checkbox("Auto Airblast");
	Checkbox airblastrage = Checkbox("Aim @ Projectiles");
	//Checkbox waitforcharge = Checkbox("Wait For Charge");
};

extern CTriggerbot gTrigger;