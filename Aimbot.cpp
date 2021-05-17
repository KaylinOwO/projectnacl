#include "Aimbot.h"
#include "Util.h"
#include "Misc.h"
#include "Client.h"

bool CAimbot::CanAmbassadorHeadshot(CBaseEntity* pLocal)
{
	return gInts.globals->curtime - pLocal->GetActiveWeapon()->m_flLastFireTime() >= 0.930;//1.0;
}

CAimbot gAim;

Vector CAimbot::EstimateAbsVelocity(CBaseEntity* entity)
{
	typedef void(__thiscall * estimate_abs_velocity_fn)(CBaseEntity*, Vector &);
	static uintptr_t function = gSignatures.GetClientSignature("E8 ? ? ? ? F3 0F 10 4D ? 8D 85 ? ? ? ? F3 0F 10 45 ? F3 0F 59 C9 56 F3 0F 59 C0 F3 0F 58 C8 0F 2F 0D ? ? ? ? 76 07") + 0x1;
	static uintptr_t estimate = ((*(PDWORD)(function)) + function + 4);

	estimate_abs_velocity_fn vel = (estimate_abs_velocity_fn)estimate;
	Vector v; vel(entity, v); return v;
}

void CAimbot::Projectile(CBaseEntity* local_player, CBaseEntity* entity, CBaseCombatWeapon* local_weapon, Vector& vec_hitbox)
{
	auto item_index = local_weapon->GetItemDefinitionIndex();
	auto get_speed = [&local_player, &local_weapon, &entity, &item_index]() -> float {
		auto weapon_speed = 0.0f;
		switch (item_index) {
		case WPN_DirectHit:
			weapon_speed = 1980.0f; break;
		case WPN_BotRocketlauncherB:
		case WPN_BotRocketlauncherC:
		case WPN_BotRocketlauncherD:
		case WPN_BotRocketlauncherEG:
		case WPN_BotRocketlauncherES:
		case WPN_BotRocketlauncherG:
		case WPN_BotRocketlauncherR:
		case WPN_BotRocketlauncherS:
		case WPN_FestiveRocketLauncher:
		case WPN_NewRocketLauncher:
		case WPN_RocketLauncher:
		case WPN_Original:
		case WPN_Airstrike:
		case WPN_BlackBox:
			weapon_speed = 1100.0f; break;
		case WPN_FestiveFlaregun:
		case WPN_Flaregun:
			weapon_speed = 2000.0f; break;
		case WPN_SyringeGun:
		case WPN_NewSyringeGun:
		case WPN_Blutsauger:
		case WPN_Overdose:
			weapon_speed = 1000.0f; break;
		case WPN_RescueRanger:
		case WPN_Crossbow:
		case WPN_FestiveCrossbow:
			weapon_speed = 2400.0f; break;
		case WPN_GrenadeLauncher:
		case WPN_NewGrenadeLauncher:
		case WPN_FestiveGrenadeLauncher:
			weapon_speed = 1216.6f; break;
		case WPN_LochNLoad:
			weapon_speed = 1513.3f; break;
		case WPN_LoooseCannon:
			weapon_speed = 1453.9f; break;
		case WPN_IronBomber:
			weapon_speed = 1216.6f; break;
		default: weapon_speed = 0.0f; break;
		}
		return weapon_speed;
	};
	auto get_gravity = [&local_player, &local_weapon, &entity, &item_index]() -> float {
		auto weapon_gravity = 0.0f;
		switch (item_index) {
		case WPN_RescueRanger:
		case WPN_Crossbow:
		case WPN_FestiveCrossbow:
			weapon_gravity = 0.2f; break;
		case WPN_GrenadeLauncher:
		case WPN_NewGrenadeLauncher:
		case WPN_FestiveGrenadeLauncher:
		case WPN_LoooseCannon:
		case WPN_LochNLoad:
		case WPN_IronBomber:
			weapon_gravity = 0.4f; break;
		default: weapon_gravity = 0.0f; break;
		}
		return weapon_gravity;
	};

	auto distance_to_ground = [&entity]() -> float {
		if (entity->GetFlags() & FL_ONGROUND) return 0.0f;
		auto distance_to_ground = [&entity](Vector origin) -> float
		{
			trace_t ground_trace; Ray_t ray;
			CTraceFilter filter; filter.pSkip = entity;
			Vector endpos = origin;

			endpos.z -= 8192;
			ray.Init(origin, endpos);
			gInts.EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &ground_trace);
			return 8192.0f * ground_trace.fraction;
		};

		Vector origin = entity->GetAbsOrigin();
		float v1 = distance_to_ground(origin + Vector(10.0f, 10.0f, 0.0f));
		float v2 = distance_to_ground(origin + Vector(-10.0f, 10.0f, 0.0f));
		float v3 = distance_to_ground(origin + Vector(10.0f, -10.0f, 0.0f));
		float v4 = distance_to_ground(origin + Vector(-10.0f, -10.0f, 0.0f));
		return min(v1, min(v2, min(v3, v4)));
	};
	if (IsProjectileWeapon(local_player, local_weapon)) {
		bool on_ground = entity->GetFlags() & FL_ONGROUND;
		if (local_player->GetClassNum() == TF2_Medic || local_player->GetClassNum() == TF2_Engineer || local_player->GetClassNum() == TF2_Pyro)
			vec_hitbox = entity->GetHitboxPosition(4);
		else if (local_player->GetClassNum() == TF2_Soldier || local_player->GetClassNum() == TF2_Demoman) {
			vec_hitbox = entity->GetAbsOrigin();
			vec_hitbox[2] += 10.0f;
		}

		auto hitbox_pred = [&local_player, &entity, &on_ground](Vector hitbox, float speed, float gravity, float distance_to_ground) -> Vector {
			Vector result = hitbox, bestpos = result;

			auto vec_velocity = gAim.EstimateAbsVelocity(entity);
			auto medianTime = (local_player->GetEyePosition().DistTo(result) / speed), range = 1.5f,
				currenttime = medianTime - range;

			if (currenttime <= 0.0f) currenttime = 0.01f;

			auto besttime = currenttime, mindelta = 65536.0f; auto maxsteps = 300;
			for (int steps = 0; steps < maxsteps; steps++, currenttime += ((float)(2 * range) / (float)maxsteps)) {
				Vector curpos = result; curpos += vec_velocity * currenttime;

				if (distance_to_ground > 0.0f) {
					curpos.z -= currenttime * currenttime * 400.0f * ((entity->GetCond() & TFCondEx2_Parachute) ? 0.448f : 1.0f);
					if (curpos.z < result.z - distance_to_ground) {
						curpos.z = result.z - distance_to_ground;
					}
				}

				auto rockettime = (local_player->GetEyePosition().DistTo(curpos) / speed);
				if (fabs(rockettime - currenttime) < mindelta) {
					besttime = currenttime; bestpos = curpos;
					mindelta = fabs(rockettime - currenttime);
				}
			}
			bestpos.z += (400 * besttime * besttime * gravity);
			return bestpos;
		};

		vec_hitbox = hitbox_pred(vec_hitbox, get_speed(), get_gravity(), distance_to_ground());
	}
}

