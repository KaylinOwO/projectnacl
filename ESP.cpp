#include "ESP.h"
#include "CDrawManager.h"
#include "Util.h"

#include "Aimbot.h"
#include "Misc.h"
#include "Backtrack.h"

#include "CEtags.h"

CESP gESP;

void CESP::Run(CBaseEntity* pLocal)
{
	Vector vOrigin, vIdk;
	if (!enabled.value)
		return;
	for (int i = 1; i <= gInts.Engine->GetMaxClients(); i++)
	{
		if (!thirdperson.value && i == me)
			continue;

		CBaseEntity* pEntity = GetBaseEntity(i);

		if (!pEntity)
			continue;

		if (pEntity->IsDormant())
			continue;

		if (pEntity->GetLifeState() != LIFE_ALIVE)
			continue;

		if (enemyonly.value && pEntity->GetTeamNum() == pLocal->GetTeamNum())
			continue;

		if (ignorecloaked.value && pEntity->GetCond() & TFCond_Cloaked)
			continue;

		if (ignoredisguised.value && pEntity->GetCond() & TFCond_Disguised)
			continue;

		Player_ESP(pLocal, pEntity);
	}
}

Color gaylol(CBaseEntity* pPlayer)
{
	switch (pPlayer->GetTeamNum())
	{
	case 2: //RED
		return Color(255, 165, 0, 255);
	case 3: //BLU
		return Color(255, 165, 0, 255);
	default:
		return Color(255, 165, 0, 255);
	}

	return Color(0, 0, 0, 0);
}

Color GetHealthColor(CBaseEntity* pPlayer)
{
		if (pPlayer->GetHealth() > 100)
			return Color(0, 255, 0, 255);
		if (pPlayer->GetHealth() > 65)
			return Color(55, 255, 0, 255);
		if (pPlayer->GetHealth() > 35)
			return Color(255, 20, 20, 255);
	return Color(255, 20, 20, 255);
}

