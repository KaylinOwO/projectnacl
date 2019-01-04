#include "SDK.h"

class CGlobalVariables
{
public:
	int iAimbotIndex = -1;

	float menu_style = 1;
	float menu_color = 0;

	float settings_switch;
	float iMenu_Pos_X = 300;
	float iMenu_Pos_Y = 75;
	float szsubstest;

	float aimbot_switch;
	float aimbot_active;
	float aimbot_key;
	float aimbot_fov = 1;
	float aimbot_melee = 1;
	float aimbot_medigun;
	float aimbot_zoomedonly;
	float aimbot_waitforcharge;
	float aimbot_hitscan;
	float aimbot_sniperhitscan;
	float aimbot_hitbox;
	float aimbot_silent;
	float aimbot_smooth;
	float aimbot_badresolver;
	float aimbot_autoshoot;
	float aimbot_ignorecloaked;
	float aimbot_ignoredisguised;
	float aimbot_ignoretaunting;

	float vismisc_active;

	float triggerbot_switch;
	float triggerbot_active;
	float triggerbot_key;
	float triggerbot_headonly;
	float triggerbot_zoomedonly;
	float triggerbot_melee;
	float triggerbot_autobackstab;
	float autoairblast_enabled;
	float autoairblast_silent = true;
	float autoairblast_rage;

	float melee_enable;

	float vis_switch;

	float esp_switch;
	float esp_active;
	float esp_door;
	float esp_enemyonly;
	float esp_box;
	float esp_name;
	float esp_status;
	float esp_status_def;
	float esp_friends;
	float esp_class;
	float esp_health;
	float esp_bones;
	float esp_aimbothighlights = true;
	float esp_smileyface;
	float esp_view_lines;

	float radar_switch;
	float radar_enabled;
	float radar_posy = 101;
	float radar_posx = 101;
	float radar_width = 100;

	float autoairblast_actionOnDeflect;

	float misc_switch;
	float misc_enabled;
	float misc_bunnyhop;
	float misc_autostrafe;
	float misc_tauntslide;
	float misc_fix;
	float misc_nopush;
	float misc_lagexploit;
	float misc_lagexploit_key;
	float misc_bigheadisbig;
	float misc_bigtorsoisbig;
	float misc_thirdperson;
	float sky_changer;
	float sky_changer_value;
	float misc_cleanscreenshots;
	float misc_nozoom;
	float misc_nozoom2;
	float misc_noscope;
	float misc_noscope_disablecrosshair;
	float misc_noscope_drawlines;
	float misc_noisemaker_spam;
	float misc_chatspam;
	float misc_miniguntoggle;
	float misc_chatspam_newlines;
	float misc_cheatsbypass;
	float misc_claymode;
	float misc_speedcrouch;
	float misc_fakelag;
	float misc_fakelag_value = 1;
	float aimbot_resolver;
	float aimbot_resolver_key;
	float hvh_switch;
	float hvh_aaswitch;
	float hvh_aap;
	float hvh_aay;
	float hvh_spinspeed;
	float hvh_resolver;
	float hvh_resolver_fakeup;
	float hvh_resolver_fakedown;
	float res_left;
	float res_right;
	float res_up;
	float res_half_up;
	float res_center;
	float res_half_down;
	float res_down;

	float removecond_switch;
	float removecond_enabled;
	float removecond_value = 800;
	float removecond_key = 3;
	float removecond_automedkit;
	float removecond_autoinstantcap;
	float removecond_disableonattack = true;
	float removecond_onattack;


	float originalCrosshair;

	float playerlist_switch;
	float PlayerMode[64] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	};
};