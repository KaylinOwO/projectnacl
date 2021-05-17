#pragma once
#include "SDK.h"

class CAimbot
{
public:

	void Run(CBaseEntity* pLocal, CUserCmd* pCommand);

	float GetFOV(Vector angle, Vector src, Vector dst);

	Vector EstimateAbsVelocity(CBaseEntity* entity);

	void Projectile(CBaseEntity* local_player, CBaseEntity* entity, CBaseCombatWeapon* local_weapon, Vector& vec_hitbox);

	Checkbox enabled = Checkbox("Enabled");
	Listbox key = Listbox("Key", { "None", "Mouse 1", "Mouse 2", "Mouse 3", "Mouse 4", "Mouse 5", "Shift", "Alt", "F" }, 0);
	Slider fov = Slider("FOV Limit", 30, 1, 180, 1);
	Slider smooth = Slider("Smooth", 0, 0, 180, 1);
	Checkbox silent = Checkbox("Silent");
	Checkbox Autoshoot = Checkbox("Autoshoot");
	Checkbox waitforcharge = Checkbox("Wait For Charge");
	Checkbox zoomedonly = Checkbox("Zoomed Only");
	Checkbox aimatteammates = Checkbox("Aim @ Teammates");

	Checkbox antirecoil = Checkbox("Anti-Recoil");
	Checkbox autopistol = Checkbox("Auto Pistol");

	Listbox hitbox = Listbox("Hitbox", { "Auto", "Head", "Body"}, 0);
	Checkbox ignorecloaked = Checkbox("Ignore Cloaked");
	Checkbox ignoredisguised = Checkbox("Ignore Disguised");

	bool IsProjectileWeapon(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon) {
		auto pLocalClass = pLocal->GetClassNum(), WeaponSlot = pWeapon->GetSlot();
		if (pLocalClass == TF2_Demoman || pLocalClass == TF2_Soldier || pLocalClass == TF2_Medic)
		{
			if (WeaponSlot == 0) {
				return true;
			}
		}

		if (pLocalClass == TF2_Engineer)
		{
			if (WeaponSlot == 0) {
				if (pWeapon->GetItemDefinitionIndex() == WPN_RescueRanger) {
					return true;
				}
			}
		}

		return false;
	};

private:

	void w(bool silent, Vector vAngs, CUserCmd* pCommand);

	
	void MakeVector(Vector angle, Vector& vector);

	Vector calc_angle(Vector src, Vector dst);

	int GetBestTarget(CBaseEntity* pLocal, CUserCmd* pCommand);

	int GetBestHitbox(CBaseEntity* pLocal, CBaseEntity* pEntity);
	
	bool CanAmbassadorHeadshot(CBaseEntity* pLocal);


};

extern CAimbot gAim; //dsajkhfdlksahf