void CESP::Player_ESP(CBaseEntity* pLocal, CBaseEntity* pEntity)
{
	Color HealthColor = GetHealthColor(pEntity);
	player_info_t pInfo;
	if (!gInts.Engine->GetPlayerInfo(pEntity->GetIndex(), &pInfo))
		return;

	const matrix3x4& vMatrix = pEntity->GetRgflCoordinateFrame();

	Vector vMin = pEntity->GetCollideableMins();
	Vector vMax = pEntity->GetCollideableMaxs();

	Vector vPointList[] = {
		Vector(vMin.x, vMin.y, vMin.z),
		Vector(vMin.x, vMax.y, vMin.z),
		Vector(vMax.x, vMax.y, vMin.z),
		Vector(vMax.x, vMin.y, vMin.z),
		Vector(vMax.x, vMax.y, vMax.z),
		Vector(vMin.x, vMax.y, vMax.z),
		Vector(vMin.x, vMin.y, vMax.z),
		Vector(vMax.x, vMin.y, vMax.z)
	};
	Vector a;
	Vector vTransformed[8];

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 3; j++)
			vTransformed[i][j] = vPointList[i].Dot((Vector&)vMatrix[j]) + vMatrix[j][3];

	Vector flb, brt, blb, frt, frb, brb, blt, flt;

	if (!gDrawManager.WorldToScreen(vTransformed[3], flb) ||
		!gDrawManager.WorldToScreen(vTransformed[0], blb) ||
		!gDrawManager.WorldToScreen(vTransformed[2], frb) ||
		!gDrawManager.WorldToScreen(vTransformed[6], blt) ||
		!gDrawManager.WorldToScreen(vTransformed[5], brt) ||
		!gDrawManager.WorldToScreen(vTransformed[4], frt) ||
		!gDrawManager.WorldToScreen(vTransformed[1], brb) ||
		!gDrawManager.WorldToScreen(vTransformed[7], flt))
		return; 

	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	float left = flb.x;
	float top = flb.y;
	float right = flb.x;
	float bottom = flb.y;

	for (int i = 0; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (top < arr[i].y)
			top = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (bottom > arr[i].y)
			bottom = arr[i].y;
	}

	Color clrTeam = Color(255, 255, 255, 255);
	if (pEntity->GetTeamNum() == 2)
		clrTeam = Color(255, 20, 20, 255); //red
	else if (pEntity->GetTeamNum() == 3)
		clrTeam = Color(0, 153, 255, 255);//blue


	float x = left;
	float y = bottom;
	float w = right - left;
	float h = top - bottom;

	x += ((right - left) / 8); //pseudo fix for those THICC boxes
	w -= ((right - left) / 8) * 2;

	Color Orange = gaylol(pEntity);
	Color clrPlayerCol = gDrawManager.GetPlayerColor(pEntity);
	Color clrBoneCol = Color::White();
	int iY = 0;
	//iHp is only for health bar
	int iHp = pEntity->GetHealth(), iMaxHp = pEntity->GetMaxHealth();
	if (iHp > iMaxHp)
		iHp = iMaxHp;



	if (box.value == 1)
	{
		gDrawManager.OutlineRect(x, y, w, h, clrPlayerCol);
		gDrawManager.OutlineRect(x + 1, y + 1, w - 2, h - 2, clrPlayerCol);
	}
	else if (box.value == 2)
	{
		int VertLine;
		int HorzLine;
		VertLine = (((float)w) * (0.2f));
		HorzLine = (((float)h) * (0.2f));

		gDrawManager.OutlineRect(x, y - 1, VertLine, 1, Color(10, 10, 10, 150));
		gDrawManager.OutlineRect(x + w - VertLine, y - 1, VertLine, 1, Color(10, 10, 10, 150));
		gDrawManager.OutlineRect(x, y + h - 1, VertLine, 1, Color(10, 10, 10, 150));
		gDrawManager.OutlineRect(x + w - VertLine, y + h - 1, VertLine, 1, Color(10, 10, 10, 150));

		gDrawManager.OutlineRect(x - 1, y, 1, HorzLine, Color(10, 10, 10, 150));
		gDrawManager.OutlineRect(x - 1, y + h - HorzLine, 1, HorzLine, Color(10, 10, 10, 150));
		gDrawManager.OutlineRect(x + w - 1, y, 1, HorzLine, Color(10, 10, 10, 150));
		gDrawManager.OutlineRect(x + w - 1, y + h - HorzLine, 1, HorzLine, Color(10, 10, 10, 150));

		gDrawManager.OutlineRect(x, y, VertLine, 1, clrPlayerCol);
		gDrawManager.OutlineRect(x + w - VertLine, y, VertLine, 1, clrPlayerCol);
		gDrawManager.OutlineRect(x, y + h, VertLine, 1, clrPlayerCol);
		gDrawManager.OutlineRect(x + w - VertLine, y + h, VertLine, 1, clrPlayerCol);

		gDrawManager.OutlineRect(x, y, 1, HorzLine, clrPlayerCol);
		gDrawManager.OutlineRect(x, y + h - HorzLine, 1, HorzLine, clrPlayerCol);
		gDrawManager.OutlineRect(x + w, y, 1, HorzLine, clrPlayerCol);
		gDrawManager.OutlineRect(x + w, y + h - HorzLine, 1, HorzLine, clrPlayerCol);
	}

	if (health.value == 2 || health.value == 3)
	{
		gDrawManager.OutlineRect(x - 6, y - 1, 5, h, Color::Black());
		gDrawManager.DrawRect(x - 5, y + (h - (h / iMaxHp * iHp)) - 1, 3, h / iMaxHp * iHp, Color::Green());
	}

	if (name.value)
	{
		gDrawManager.DrawString(x + w + 2, y + iY, clrPlayerCol, pInfo.name);
		iY += gDrawManager.GetESPHeight();
	}

	if (GAME_TF2 && tfclass.value)
	{
		gDrawManager.DrawString(x + w + 2, y + iY, clrPlayerCol, "%s", pEntity->szGetClass());
		iY += gDrawManager.GetESPHeight();
	}

	if (health.value == 1 || health.value == 3)
	{
		gDrawManager.DrawString(x + w + 2, y + iY, HealthColor, "%d HP", pEntity->GetHealth());
		iY += gDrawManager.GetESPHeight();
	}
	
	if (viewlines.value)
	{
		//Lines;
		Vector angles = pEntity->GetEyeAngles();
		Vector forward;
		AngleVectors(angles, &forward);
		Vector eyepos = pEntity->GetEyePosition();
		forward = forward * 83 + eyepos;
		Vector screenForward, screenEyepos;
		if (gDrawManager.WorldToScreen(eyepos, screenEyepos) && gDrawManager.WorldToScreen(forward, screenForward))
		{
			gDrawManager.DrawLine(screenForward.x, screenForward.y, screenEyepos.x, screenEyepos.y, clrPlayerCol);
		}
	}

	if (pLocal->IsAlive() && gMisc.backtrack.value && visualize_backtrack.value) {
		if (backtrack::ticks[pEntity->GetIndex()].empty()) {
			return;
		}

		for (unsigned int t = 0; t < backtrack::ticks[pEntity->GetIndex()].size(); t++) {
			Vector hitbox = backtrack::ticks[pEntity->GetIndex()].at(t).head_position, screen;
			if (gDrawManager.WorldToScreen(hitbox, screen)) {
				gDrawManager.DrawLine(screen[0] - 8, screen[1] - 0, screen[0] + 8, screen[1] + 0, Color(255, 0, 0, 200));
				gDrawManager.DrawLine(screen[0] + 0, screen[1] - 8, screen[0] - 0, screen[1] + 8, Color(255, 0, 0, 200));
				gDrawManager.DrawLine(screen[0] - 4, screen[1] - 0, screen[0] + 4, screen[1] + 0, Color(255, 255, 255, 255));
				gDrawManager.DrawLine(screen[0] + 0, screen[1] - 4, screen[0] - 0, screen[1] + 4, Color(255, 255, 255, 255));
			}
		}
	}

	if (GAME_TF2 && status.value)
	{
		switch (pEntity->GetCond())
		{
		case TFCond_Slowed:
			gDrawManager.DrawString(x + w + 2, y + iY, Orange, "(SLOWED)");
			break;
		case TFCond_Zoomed:
			gDrawManager.DrawString(x + w + 2, y + iY, Orange, "(ZOOMED)");
			break;
		case TFCond_Bonked:
			gDrawManager.DrawString(x + w + 2, y + iY, Orange, "(BONKED)");
			break;
		case TFCond_Disguised:
			gDrawManager.DrawString(x + w + 2, y + iY, Orange, "(DISGUISED)");
			break;
		case TFCond_Cloaked:
			gDrawManager.DrawString(x + w + 2, y + iY, Orange, "(CLOAKED)");
			break;
		case TFCond_Ubercharged:
			gDrawManager.DrawString(x + w + 2, y + iY, Orange, "(Ubercharged)");
			break;
		case TFCond_UberchargeFading:
			gDrawManager.DrawString(x + w + 2, y + iY, Orange, "(Ubercharge Fading)");
			break;
		}
		iY += gDrawManager.GetESPHeight();
	}

	if (bones.value) //bones
	{
		static int iLeftArmBones[] = { 8, 7, 6, 4 };
		static int iRightArmBones[] = { 11, 10, 9, 4 };
		static int iHeadBones[] = { 0, 4, 1 };
		static int iLeftLegBones[] = { 14, 13, 1 };
		static int iRightLegBones[] = { 17, 16, 1 };

		DrawBone(pEntity, iLeftArmBones, 4, clrBoneCol);
		DrawBone(pEntity, iRightArmBones, 4, clrBoneCol);

		DrawBone(pEntity, iHeadBones, 3, clrBoneCol);

		DrawBone(pEntity, iLeftLegBones, 3, clrBoneCol);
		DrawBone(pEntity, iRightLegBones, 3, clrBoneCol);
	}

	Vector b = pLocal->GetAbsAngles();
}

