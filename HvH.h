#pragma once
#include "SDK.h"

class CHvH
{
public:

	void Run(CBaseEntity* pLocal, CUserCmd* pCommand, bool bSendPacket);

	Checkbox resolver = Checkbox("Resolver");
	Checkbox antiaim = Checkbox("Anti-Aim");
	Listbox pitch = Listbox("Pitch", { "Disabled", "Fake Up", "Up", "Fake Down", "Down" }, 0);
	Listbox yaw = Listbox("Yaw", { "Disabled", "Right", "Left", "Back", "Emotion", "Random", "Fake Sideways Right", "Fake Sideways Left", "Fake Right", "Half Back Left", "Half Back Right", "Fake Left", "Fake Manual" }, 0);
};

extern CHvH gHvH;