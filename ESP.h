#pragma once
#include "SDK.h"
#include "Client.h"

class CESP
{
public:

	void Run(CBaseEntity* pLocal);
	void FrameStageNotify(ClientFrameStage_t Stage);

	Checkbox enabled = Checkbox("Enabled");
	Checkbox enemyonly = Checkbox("Enemy Only");
	Listbox box = Listbox("Box", { "Disabled", "2D - Normal", "2D - Corner" }, 0);
	Checkbox name = Checkbox("Name");
	Checkbox tfclass = Checkbox("Class");
	Listbox health = Listbox("Health", { "Disabled", "Value", "Bar", "Both" }, 0);
	Checkbox bones = Checkbox("Bones");
	Checkbox status = Checkbox("Status");
	Checkbox viewlines = Checkbox("View Lines");
	Listbox player_enabled = Listbox("Chams", { "Disabled", "Default Visibility", "Always Visible", "Only Behind Walls" }, 0);
	Listbox chamsmat = Listbox("Chams Material", { "Flat", "Shaded", "Shiny", "Wireframe" }, 0);
	ColorPicker player_mat = ColorPicker("Chams Color", SColor(200));
	Checkbox ignorecloaked = Checkbox("Ignore Cloaked");
	Checkbox ignoredisguised = Checkbox("Ignore Disguised");

	Checkbox nozoom = Checkbox("No Zoom");
	Checkbox noscope = Checkbox("No Scope");
	Checkbox nohands = Checkbox("No Hands");
	Checkbox thirdperson = Checkbox("Thirdperson");
	Checkbox visualize_backtrack = Checkbox("Visualize Backtrack");
	Listbox hands_enabled = Listbox("Hands", { "Disabled", "None", "Chams" });
	Listbox hands_chams = Listbox("Hand Chams Material", { "Flat", "Shaded", "Shiny", "Wireframe" }, 0);
	ColorPicker hand_mat_color = ColorPicker("Hand Chams Color", SColor(200));

	Checkbox world_enabled = Checkbox("Enabled");
	ColorPicker world_clr = ColorPicker("World Color", SColor(200));
	ColorPicker sky_clr = ColorPicker("Sky Color", SColor(200));


	void DrawModelExecute(const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld, DrawModelExecuteFn Original);

private:

	void Player_ESP(CBaseEntity* pLocal, CBaseEntity* pEntity);
	//Other funcs

	void DrawBone(CBaseEntity* pEntity, int* iBones, int count, Color clrCol);

	void DrawFace(CBaseEntity* pEntity, Color clr);
};

extern CESP gESP; 