void FixMove(CUserCmd* pCmd, Vector m_vOldAngles, float m_fOldForward, float m_fOldSidemove)
{
	float deltaView = pCmd->viewangles.y - m_vOldAngles.y;
	float f1;
	float f2;

	if (m_vOldAngles.y < 0.f)
		f1 = 360.0f + m_vOldAngles.y;
	else
		f1 = m_vOldAngles.y;

	if (pCmd->viewangles.y < 0.0f)
		f2 = 360.0f + pCmd->viewangles.y;
	else
		f2 = pCmd->viewangles.y;

	if (f2 < f1)
		deltaView = abs(f2 - f1);
	else
		deltaView = 360.0f - abs(f1 - f2);
	deltaView = 360.0f - deltaView;

	pCmd->forwardmove = cos(DEG2RAD(deltaView)) * m_fOldForward + cos(DEG2RAD(deltaView + 90.f)) * m_fOldSidemove;
	pCmd->sidemove = sin(DEG2RAD(deltaView)) * m_fOldForward + sin(DEG2RAD(deltaView + 90.f)) * m_fOldSidemove;
}

Vector CAimbot::calc_angle(Vector src, Vector dst)
{
	Vector AimAngles, delta;
	float hyp;
	delta = src - dst;
	hyp = sqrtf((delta.x * delta.x) + (delta.y * delta.y)); //SUPER SECRET IMPROVEMENT CODE NAME DONUT STEEL
	AimAngles.x = atanf(delta.z / hyp) * RADPI;
	AimAngles.y = atanf(delta.y / delta.x) * RADPI;
	AimAngles.z = 0.0f;
	if (delta.x >= 0.0)
		AimAngles.y += 180.0f;
	return AimAngles;
}
void CAimbot::MakeVector(Vector angle, Vector& vector)
{
	float pitch, yaw, tmp;
	pitch = float(angle[0] * PI / 180);
	yaw = float(angle[1] * PI / 180);
	tmp = float(cos(pitch));
	vector[0] = float(-tmp * -cos(yaw));
	vector[1] = float(sin(yaw)*tmp);
	vector[2] = float(-sin(pitch));
}
float CAimbot::GetFOV(Vector angle, Vector src, Vector dst)
{
	Vector ang, aim;
	float mag, u_dot_v;
	ang = calc_angle(src, dst);


	MakeVector(angle, aim);
	MakeVector(ang, ang);

	mag = sqrtf(pow(aim.x, 2) + pow(aim.y, 2) + pow(aim.z, 2));
	u_dot_v = aim.Dot(ang);

	return RAD2DEG(acos(u_dot_v / (pow(mag, 2))));
}

