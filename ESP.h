#pragma once
#include "SDK.h"

class CESP
{
public:

	void Run(CBaseEntity* pLocal);

	Checkbox enabled = Checkbox("Enabled");
	Checkbox enemyonly = Checkbox("Enemy Only");
	Listbox box = Listbox("Box", { "Disabled", "2D - Normal", "2D - Corner" }, 0);
	Checkbox name = Checkbox("Name");
	Checkbox tfclass = Checkbox("Class");
	Listbox health = Listbox("Health", { "Disabled", "Value", "Bar", "Both" }, 0);
	Checkbox bones = Checkbox("Bones");
	Checkbox status = Checkbox("Status");
	Checkbox viewlines = Checkbox("View Lines");
	Checkbox ignorecloaked = Checkbox("Ignore Cloaked");
	Checkbox ignoredisguised = Checkbox("Ignore Disguised");

	Checkbox nozoom = Checkbox("No Zoom");
	Checkbox noscope = Checkbox("No Scope");
	Checkbox thirdperson = Checkbox("Thirdperson");

private:

	void Player_ESP(CBaseEntity* pLocal, CBaseEntity* pEntity);
	//Other funcs

	void DrawBone(CBaseEntity* pEntity, int* iBones, int count, Color clrCol);

	void DrawFace(CBaseEntity* pEntity, Color clr);
};

extern CESP gESP; 