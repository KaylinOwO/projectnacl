#pragma once
#include "SDK.h"

struct backtracking_record {
	CBaseEntity* entity;
	Vector head_position;
};

struct position_adjustment_data {
	int tick_count;
	std::vector<backtracking_record> records;
};

struct tick_record {
	tick_record(float simulationtime, Vector headposition) {
		simulation_time = simulationtime;
		head_position = headposition;
	}

	float simulation_time = -1;
	Vector head_position;
	matrix3x4 bone_matrix[128];
};

namespace global {
	extern CBaseEntity*	local_player;
	extern CUserCmd*		command;
}

namespace position_adjustment {
	extern std::vector<tick_record> ticks[64];
	extern void position_adjustment_start();
	extern void position_adjustment_calc();
	extern void do_position_adjustment();
};