void CAimbot::Run(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	Vector m_vOldViewAngle = pCommand->viewangles;
	float m_fOldSideMove = pCommand->sidemove;
	float m_fOldForwardMove = pCommand->forwardmove;

	gCvars.iAimbotIndex = -1;



	if (!enabled.value)
		return;

	if (!Util->IsKeyPressed(key.value))
		return;

	if (!pLocal->IsAlive())
		return;

	if (GAME_TF2)
	{
		if (!pLocal->GetActiveWeapon())
			return;
	}
	else
	{
		if (!pLocal->GetActiveWeaponOther())
			return;
	}

	if (GAME_TF2)
	{
		auto id = pLocal->GetActiveWeapon()->GetItemDefinitionIndex(); //This ignores all projectile weapons, doesn't work for strange/killstreak/etc
		if (
#pragma region scout
			id == (int)scoutweapons::WPN_Sandman
			|| id == (int)scoutweapons::WPN_CritCola
			|| id == (int)scoutweapons::WPN_CritCola
			|| id == (int)scoutweapons::WPN_FlyingGuillotine1
			|| id == (int)scoutweapons::WPN_FlyingGuillotine2
			|| id == (int)scoutweapons::WPN_Milk
			|| id == (int)scoutweapons::WPN_MutatedMilk
			|| id == (int)scoutweapons::WPN_WrapAssassin
#pragma endregion
#pragma region pyro
			|| id == (int)pyroweapons::WPN_Detonator
			//	|| id == (int)pyroweapons::WPN_FestiveFlaregun
			//	|| id == (int)pyroweapons::WPN_Flaregun
			|| id == (int)pyroweapons::WPN_ScorchShot
#pragma endregion
#pragma region demo
			//|| id == (int)demomanweapons::WPN_FestiveGrenadeLauncher
			//|| id == (int)demomanweapons::WPN_FestiveStickyLauncher
			//|| id == (int)demomanweapons::WPN_GrenadeLauncher
			//|| id == (int)demomanweapons::WPN_IronBomber
			//|| id == (int)demomanweapons::WPN_LochNLoad
			//|| id == (int)demomanweapons::WPN_LoooseCannon
			//|| id == (int)demomanweapons::WPN_QuickieBombLauncher
			//|| id == (int)demomanweapons::WPN_StickyLauncher
			|| id == (int)demomanweapons::WPN_ScottsSkullctter
#pragma endregion
#pragma region heavy
			|| id == (int)heavyweapons::WPN_FestiveSandvich
			|| id == (int)heavyweapons::WPN_RobotSandvich
			|| id == (int)heavyweapons::WPN_Sandvich
			|| id == (int)heavyweapons::WPN_Steak
			|| id == (int)heavyweapons::WPN_CandyBar
			|| id == (int)heavyweapons::WPN_Fishcake
#pragma endregion
#pragma region engineer
			|| id == (int)engineerweapons::WPN_Builder
			|| id == (int)engineerweapons::WPN_Builder2
			|| id == (int)engineerweapons::WPN_Destructor
			|| id == (int)engineerweapons::WPN_FestiveWrangler
			|| id == (int)engineerweapons::WPN_Pomson
			//|| id == (int)engineerweapons::WPN_RescueRanger
			|| id == (int)engineerweapons::WPN_ShortCircut
			|| id == (int)engineerweapons::WPN_Toolbox
			|| id == (int)engineerweapons::WPN_Wrangler
#pragma endregion
#pragma region medic
			//	|| id == (int)medicweapons::WPN_Blutsauger
			|| id == (int)medicweapons::WPN_FestiveCrossbow
			|| id == (int)medicweapons::WPN_FestiveMedigun
			|| id == (int)medicweapons::WPN_Kritzkrieg
			|| id == (int)medicweapons::WPN_Medigun
			//	|| id == (int)medicweapons::WPN_Overdose
			|| id == (int)medicweapons::WPN_QuickFix
			//|| id == (int)medicweapons::WPN_SyringeGun
			|| id == (int)medicweapons::WPN_Vaccinator

#pragma endregion
#pragma region sniper
			|| id == (int)sniperweapons::WPN_CompoundBow
			|| id == (int)sniperweapons::WPN_FestiveJarate
			|| id == (int)sniperweapons::WPN_FestiveHuntsman
			|| id == (int)sniperweapons::WPN_Huntsman
			|| id == (int)sniperweapons::WPN_Jarate
#pragma endregion
#pragma region spy
			|| id == (int)spyweapons::WPN_ApSap
			|| id == (int)spyweapons::WPN_DisguiseKit
			|| id == (int)spyweapons::WPN_RedTape1
			|| id == (int)spyweapons::WPN_RedTape2
			|| id == (int)spyweapons::WPN_Sapper
			|| id == (int)spyweapons::WPN_Sapper2
			|| id == (int)spyweapons::WPN_FestiveSapper
#pragma endregion
			)
			return;
	}



	CBaseEntity* pEntity = GetBaseEntity(GetBestTarget(pLocal, pCommand));

	if (!pEntity)
		return;

	int iBestHitbox = GetBestHitbox(pLocal, pEntity);

	if (iBestHitbox == -1)
		return;

	if (GAME_TF2)
	{
		auto pWep = pLocal->GetActiveWeapon();
		if (waitforcharge.value) //Advanced Ambassador Wait For Charge!
			if (pWep->GetItemDefinitionIndex() == spyweapons::WPN_Ambassador || pWep->GetItemDefinitionIndex() == spyweapons::WPN_FestiveAmbassador)
				if (!CanAmbassadorHeadshot(pLocal)) return;
	}

	if (iBestHitbox == -1)
		return;
	Vector vEntity = pEntity->GetHitboxPosition(iBestHitbox);

	Vector vLocal = pLocal->GetEyePosition();

	Vector vAngs;

	if (GAME_TF2)
	{
		CBaseCombatWeapon* pWeapon = pLocal->GetActiveWeapon();
		Projectile(pLocal, pEntity, pWeapon, vEntity);
	}

	VectorAngles((vEntity - vLocal), vAngs);

	ClampAngle(vAngs);

	gCvars.iAimbotIndex = pEntity->GetIndex();

	if (smooth.value > 0.0 && !silent.value)
	{
		Vector vDelta(pCommand->viewangles - vAngs);
		AngleNormalize(vDelta);
		vAngs = pCommand->viewangles - vDelta / smooth.value;
	}

	if (Autoshoot.value)
	{
		pCommand->buttons |= IN_ATTACK;
		if (GAME_CSS && gAim.autopistol.value)
			gMisc.AutoPistol(pLocal, pCommand);
	}

	if ((gMisc.CanShoot() && (pCommand->buttons & IN_ATTACK)) || (smooth.value > 0.0 && !silent.value)) {
		if (GAME_TF2)
		{
			pCommand->viewangles = vAngs;
			if (!silent.value) gInts.Engine->SetViewAngles(pCommand->viewangles);
			g.silenttime = true;
		}
		else
		{
			Vector AimPunch = pLocal->GetVecPunchAngle(); //Best hope the game you're playing has m_vecPunchAngle lol
			pCommand->viewangles = gAim.antirecoil.value ? vAngs - (AimPunch * 2.f) : vAngs;
			if (!silent.value) gInts.Engine->SetViewAngles(pCommand->viewangles);
		}
	}

	FixMove(pCommand, m_vOldViewAngle, m_fOldForwardMove, m_fOldSideMove);
}