static SColor colors_team[] =
{
	SColor(0), //Dummy
	SColor(160), // 1 Teamone (UNUSED)
	SColor(186, 52, 53), // 2 Red
	SColor(0, 128, 255), // 3 Blu
	SColor(0) // 4 Teamfour (UNUSED) 
};

static SColor colors_team_light[] =
{
	SColor(0), //Dummy
	SColor(160), // 1 Teamone (UNUSED)
	SColor(240, 100, 90), // 2 Red
	SColor(100, 150, 240), // 3 Blu
	SColor(0) // 4 Teamfour (UNUSED) 
};

#include "Client.h"
#include "CMat.h"
void CESP::DrawModelExecute(const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld, DrawModelExecuteFn Original)
{
	const char* pszModelName = gInts.ModelInfo->GetModelName((DWORD*)pInfo.pModel);
	CBaseEntity* entity;
	CBaseEntity* pLocalEntity = GetBaseEntity(me);

	if (hands_enabled.value == 2 && strstr(pszModelName, "arms"))
	{
		Matptr desiredhands;

		if (GAME_TF2)
		{
			if (gESP.hands_chams.value == 0)
				desiredhands = gMat.glow;
			else if (gESP.hands_chams.value == 1)
				desiredhands = gMat.shaded;
			else if (gESP.hands_chams.value == 2)
				desiredhands = gMat.shiny;
			else if (gESP.hands_chams.value == 3)
				desiredhands = gMat.wireframe;
		}
		else
		{
			if (gESP.hands_chams.value == 0)
				desiredhands = gMat.glowother;
			else if (gESP.hands_chams.value == 1)
				desiredhands = gMat.shadedother;
			else if (gESP.hands_chams.value == 2)
				desiredhands = gMat.shinyother;
			else if (gESP.hands_chams.value == 3)
				desiredhands = gMat.wireframeother;
		}

		byte localteam = pLocalEntity->GetTeamNum();

		CBaseEntity* pLocal = (CBaseEntity*)gInts.EntList->GetClientEntity(gInts.Engine->GetLocalPlayer());
		SColor color = hand_mat_color.bDef ? colors_team_light[localteam] : hand_mat_color.color;

		if (pLocal->IsAlive() && pLocal->GetHealth() > 0 && !pLocal->IsDormant()) 
		{
			Color RGBA = gDrawManager.GetPlayerColor(pLocal);

			desiredhands->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			desiredhands->ColorModulate(RGBA[0], RGBA[1], RGBA[2]);
			desiredhands->AddRef();

			gMat.ForceMaterial(color, desiredhands);
		}
		else
		{
			gInts.ModelRender->ForcedMaterialOverride(NULL, OverrideType_t::OVERRIDE_NORMAL);
		}
	}

	if (player_enabled.value)
	{
		if (!(entity = GetBaseEntity(pInfo.entity_index)))
			return;

		static std::string sModelStr = "models/player";

		bool bPlayerModel = strstr(pszModelName, sModelStr.data());

		if (GAME_TF2)
		{
			ent_id type = entity->Type();
			if (type != ent_id::CTFPlayer)
				return;
		}
		else
			if (!bPlayerModel)
				return;


		// Normal visibility will just run once
		// And "Always" visibility will run twice with the same mat
		bool bReset = player_enabled.value > 2;

		// Stubbed for now. More materials later.
		Matptr desired;
		if (GAME_TF2)
		{
			if (gESP.chamsmat.value == 0)
				desired = gMat.glow;
			else if (gESP.chamsmat.value == 1)
				desired = gMat.shaded;
			else if (gESP.chamsmat.value == 2)
				desired = gMat.shiny;
			else if (gESP.chamsmat.value == 3)
				desired = gMat.wireframe;
		}
		else
		{
			if (gESP.chamsmat.value == 0)
				desired = gMat.glowother;
			else if (gESP.chamsmat.value == 1)
				desired = gMat.shadedother;
			else if (gESP.chamsmat.value == 2)
				desired = gMat.shinyother;
			else if (gESP.chamsmat.value == 3)
				desired = gMat.wireframeother;
		}

		if (!desired)
			return;

		byte team = entity->GetTeamNum();
		byte localteam = pLocalEntity->GetTeamNum();

		if (gESP.enemyonly.value && team == localteam)
			return;

		SColor color = player_mat.bDef ? colors_team_light[team] : player_mat.color;

		desired->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, player_enabled.value != 1);
		gMat.ForceMaterial(color, desired);

		// "Behind walls" / "always" requires a second render with ignorez before the original
		if (player_enabled.value != 1)
		{
			Original(gInts.ModelRender, state, pInfo, pCustomBoneToWorld);
			if (player_enabled.value == 2) // Now we will render a normal visibility model with "always" enabled
				desired->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
		}

		if (bReset)
			gMat.ResetMaterial();
	}
}


	 //My code, but creds to f1ssion for giving me the idea
