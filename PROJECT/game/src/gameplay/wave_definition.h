#pragma once
#include <deque>
#include "../entities/enemy_type.h"

/*
Struct containing important information
about a "wave" of enemies
*/
struct wave_definition {
	int num_enemies;
	float enemy_spacing;
	int prep_time;
	std::deque<std::pair<int, enemy_type>> enemies;
};