int CAimbot::GetBestTarget(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	int iBestTarget = -1;
	int bestTarget = -1;					 //this num could be smaller 
	float flDistToBest = 99999.f;
	double minimalDistance = 99999.0;

	Vector vLocal = pLocal->GetEyePosition();

	for (int i = 1; i <= gInts.Engine->GetMaxClients(); i++)
	{
		if (i == me)
			continue;

		CBaseEntity* pEntity = GetBaseEntity(i);

		if (!pEntity)
			continue;

		if (pEntity->IsDormant())
			continue;

		if (pEntity->GetLifeState() != LIFE_ALIVE)
			continue;

		if (!aimatteammates.value && pEntity->GetTeamNum() == pLocal->GetTeamNum())
			continue;

		int iBestHitbox = GetBestHitbox(pLocal, pEntity);

		if (iBestHitbox == -1)
			continue;

		Vector vEntity = pEntity->GetHitboxPosition(iBestHitbox);

		if (!gCvars.PlayerMode[i])
			continue;


		if (GAME_TF2)
		{
			auto weap = pLocal->GetActiveWeapon()->GetItemDefinitionIndex();

			if (pEntity->GetCond() & TFCond_Ubercharged ||
				pEntity->GetCond() & TFCond_UberchargeFading ||
				pEntity->GetCond() & TFCond_Bonked)
				continue;

			if (pEntity->GetCond() & TFCond_Cloaked && ignorecloaked.value)
				continue;

			if (pEntity->GetCond() & TFCond_Disguised && ignoredisguised.value)
				continue;

			auto pWep = pLocal->GetActiveWeapon();
			auto pClass = pWep->GetItemDefinitionIndex();
			auto urmomgay = pClass == demomanweapons::WPN_Sword || pClass == demomanweapons::WPN_FestiveEyelander || pClass == demomanweapons::WPN_Golfclub || pClass == demomanweapons::WPN_ScottsSkullctter || pClass == demomanweapons::WPN_Headless;
			if (pWep->GetSlot() == 2 && !urmomgay)
				minimalDistance = 8.4;

			if (pClass == demomanweapons::WPN_Sword || pClass == demomanweapons::WPN_FestiveEyelander || pClass == demomanweapons::WPN_Golfclub || pClass == demomanweapons::WPN_ScottsSkullctter || pClass == demomanweapons::WPN_Headless)
				minimalDistance = 12.0;

			if (pLocal->szGetClass() == "Pyro" && (pClass == pyroweapons::WPN_Backburner || pClass == pyroweapons::WPN_Degreaser || pClass == pyroweapons::WPN_FestiveBackburner || pClass == pyroweapons::WPN_FestiveFlamethrower || pClass == pyroweapons::WPN_Flamethrower || pClass == pyroweapons::WPN_Phlogistinator || pClass == pyroweapons::WPN_Rainblower))
				minimalDistance = 17.0;

			if (pLocal->szGetClass() == "Sniper" && pLocal->GetActiveWeapon()->GetSlot() == 0)
			{
				float damage = pLocal->GetActiveWeapon()->GetChargeDamage();
				if (zoomedonly.value)// || gCvars.aimbot.waitforcharge)
				{
					if (!(pLocal->GetCond() & tf_cond::TFCond_Zoomed))
						return -1;

					if (damage < 10.f)
						return -1;


				}
				if (waitforcharge.value) //This isn't the best code but it works for raging.
					if (waitforcharge.value && ZOOM_BASE_DAMAGE + damage < pEntity->GetHealth())//<
						return -1;
			}
		}
		else
		{
			CBaseCombatWeapon *pWeapon = pLocal->GetActiveWeaponOther();
			if (!pWeapon->HasAmmo())
				return -1;
		}

		float flFOV = GetFOV(pCommand->viewangles, vLocal, vEntity);
		float distance = Util->flGetDistance(vEntity, pLocal->GetEyePosition());

		if (distance < minimalDistance)//gCvars.aimbot.fov)
		{
			if (flFOV < flDistToBest && flFOV < fov.value)
			{
				if (gCvars.PlayerMode[i] == 2)
					return i;
				//flDistToBest = flDistToTarget;
				flDistToBest = flFOV;
				gCvars.iAimbotIndex = i;
				bestTarget = i;
			}
		}

		if (gCvars.PlayerMode[i] == 2) //always aim at rage targets first
			return i;
	}


	return bestTarget;
}

