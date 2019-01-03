#include "ESP.h"
#include "CDrawManager.h"
#include "Util.h"

#include "Aimbot.h"

#include "CEtags.h"

CESP gESP;

void CESP::Run(CBaseEntity* pLocal)
{
	Vector vOrigin, vIdk;
	if (!enabled.value)
		return;
	for (int i = 1; i <= gInts.Engine->GetMaxClients(); i++)
	{
	//	if (i == me)
	//		return Color(255, 165, 0, 255);

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

	return Color(0, 0, 0, 0); //no reason for this to be here, i just wanna look smart
}

Color GetHealthColor(CBaseEntity* pPlayer)
{
		if (pPlayer->GetHealth() > 100)
			return Color(0, 255, 0, 255);
		if (pPlayer->GetHealth() > 65)
			return Color(55, 255, 0, 255);
		if (pPlayer->GetHealth() > 35)
			return Color(255, 20, 20, 255);
	return Color(255, 20, 20, 255); //no reason for this to be here, i just wanna look smart
}
#include "Radar.h"
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

	if (tfclass.value)
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



	if (status.value)
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

	//if (radar.value) Im too lazy to add the radar shit into the new menu (which would take 5 seconds and would probably be even faster than writing this but I don't think anyone actually liked it anyways)
		//gRadar.DrawRadarPoint(pEntity->GetAbsOrigin(), pLocal->GetAbsOrigin(), QAngle(b.x, b.y, b.z), pEntity, clrTeam);
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
