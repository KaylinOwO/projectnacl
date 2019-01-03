#include "CMenu.h"
#include "SDK.h"
#include "CDrawManager.h"
#include "Util.h"

CCheatMenu gCheatMenu;

int ScreenH;
int ScreenW;

char* szKeyNames[] = {
	"None", "Mouse 1", "Mouse 2", "Mouse 3", "Mouse 4", "Mouse 5", "Shift", "Alt", "F",
};

char* szHealthModes[] = {
	"OFF", "Text", "Bar", "Both",
};

char* szBoneModes[] = {
	"OFF", "White", "Health", "Team",
};

char* szSkys[] = {
	"Night", "Nightfall", "HarvestN", "Halloween",
};

char* szMenuColors[] = {
	"Default", "Blue", "Red", "Yellow", "Green",
};


char* szPitchAA[] = {
	"None", "Fake Up", "Up", "Fake Down", "Down",
};

char* szYawAA[] = {
	"None", "Right", "Left", "Back", "Emotion", "Random", "FSRight", "FSLeft", "FRight", "HBL", "HBR", "FLeft", "FManual",
};

char* szNewHitboxes[] =
{
	"Auto", "Head", "Body",
};

int CCheatMenu::AddItem(int nIndex, char szTitle[30], float* value, float flMin, float flMax, float flStep, bool isClassSwitch)
{
	strcpy(pMenu[nIndex].szTitle, szTitle);
	pMenu[nIndex].value = value;
	pMenu[nIndex].flMin = flMin;
	pMenu[nIndex].flMax = flMax;
	pMenu[nIndex].flStep = flStep;
	pMenu[nIndex].isClassSwitch = isClassSwitch;
	return (nIndex + 1);
}
enum class e_mw
{
	null,
	up,
	down
};

byte key = NULL;

