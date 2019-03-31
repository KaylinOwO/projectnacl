#pragma once
#include "SDK.h"

struct backtracking_record {
	CBaseEntity* entity;
	Vector head_position;
};

struct backtrack_data {
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

namespace backtrack {
	extern std::vector<tick_record> ticks[64];
	extern void backtrack_start();
	extern void backtrack_calc();
	extern void do_backtrack();
};