int CAimbot::GetBestHitbox(CBaseEntity* pLocal, CBaseEntity* pEntity)
{
	int iBestHitbox = -1;

	if (GAME_TF2)
	{
		if (!hitbox.value)
		{
			if (Util->IsHeadshotWeapon(pLocal, pLocal->GetActiveWeapon()))
				iBestHitbox = 0;
			else
				iBestHitbox = 4;
			for (int i = iBestHitbox; i < 17; i++) // int i equals prioritized hitbux, so above we check the weapon so it prioritizes the proper hitbox.
			{
				if (Util->IsVisible(pLocal, pEntity, pLocal->GetEyePosition(), pEntity->GetHitboxPosition(i)))
					return i;
			}
		}
		else if (hitbox.value == 1)
			iBestHitbox = 0;
		else if (hitbox.value == 2)
			iBestHitbox = 4;
	}
	else if (GAME_HL2DM)
	{
		if (!hitbox.value)
		{
			iBestHitbox = 4;
			for (int i = iBestHitbox; i < 17; i++) // int i equals prioritized hitbux, so above we check the weapon so it prioritizes the proper hitbox.
			{
				if (Util->IsVisible(pLocal, pEntity, pLocal->GetEyePosition(), pEntity->GetHitboxPosition(i)))
					return i;
			}
		}
		else if (hitbox.value == 1)
			iBestHitbox = 0;
		else if (hitbox.value == 2)
			iBestHitbox = 4;
	}
	else
	{
		if (!hitbox.value)
		{
			if (GAME_CSS)
				iBestHitbox = 12;
			else
				iBestHitbox = 10;
			for (int i = iBestHitbox; i < 17; i++) // int i equals prioritized hitbux, so above we check the weapon so it prioritizes the proper hitbox.
			{
				if (Util->IsVisible(pLocal, pEntity, pLocal->GetEyePosition(), pEntity->GetHitboxPosition(i)))
					return i;
			}
		}
		else if (hitbox.value == 1)
			iBestHitbox = 11;
		else if (hitbox.value == 2)
			iBestHitbox = 10;
	}

	if (pEntity->GetHitboxPosition(iBestHitbox).IsZero())
		return -1;

	if (!Util->IsVisible(pLocal, pEntity, pLocal->GetEyePosition(), pEntity->GetHitboxPosition(iBestHitbox)))
		return -1;

	return iBestHitbox;
}
