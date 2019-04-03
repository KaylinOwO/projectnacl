#include "Backtrack.h"
#include "Misc.h"
#include "Util.h"
#include "Client.h"

namespace backtrack {
	std::vector<tick_record> backtrack::ticks[64];

	void backtrack::backtrack_start() {
		if (g.local == nullptr || g.cmd == nullptr) return;

		if (GAME_TF2)
		{
			CBaseCombatWeapon* local_weapon = g.local->GetActiveWeapon();
			if (!local_weapon) { return; }
		}

		for (int i = 0; i < gInts.Engine->GetMaxClients(); i++) {
			CBaseEntity* entity = reinterpret_cast<CBaseEntity*>(gInts.EntList->GetClientEntity(i));
			if (!entity || entity->IsDormant() || entity->GetLifeState() != LIFE_ALIVE) continue;
			if (gInts.Engine->GetAppId() != 320)
				if (entity->GetTeamNum() == g.local->GetTeamNum()) continue;

			int hitbox = 0;
			if (GAME_TF2 || GAME_HL2DM)
				hitbox = 0;
			else
				hitbox = 12;

			ticks[i].insert(ticks[i].begin(), tick_record{ entity->GetSimulationTime(), entity->GetHitboxPosition(hitbox) });

			if (ticks[i].size() > gMisc.backtrack_ticks.value) {
				ticks[i].pop_back();
			}
		}
	}

	void backtrack::backtrack_calc() {
		if (g.local == nullptr || g.cmd == nullptr) return;

		Vector view_direction, new_view_direction;
		gInts.Engine->GetViewAngles(view_direction);
		new_view_direction = Util->AngleVector(view_direction);

		if (GAME_TF2)
		{
			CBaseCombatWeapon* local_weapon = g.local->GetActiveWeapon();
			if (!local_weapon) { return; }
		}

		int best_target_index = -1; float best_field_of_view = FLT_MAX;
		for (int i = 0; i < gInts.Engine->GetMaxClients(); i++) {
			CBaseEntity* entity = reinterpret_cast<CBaseEntity*>(gInts.EntList->GetClientEntity(i));
			if (!entity || entity->IsDormant() || entity->GetLifeState() != LIFE_ALIVE) continue;
			if (gInts.Engine->GetAppId() != 320)
				if (entity->GetTeamNum() == g.local->GetTeamNum()) continue;

			if (ticks[i].empty()) continue;

			float field_of_view_distance = Util->DistPointToLine(entity->GetEyePosition(), g.local->GetEyePosition(), new_view_direction);
			if (field_of_view_distance < best_field_of_view) {
				best_field_of_view = field_of_view_distance;
				best_target_index = i;
			}

			for (unsigned int t = 0; t < ticks[i].size(); t++) {
				if (!entity->SetupBones(ticks[i].at(t).bone_matrix, 128, 256, 0)) {
					continue;
				}
			}
		}

		float final_target_index = -1;
		if (best_target_index != -1) {
			for (unsigned int i = 0; i < ticks[best_target_index].size(); i++) {
				float field_of_view_distance = Util->DistPointToLine(ticks[best_target_index].at(i).head_position, g.local->GetEyePosition(), new_view_direction);
				if (field_of_view_distance < best_field_of_view) {
					best_field_of_view = field_of_view_distance;
					final_target_index = ticks[best_target_index].at(i).simulation_time;
				}
			}

			if (final_target_index != -1) {
				if (g.cmd->buttons & IN_ATTACK) {
					g.cmd->tick_count = TIME_TO_TICKS(final_target_index);
				}
			}
		}
	}

	void backtrack::do_backtrack() {
		if (g.local == nullptr || g.cmd == nullptr || !gMisc.backtrack.value) {
			return;
		}

		backtrack_start();
		backtrack_calc();
	}
}