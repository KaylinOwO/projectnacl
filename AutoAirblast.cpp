#include "AutoAirblast.h"
#include "Util.h"
#include "SDK.h"

CAutoAirblast gBlast;

int phlogID = 594;

Vector EstimateAbsVelocity(CBaseEntity *ent);
void AimAt(Vector origin, Vector target, CUserCmd *cmd, bool silent);
bool IsVectorVisible(Vector target, CBaseEntity* pLocal);
bool ShouldReflect(CBaseEntity *ent, int lTeamNum, const char* name);

void CAutoAirblast::Run(CBaseEntity* g_pLocalPlayer, CUserCmd* g_pUserCmd)
{
	// Check if user settings allow Auto Reflect
	if (!gTrigger.airblast.value)
		return;

	if (g_pLocalPlayer->GetLifeState() != LIFE_ALIVE)
		return;

	if (g_pLocalPlayer->GetClassNum() != TF2_Pyro)
		return;

	if (g_pLocalPlayer->GetActiveWeapon()->GetSlot() != 0)
		return;

	int iClassID = g_pLocalPlayer->GetActiveWeapon()->GetClientClass()->iClassID;

	// Check for phlogistinator, which is item 594
	if (iClassID == phlogID)
		return;

	// If user settings allow, return if local player is in attack
	//if (idle_only && (g_pUserCmd->buttons & IN_ATTACK))
	//	return;

	// Create some book-keeping vars
	float closest_dist = 0.0f;
	Vector closest_vec;
	// Loop to find the closest entity
	for (int i = 0; i < gInts.EntList->GetHighestEntityIndex(); i++)
	{

		// Find an ent from the for loops current tick
		CBaseEntity *ent = gInts.EntList->GetClientEntity(i);

		// Check if null or dormant
		if (!ent)
			continue;

		if (ent->IsDormant())
			continue;

		if (ent->GetTeamNum() == g_pLocalPlayer->GetTeamNum())
			continue;

		// Check if ent should be reflected
		if (!ShouldReflect(ent, g_pLocalPlayer->GetTeamNum(), ent->GetClientClass()->chName))
			continue;

		auto net = reinterpret_cast<INetChannelInfo*>(gInts.Engine->GetNetChannelInfo());

		float lInterp = gInts.cvar->FindVar("cl_interp_ratio")->GetFloat() / gInts.cvar->FindVar("cl_updaterate")->GetFloat() * gInts.globals->interval_per_tick;
		float latency = net->GetLatency(FLOW_OUTGOING) + net->GetLatency(FLOW_INCOMING);
		float scale = lInterp + latency;

		// Create a vector variable to store our velocity
		Vector velocity;
		// Grab Velocity of projectile
		velocity = EstimateAbsVelocity(ent);
		// Predict a vector for where the projectile will be
		Vector predicted_proj = ent->GetAbsOrigin() +(velocity * latency);
		;

		  // Calculate distance
		float dist = predicted_proj.DistToSqr(g_pLocalPlayer->GetAbsOrigin());

		// Compare our info to the others and determine if its the best, if we
		// dont have a projectile already, then we save it here
		if (dist < closest_dist || closest_dist == 0.0f)
		{
			closest_dist = dist;
			closest_vec = predicted_proj;
		}
	}

	// Determine whether the closest projectile is whithin our parameters,
	// preferably 185 units should be our limit, sqr is around the number below
	if (closest_dist == 0 || closest_dist > 34400)
		return;

	Vector previousAngles = g_pUserCmd->viewangles;

	if (gTrigger.airblastrage.value)
	{
		AimAt(g_pLocalPlayer->GetEyePosition(), closest_vec, g_pUserCmd, gCvars.autoairblast_silent);
	}

	// Airblast
	g_pUserCmd->buttons |= IN_ATTACK2;

	if (!gCvars.autoairblast_silent)
		g_pUserCmd->viewangles = previousAngles;

	// Function is finished, return
	return;
}

	Vector EstimateAbsVelocity(CBaseEntity *ent)
	{
		typedef void(__thiscall * EstimateAbsVelocityFn)(CBaseEntity *, Vector &);
	
		static DWORD dwFn = gSignatures.GetClientSignature("E8 ? ? ? ? F3 0F 10 4D ? 8D 85 ? ? ? ? F3 0F 10 45 ? F3 0F 59 C9 56 F3 0F 59 C0 F3 0F 58 C8 0F 2F 0D ? ? ? ? 76 07") + 0x1;
	
		static DWORD dwEstimate = ((*(PDWORD)(dwFn)) + dwFn + 4);
	
		EstimateAbsVelocityFn vel = (EstimateAbsVelocityFn)dwEstimate;
	
		Vector v;
		vel(ent, v);
	
		return v;
	}

	void AimAt(Vector origin, Vector target, CUserCmd *cmd, bool silent)
	{
		Vector angles, tr;
		tr = (target - origin);
		VectorAngles(tr, angles);
		ClampAngle(angles);
		cmd->viewangles = angles;
		if (!silent)
			gInts.Engine->SetViewAngles(cmd->viewangles);
	}

	bool IsVectorVisible(Vector target, CBaseEntity* pLocal)
	{
		trace_t trace_visible;
		CTraceFilter filter;
		Ray_t ray;

		filter.pSkip = pLocal;
		ray.Init(pLocal->GetAbsOrigin(), target);
		gInts.EngineTrace->TraceRay(ray, MASK_SHOT_HULL, &filter, &trace_visible);
		return (trace_visible.fraction == 1.0f);
	}

	bool ShouldReflect(CBaseEntity *ent, int lTeamNum, const char* name)
	{
		if (!strcmp(name, "CTFProjectile_SentryRocket")
			|| !strcmp(name, "CTFProjectile_Arrow")
			|| !strcmp(name, "CTFProjectile_HealingBolt")
			|| !strcmp(name, "CTFProjectile_EnergyBall")
			|| !strcmp(name, "CTFProjectile_Flare")
			|| !strcmp(name, "CTFProjectile_Jar")
			|| !strcmp(name, "CTFProjectile_JarMilk")
			|| !strcmp(name, "CTFProjectile_Cleaver")
			|| !strcmp(name, "CTFProjectile_Throwable")
			|| !strcmp(name, "CTFProjectile_Rocket")
			|| !strcmp(name, "CTFGrenadePipebombProjectile")
			|| !strcmp(name, "CTFProjectile_ThrowableRepel")
			|| !strcmp(name, "CTFProjectile_ThrowableBrick")
			|| !strcmp(name, "CTFProjectile_ThrowableBreadMonster")
			&& ent->GetTeamNum() != lTeamNum)
			return true;

		return false;
	}