void CCheatMenu::Render(void)
{//
	gInts.Engine->GetScreenSize(ScreenW, ScreenH);

	int i = 0;


	i = AddItem(i, "Aimbot", &gCvars.aimbot_switch, 0, 1, 1, true);
	if (gCvars.aimbot_switch)
	{
		i = AddItem(i, " - Enabled", &gCvars.aimbot_active, 0, 1, 1, false);
		i = AddItem(i, "  * Key", &gCvars.aimbot_key, 0, 8, 1, false);
		i = AddItem(i, "  * FOV", &gCvars.aimbot_fov, 0, 180, 1, false);
		if (!gCvars.aimbot_silent)
		{
			i = AddItem(i, "    > Smooth", &gCvars.aimbot_smooth, 0, 180, 1, false);
		}
		i = AddItem(i, "  * Silent", &gCvars.aimbot_silent, 0, 1, 1, false);
		i = AddItem(i, "  * Zoomed Only", &gCvars.aimbot_zoomedonly, 0, 1, 1, false);
		i = AddItem(i, "  * Wait For Charge", &gCvars.aimbot_waitforcharge, 0, 1, 1, false);
		i = AddItem(i, "  * Hitbox", &gCvars.aimbot_hitbox, 0, 2, 1, false);
		i = AddItem(i, "  * Melee Aimbot", &gCvars.aimbot_melee, 0, 1, 1, false);
		i = AddItem(i, "  * Autoshoot", &gCvars.aimbot_autoshoot, 0, 1, 1, false);
		i = AddItem(i, "  * Ignore Cloaked", &gCvars.aimbot_ignorecloaked, 0, 1, 1, false);
		i = AddItem(i, "  * Ignore Disguised", &gCvars.aimbot_ignoredisguised, 0, 1, 1, false);
		i = AddItem(i, "  * Ignore Taunting", &gCvars.aimbot_ignoretaunting, 0, 1, 1, false);
	}


	i = AddItem(i, "Triggerbot", &gCvars.triggerbot_switch, 0, 1, 1, true);
	if (gCvars.triggerbot_switch)
	{
		i = AddItem(i, " - Hitscan", &gCvars.triggerbot_active, 0, 1, 1, false);
		if (gCvars.triggerbot_active)
		{
			i = AddItem(i, "  * Key", &gCvars.triggerbot_key, 0, 8, 1, false);
			i = AddItem(i, "  * Zoomed Only", &gCvars.triggerbot_zoomedonly, 0, 1, 1, false);
			i = AddItem(i, "  * Head Only", &gCvars.triggerbot_headonly, 0, 1, 1, false);
		}

		i = AddItem(i, " - Misc", &gCvars.melee_enable, 0, 1, 1, false);
		if (gCvars.melee_enable)
		{
			i = AddItem(i, "  * Auto-Backstab", &gCvars.triggerbot_autobackstab, 0, 1, 1, false);
			i = AddItem(i, "  * Auto-Airblast", &gCvars.autoairblast_enabled, 0, 1, 1, false);
			if (gCvars.autoairblast_enabled)
			{
				i = AddItem(i, "    > Rage Mode", &gCvars.autoairblast_rage, 0, 1, 1, false);
			}
		}
	}


	i = AddItem(i, "Player List", &gCvars.playerlist_switch, 0, 1, 1, true);
	for (int p = 1; p <= gInts.Engine->GetMaxClients(); p++)
	{
		if (p == me)
			continue;

		CBaseEntity *pPlayer = GetBaseEntity(p);

		if (pPlayer == NULL)
			continue;

		player_info_t pInfo;
		gInts.Engine->GetPlayerInfo(p, &pInfo);

		char szString[256];
		sprintf(szString, " - %s", pInfo.name);

		if (gCvars.playerlist_switch)
			i = AddItem(i, szString, &gCvars.PlayerMode[pPlayer->GetIndex()], 0, 2, 1, false);
	}

	i = AddItem(i, "Visuals", &gCvars.vis_switch, 0, 1, 1, true);
	if (gCvars.vis_switch)
	{
		i = AddItem(i, "  - ESP/Radar", &gCvars.esp_active, 0, 1, 1, false);
		if (gCvars.esp_active)
		{
			i = AddItem(i, "   * Enemies Only", &gCvars.esp_enemyonly, 0, 1, 1, false);
			i = AddItem(i, "   * Box", &gCvars.esp_box, 0, 1, 1, false);
			i = AddItem(i, "   * Name", &gCvars.esp_name, 0, 1, 1, false);
			i = AddItem(i, "   * Class", &gCvars.esp_class, 0, 1, 1, false);
			i = AddItem(i, "   * Status", &gCvars.esp_status, 0, 1, 1, false);
			i = AddItem(i, "   * Health", &gCvars.esp_health, 0, 3, 1, false);
			i = AddItem(i, "   * Bones", &gCvars.esp_bones, 0, 3, 1, false);
			i = AddItem(i, "   * Door ESP", &gCvars.esp_door, 0, 1, 1, false);
			i = AddItem(i, "   * Aimbot Highlight", &gCvars.esp_aimbothighlights, 0, 1, 1, false);
			i = AddItem(i, "   * Radar", &gCvars.radar_enabled, 0, 1, 1, false);
			if (gCvars.radar_enabled)
			{
				i = AddItem(i, "    > Position X", &gCvars.radar_posx, 0, 1000.0, 101, false);
				i = AddItem(i, "    > Position Y", &gCvars.radar_posy, 0, 1000.0, 101, false);
				i = AddItem(i, "    > Width", &gCvars.radar_width, 0, 255.0, 100, false);
			}
		}

		i = AddItem(i, "  - Misc", &gCvars.vismisc_active, 0, 1, 1, false);
		if (gCvars.vismisc_active)
		{
			i = AddItem(i, "   * Sky Changer", &gCvars.sky_changer, 0, 1, 1, false);
			if (gCvars.sky_changer)
				i = AddItem(i, "    > Sky Selection", &gCvars.sky_changer_value, 0, 3, 1, false);

			i = AddItem(i, "   * No Zoom", &gCvars.misc_nozoom, 0, 1, 1, false);
			i = AddItem(i, "   * No Scope", &gCvars.misc_noscope, 0, 1, 1, false);
			if (gCvars.misc_noscope)
			{
				i = AddItem(i, "    > Draw Lines", &gCvars.misc_noscope_drawlines, 0, 1, 1, false);
				i = AddItem(i, "    > Disable CHair", &gCvars.misc_noscope_disablecrosshair, 0, 1, 1, false);
			}
			i = AddItem(i, "   * Thirdperson", &gCvars.misc_thirdperson, 0, 1, 1, false);
		}
	}

	i = AddItem(i, "HvH", &gCvars.hvh_switch, 0, 1, 1, true);
	if (gCvars.hvh_switch)
	{
		i = AddItem(i, " - Anti Aim", &gCvars.hvh_aaswitch, 0, 1, 1, false);
		if (gCvars.hvh_aaswitch)
		{
			i = AddItem(i, "  * Pitch", &gCvars.hvh_aap, 0, 4, 1, false);
			i = AddItem(i, "  * Yaw", &gCvars.hvh_aay, 0, 12, 1, false);
		}
		i = AddItem(i, " - Resolver", &gCvars.aimbot_resolver, 0, 1, 1, false);
		if (gCvars.aimbot_resolver)
		{
			i = AddItem(i, "  * Toggle Key", &gCvars.aimbot_resolver_key, 0, 8, 1, false);
		}
	}

	i = AddItem(i, "Misc", &gCvars.misc_switch, 0, 1, 1, true);
	if (gCvars.misc_switch)
	{
		i = AddItem(i, " - Enabled", &gCvars.misc_enabled, 0, 1, 1, false);
		if (gCvars.misc_enabled)
		{
			i = AddItem(i, "  * Bunnyhop", &gCvars.misc_bunnyhop, 0, 1, 1, false);
			i = AddItem(i, "  * Autostrafe", &gCvars.misc_autostrafe, 0, 1, 1, false);
			i = AddItem(i, "  * Clean Screenshots", &gCvars.misc_cleanscreenshots, 0, 1, 1, false);
			i = AddItem(i, "  * Taunt Slide", &gCvars.misc_tauntslide, 0, 1, 1, false);
			i = AddItem(i, "  * No Push", &gCvars.misc_nopush, 0, 1, 1, false);
			i = AddItem(i, "  * Cheats Bypass", &gCvars.misc_cheatsbypass, 0, 1, 1, false);
			i = AddItem(i, "  * Clay Mode", &gCvars.misc_claymode, 0, 1, 1, false);
			i = AddItem(i, "  * Crouch Exploit", &gCvars.misc_speedcrouch, 0, 1, 1, false);
			i = AddItem(i, "  * Fakelag", &gCvars.misc_fakelag, 0, 1, 1, false);
			if (gCvars.misc_fakelag)
				i = AddItem(i, "   > Value", &gCvars.misc_fakelag_value, 1, 15, 1, false);
			i = AddItem(i, "  * Chat Spam", &gCvars.misc_chatspam, 0, 1, 1, false);
			if (gCvars.misc_chatspam)
				i = AddItem(i, "   > Newlines", &gCvars.misc_chatspam_newlines, 0, 32, 1, false);
			i = AddItem(i, "  * Noisemaker Spam", &gCvars.misc_noisemaker_spam, 0, 1, 1, false);
		}
	}

	i = AddItem(i, "Settings", &gCvars.settings_switch, 0, 1, 1, true);
	if (gCvars.settings_switch)
	{
		i = AddItem(i, " - Menu Postion X", &gCvars.iMenu_Pos_X, 0, ScreenW, 25, false);
		i = AddItem(i, " - Menu Postion Y", &gCvars.iMenu_Pos_Y, 0, ScreenH, 25, false);
		i = AddItem(i, " - Menu Color", &gCvars.menu_color, 0, 4, 1, false);
	}

	iMenuItems = i;
}

