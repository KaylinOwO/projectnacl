#include "Backtrack.h"


namespace global {
	CBaseEntity*	global::local_player;
	CUserCmd*		global::command;
}

bool is_projectile(int item_definition_index, bool flamethrower_check) {
	bool is_projectile_weapon = false;
	switch (item_definition_index) {
	case WPN_DirectHit:
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
	case WPN_RocketJumper:
	case WPN_Original:
	case WPN_Airstrike:
	case WPN_BlackBox:
	case WPN_FestiveFlaregun:
	case WPN_Flaregun:
	case WPN_SyringeGun:
	case WPN_NewSyringeGun:
	case WPN_Blutsauger:
	case WPN_Overdose:
	case WPN_RescueRanger:
	case WPN_Crossbow:
	case WPN_FestiveCrossbow:
	case WPN_GrenadeLauncher:
	case WPN_NewGrenadeLauncher:
	case WPN_FestiveGrenadeLauncher:
	case WPN_LochNLoad:
	case WPN_LoooseCannon:
	case WPN_IronBomber:
	case WPN_StickyLauncher:
	case WPN_StickyJumper:
	case WPN_BotStickyB:
	case WPN_BotStickyC:
	case WPN_BotStickyD:
	case WPN_BotStickyEG:
	case WPN_BotStickyES:
	case WPN_BotStickyG:
	case WPN_BotStickyR:
	case WPN_BotStickyS:
	case WPN_NewStickyLauncher:
	case WPN_Huntsman:
	case WPN_FestiveHuntsman:
	case WPN_CompoundBow:
		return true;
	default: break;
	}

	if (flamethrower_check) {
		switch (item_definition_index) {
		case WPN_FestiveFlamethrower:
		case WPN_Flamethrower:
		case WPN_NewFlamethrower:
		case WPN_BotFlamethrowerB:
		case WPN_BotFlamethrowerC:
		case WPN_BotFlamethrowerD:
		case WPN_BotFlamethrowerEG:
		case WPN_BotFlamethrowerES:
		case WPN_BotFlamethrowerG:
		case WPN_BotFlamethrowerR:
		case WPN_BotFlamethrowerS:
			return true;
		default: break;
		}
	}

	return false;
}

Vector angle_vector(Vector meme) {
	auto sy = sin(meme.y / 180.f * static_cast<float>(PI));
	auto cy = cos(meme.y / 180.f * static_cast<float>(PI));
	auto sp = sin(meme.x / 180.f * static_cast<float>(PI));
	auto cp = cos(meme.x / 180.f* static_cast<float>(PI));
	return Vector(cp * cy, cp * sy, -sp);
}

float distance_point_to_line(Vector point, Vector origin, Vector direction) {
	auto point_direction = point - origin;

	auto temporary_offset = point_direction.dot(direction) / (direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
	if (temporary_offset < 0.000001f) {
		return FLT_MAX;
	}

	auto perpendicular_point = origin + (direction * temporary_offset);

	return (point - perpendicular_point).length();
}

namespace position_adjustment {
	std::vector<tick_record> position_adjustment::ticks[64];
	void position_adjustment::position_adjustment_start() {
		if (global::local_player == nullptr || global::command == nullptr) return;

		CBaseCombatWeapon* local_weapon = global::local_player->GetActiveWeapon();
		if (!local_weapon) { return; }

		for (int i = 0; i < gInts.Engine->GetMaxClients(); i++) {
			CBaseEntity* entity = reinterpret_cast<CBaseEntity*>(gInts.EntList->GetClientEntity(i));
			if (!entity || entity->IsDormant() || entity->GetLifeState() != LIFE_ALIVE) continue;
			if (entity->GetTeamNum() == global::local_player->GetTeamNum()) continue;

			// Quick projectile check, no need to do it as it doesn't work.
			if (is_projectile(local_weapon->GetItemDefinitionIndex(), true)) { continue; }

			ticks[i].insert(ticks[i].begin(), tick_record{ entity->GetSimulationTime(), entity->GetHitboxPosition(0) });

			if (ticks[i].size() > 12) {
				ticks[i].pop_back();
			}
		}
	}
	void position_adjustment::position_adjustment_calc() {
		if (global::local_player == nullptr || global::command == nullptr) return;

		Vector view_direction, new_view_direction;
		gInts.Engine->GetViewAngles(view_direction);
		new_view_direction = angle_vector(view_direction);

		int best_target_index = -1; float best_field_of_view = FLT_MAX;
		for (int i = 0; i < gInts.Engine->GetMaxClients(); i++) {
			CBaseEntity* entity = reinterpret_cast<CBaseEntity*>(gInts.EntList->GetClientEntity(i));
			if (!entity || entity->IsDormant() || entity->GetLifeState() != LIFE_ALIVE) continue;
			if (entity->GetTeamNum() == global::local_player->GetTeamNum()) continue;

			if (ticks[i].empty()) continue;

			float field_of_view_distance = distance_point_to_line(entity->GetEyePosition(), global::local_player->GetEyePosition(), new_view_direction);
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
				float field_of_view_distance = distance_point_to_line(ticks[best_target_index].at(i).head_position, global::local_player->GetEyePosition(), new_view_direction);
				if (field_of_view_distance < best_field_of_view) {
					best_field_of_view = field_of_view_distance;
					final_target_index = ticks[best_target_index].at(i).simulation_time;
				}
			}

			if (final_target_index != -1) {
				if (global::command->buttons & IN_ATTACK) {
					global::command->tick_count = TIME_TO_TICKS(final_target_index);
				}
			}
		}
	}
	void position_adjustment::do_position_adjustment() {
		if (global::local_player == nullptr || global::command == nullptr || !gCvars.misc_backtrack) {
			return;
		}

		position_adjustment_start();
		position_adjustment_calc();
	}
};