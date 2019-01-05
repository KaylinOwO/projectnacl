#include "Sticky.h"
#include "Util.h"
#include "SDK.h"
#include "Triggerbot.h"

CAutoSticky gSticky;

bool IsBombVisible(CBaseEntity* first, CBaseEntity* second) {
	trace_t trace; Ray_t ray; CTraceFilter filter;

	Vector first_origin = first->GetAbsOrigin(),
		second_origin = second->GetAbsOrigin();
	second_origin[2] += 10.0f;

	filter.pSkip = first;
	ray.Init(first_origin, second_origin);
	gInts.EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);

	return (trace.m_pEnt == second);
}

bool HasCond(CBaseEntity* entity, int condition) {
	if (entity->GetCond() & condition) {
		return true;
	}
	return false;
}

void CAutoSticky::Run(CBaseEntity* g_pLocalPlayer, CUserCmd* g_pUserCmd)
{
	if (!gTrigger.stickydetonate.value)
		return;

	auto IsLocalGrenade = [&](CBaseEntity* entity) 
	{
		if (entity->GetClientClass()->iClassID != 216) return false;
		if (entity->GetPipeType() != 1) return false;

		auto get_thrower = entity->GetThrower();
		if (!get_thrower) return false;

		auto entity_handle = (CBaseEntity*)gInts.EntList2->GetClientEntityFromHandle(get_thrower);
		if (!entity_handle) return false;

		if (entity_handle != g_pLocalPlayer) return false;
		return true;
	};

	for (int i = 0; i < gInts.EntList2->GetHighestEntityIndex(); i++) 
	{
		CBaseEntity* first_entity = reinterpret_cast<CBaseEntity*>(gInts.EntList2->GetClientEntity(i));
		if (!first_entity) {
			continue;
		}

		Vector sticky_position = first_entity->get_world_space_center();
		if (IsLocalGrenade(first_entity)) {
			for (int player = 0; player < gInts.Engine->GetMaxClients(); player++) {
				if (i == g_pLocalPlayer->GetIndex()) { continue; }
				CBaseEntity* second_entity = reinterpret_cast<CBaseEntity*>(gInts.EntList2->GetClientEntity(player));
				if (!second_entity || second_entity->IsDormant() || second_entity->IsDead()) {
					continue;
				}
				if (second_entity->GetTeamNum() == g_pLocalPlayer->GetTeamNum()) {
					continue;
				}

				if (HasCond(second_entity, 32) || HasCond(second_entity, 16384) ||
					HasCond(second_entity, 16) || HasCond(second_entity, 8192)) {
					continue;
				}

				auto get_distance = [&](Vector to, Vector from) -> float {
					Vector delta = to - from;
					float distance = ::sqrt(delta.length());

					if (distance < 1.0f) {
						distance = 1.0f;
					}

					return distance;
				};

				auto is_bomb_visible = [&](CBaseEntity* first, CBaseEntity* second) {
					trace_t tr; Ray_t ray; CTraceFilter filter;

					auto first_origin = first->GetAbsOrigin(),
						second_origin = second->GetAbsOrigin();

					filter.pSkip = first;
					ray.Init(first_origin, second_origin);
					gInts.EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

					return (tr.m_pEnt == second);
				};

				if (get_distance(sticky_position, second_entity->GetAbsOrigin()) < 12.0f) {
					if (is_bomb_visible(first_entity, second_entity)) {
						g_pUserCmd->buttons |= IN_ATTACK2;
					}
				}
			}
		}
	}
}