void CESP::DrawBone(CBaseEntity* pEntity, int* iBones, int count, Color clrCol)
{
	for (int i = 0; i < count; i++)
	{
		if (i == count - 1)
			continue;

		Vector vBone1 = pEntity->GetHitboxPosition(iBones[i]);
		Vector vBone2 = pEntity->GetHitboxPosition(iBones[i + 1]);

		Vector vScr1, vScr2;

		if (!gDrawManager.WorldToScreen(vBone1, vScr1) || !gDrawManager.WorldToScreen(vBone2, vScr2))
			continue;

		gDrawManager.DrawLine(vScr1.x, vScr1.y, vScr2.x, vScr2.y, clrCol);
	}
}

unordered_map<MaterialHandle_t, SColor> worldmats_new, worldmats_old;

void CESP::FrameStageNotify(ClientFrameStage_t Stage)
{
	bool bReset = !gInts.Engine->IsInGame() || !world_enabled.value;
	if (bReset && worldmats_old.size())
	{
		if (gInts.Engine->IsInGame())
		{
			for (auto& hMat : worldmats_old) // Reset the material colors
			{
				IMaterial* mat;
				if (GAME_TF2)
					mat = gInts.MatSystem->GetMaterial(hMat.first);
				else
					mat = gInts.MatSystemOther->GetMaterial(hMat.first);

				if (!mat)
					continue;

				SColor color = hMat.second;
				float blend[4] = { (float)color[0] / 255.f, (float)color[1] / 255.f, (float)color[2] / 255.f, (float)color[3] / 255.f };
				mat->ColorModulate(blend[0], blend[1], blend[2]);
				mat->AlphaModulate(blend[3]);
			}
		}

		worldmats_new.clear();	// Clear cache of materials
		worldmats_old.clear();	//
	}

	if (!world_enabled.value || Stage != FRAME_NET_UPDATE_POSTDATAUPDATE_END)
		return;
	if (GAME_TF2)
	{
		for (MaterialHandle_t i = gInts.MatSystem->FirstMaterial(); i != gInts.MatSystem->InvalidMaterial(); i = gInts.MatSystem->NextMaterial(i))
		{
			IMaterial* mat = gInts.MatSystem->GetMaterial(i);
			if (!mat)
				continue;

			bool bIsSkybox = !strcmp(mat->GetTextureGroupName(), TEXTURE_GROUP_SKYBOX);
			if (!bIsSkybox && strcmp(mat->GetTextureGroupName(), TEXTURE_GROUP_WORLD))
				continue;

			if (worldmats_new.find(i) == worldmats_new.end())
			{
				float r, g, b, a = mat->GetAlphaModulation();
				mat->GetColorModulation(&r, &g, &b);

				SColor old_color(r * 255.f, g * 255.f, b * 255.f, a * 255.f);
				worldmats_old.emplace(i, old_color);
				worldmats_new.emplace(i, old_color);
			}

			SColor color = worldmats_old.at(i);
			if (bIsSkybox)
				color = sky_clr.bDef ? SColor(255) : sky_clr.color;
			else
				color = world_clr.bDef ? SColor(255) : world_clr.color;

			if (worldmats_new.at(i) != color)
			{
				float blend[4] = { (float)color[0] / 255.f, (float)color[1] / 255.f, (float)color[2] / 255.f, (float)color[3] / 255.f };
				mat->ColorModulate(blend[0], blend[1], blend[2]);
				mat->AlphaModulate(blend[3]);

				worldmats_new.at(i) = color;
			}
		}
	}
	else
	{
		for (MaterialHandle_t i = gInts.MatSystemOther->FirstMaterial(); i != gInts.MatSystemOther->InvalidMaterial(); i = gInts.MatSystemOther->NextMaterial(i))
		{
			IMaterial* mat = gInts.MatSystemOther->GetMaterial(i);
			if (!mat)
				continue;

			bool bIsSkybox = !strcmp(mat->GetTextureGroupName(), TEXTURE_GROUP_SKYBOX);
			if (!bIsSkybox && strcmp(mat->GetTextureGroupName(), TEXTURE_GROUP_WORLD))
				continue;

			if (worldmats_new.find(i) == worldmats_new.end())
			{
				float r, g, b, a = mat->GetAlphaModulation();
				mat->GetColorModulation(&r, &g, &b);

				SColor old_color(r * 255.f, g * 255.f, b * 255.f, a * 255.f);
				worldmats_old.emplace(i, old_color);
				worldmats_new.emplace(i, old_color);
			}

			SColor color = worldmats_old.at(i);
			if (bIsSkybox)
				color = sky_clr.bDef ? SColor(255) : sky_clr.color;
			else
				color = world_clr.bDef ? SColor(255) : world_clr.color;

			if (worldmats_new.at(i) != color)
			{
				float blend[4] = { (float)color[0] / 255.f, (float)color[1] / 255.f, (float)color[2] / 255.f, (float)color[3] / 255.f };
				mat->ColorModulate(blend[0], blend[1], blend[2]);
				mat->AlphaModulate(blend[3]);

				worldmats_new.at(i) = color;
			}
		}
	}
}