void CCheatMenu::DrawMenu(void)
{
	int x = gCvars.iMenu_Pos_X,
		xx = x + 150,
		y = gCvars.iMenu_Pos_Y,
		w = 210,
		h = 14;

	CBaseEntity *pLocal = GetBaseEntity(me);
	//sound / player / brutal_legend_taunt.wav
	Color clrColor = gDrawManager.MenuColor(pLocal);

	gDrawManager.OutlineRect(x, y - (h + 4), w, iMenuItems * h + 21, clrColor);

	gDrawManager.OutlineRect(x - 1, (y - (h + 4)) - 1, w + 2, (iMenuItems * h + 21) + 2, Color(0, 0, 0, 255));
	gDrawManager.OutlineRect(x + 1, (y - (h + 4)) + 1, w - 2, (iMenuItems * h + 21) - 2, Color(0, 0, 0, 255));

	gDrawManager.DrawString(x + 4, y - 16, clrColor, "%s", CHEATNAME);

	for (int i = 0; i < iMenuItems; i++)
	{
		if (i != iMenuIndex)
		{
			if (pMenu[i].isClassSwitch)
			{
				if (!(pMenu[i].value[0]))
				{
					gDrawManager.DrawString(x + 2, y + (h * i), clrColor, "[+] %s", pMenu[i].szTitle);
				}
				else if (pMenu[i].value[0])
				{
					gDrawManager.DrawString(x + 2, y + (h * i), clrColor, "[-] %s", pMenu[i].szTitle);
				}
			}
			else
			{
				gDrawManager.DrawString(x + 4, y + (h * i), Color::White(), pMenu[i].szTitle);

				if (!strcmp(pMenu[i].szTitle, "   * Health"))
				{
					gDrawManager.DrawString(xx, y + (h * i), pMenu[i].value[0] ? Color::White() : Color(105, 105, 105, 255), "%s", szHealthModes[(int)pMenu[i].value[0]]);
				}

				else if (!strcmp(pMenu[i].szTitle, "   * Bones"))
				{
					gDrawManager.DrawString(xx, y + (h * i), pMenu[i].value[0] ? Color::White() : Color(105, 105, 105, 255), "%s", szBoneModes[(int)pMenu[i].value[0]]);
				}

				else if (!strcmp(pMenu[i].szTitle, "  * Hitbox"))
				{
					gDrawManager.DrawString(xx, y + (h * i), pMenu[i].value[0] ? Color::White() : Color(105, 105, 105, 255), "%s", szNewHitboxes[(int)pMenu[i].value[0]]);
				}

				else if (!strcmp(pMenu[i].szTitle, "    > Sky Selection"))
				{
					gDrawManager.DrawString(xx, y + (h * i), pMenu[i].value[0] ? Color::White() : Color(105, 105, 105, 255), "%s", szSkys[(int)pMenu[i].value[0]]);
				}

				else if (!strcmp(pMenu[i].szTitle, " - Menu Color"))
				{
					gDrawManager.DrawString(xx, y + (h * i), pMenu[i].value[0] ? Color::White() : Color(105, 105, 105, 255), "%s", szMenuColors[(int)pMenu[i].value[0]]);
				}

				else if (!strcmp(pMenu[i].szTitle, "  * Pitch"))
				{
					gDrawManager.DrawString(xx, y + (h * i), pMenu[i].value[0] ? Color::White() : Color(105, 105, 105, 255), "%s", szPitchAA[(int)pMenu[i].value[0]]);
				}

				else if (!strcmp(pMenu[i].szTitle, "  * Yaw"))
				{
					gDrawManager.DrawString(xx, y + (h * i), pMenu[i].value[0] ? Color::White() : Color(105, 105, 105, 255), "%s", szYawAA[(int)pMenu[i].value[0]]);
				}

				else if (pMenu[i].flMax == 8)
				{
					gDrawManager.DrawString(xx, y + (h * i), Color::White(), "%s", szKeyNames[(int)pMenu[i].value[0]]);
				}

				else if (pMenu[i].flMax == 2)
				{
					gDrawManager.DrawString(xx, y + (h * i), Color::White(), !pMenu[i].value[0] ? "Ignore" : pMenu[i].value[0] == 1 ? "Normal" : "Rage");
				}

				else if (pMenu[i].flMax == 1)
				{
					gDrawManager.DrawString(xx, y + (h * i), pMenu[i].value[0] ? Color::White() : Color(105, 105, 105, 255), pMenu[i].value[0] ? "ON" : "OFF");
				}

				else if (pMenu[i].value[0] >= 1 && pMenu[i].flMax > 1)
				{
					gDrawManager.DrawString(xx, y + (h * i), pMenu[i].value[0] >= 1 ? Color::White() : Color(105, 105, 105, 255), "%0.0f", pMenu[i].value[0]);
				}
			}
		}
		else
		{
			gDrawManager.DrawRect(x + 1, y + (h * i), w - 2, h, Color(255, 255, 255, 80));

			if (pMenu[i].isClassSwitch)
			{
				if (!(pMenu[i].value[0]))
				{
					gDrawManager.DrawString(x + 2, y + (h * i), clrColor, "[+] %s", pMenu[i].szTitle);
				}
				else if (pMenu[i].value[0])
				{
					gDrawManager.DrawString(x + 2, y + (h * i), clrColor, "[-] %s", pMenu[i].szTitle);
				}
			}
			else
			{
				gDrawManager.DrawString(x + 4, y + (h * i), clrColor, pMenu[i].szTitle);

				if (!strcmp(pMenu[i].szTitle, "   * Health"))
				{
					gDrawManager.DrawString(xx, y + (h * i), pMenu[i].value[0] ? Color::White() : Color(105, 105, 105, 255), "%s", szHealthModes[(int)pMenu[i].value[0]]);
				}

				else if (!strcmp(pMenu[i].szTitle, "   * Bones"))
				{
					gDrawManager.DrawString(xx, y + (h * i), pMenu[i].value[0] ? Color::White() : Color(105, 105, 105, 255), "%s", szBoneModes[(int)pMenu[i].value[0]]);
				}

				else if (!strcmp(pMenu[i].szTitle, "  * Hitbox"))
				{
					gDrawManager.DrawString(xx, y + (h * i), pMenu[i].value[0] ? Color::White() : Color(105, 105, 105, 255), "%s", szNewHitboxes[(int)pMenu[i].value[0]]);
				}

				else if (!strcmp(pMenu[i].szTitle, "    > Sky Selection"))
				{
					gDrawManager.DrawString(xx, y + (h * i), pMenu[i].value[0] ? Color::White() : Color(105, 105, 105, 255), "%s", szSkys[(int)pMenu[i].value[0]]);
				}

				else if (!strcmp(pMenu[i].szTitle, " - Menu Color"))
				{
					gDrawManager.DrawString(xx, y + (h * i), pMenu[i].value[0] ? Color::White() : Color(105, 105, 105, 255), "%s", szMenuColors[(int)pMenu[i].value[0]]);
				}

				else if (!strcmp(pMenu[i].szTitle, "  * Pitch"))
				{
					gDrawManager.DrawString(xx, y + (h * i), pMenu[i].value[0] ? Color::White() : Color(105, 105, 105, 255), "%s", szPitchAA[(int)pMenu[i].value[0]]);
				}

				else if (!strcmp(pMenu[i].szTitle, "  * Yaw"))
				{
					gDrawManager.DrawString(xx, y + (h * i), pMenu[i].value[0] ? Color::White() : Color(105, 105, 105, 255), "%s", szYawAA[(int)pMenu[i].value[0]]);
				}

				else if (pMenu[i].flMax == 8)
				{
					gDrawManager.DrawString(xx, y + (h * i), clrColor, "%s", szKeyNames[(int)pMenu[i].value[0]]);
				}

				else if (pMenu[i].flMax == 2)
				{
					gDrawManager.DrawString(xx, y + (h * i), clrColor, !pMenu[i].value[0] ? "Ignore" : pMenu[i].value[0] == 1 ? "Normal" : "Rage");
				}

				else if (pMenu[i].flMax == 1)
				{
					gDrawManager.DrawString(xx, y + (h * i), clrColor, pMenu[i].value[0] ? "ON" : "OFF");
				}

				else
				{
					gDrawManager.DrawString(xx, y + (h * i), clrColor, "%0.0f", pMenu[i].value[0]);
				}
			}
		}
	}
}
