#pragma once

#include "enemy.h"
#include "grid/grid.h"
#include <deque>
#include "ai/pathfinder.h"
#include <set>
#include "gameplay/gameplay_manager.h"

//Static class to manage the movement of enemies in the level
class enemy_manager {
public:
	static void init(engine::ref<grid> level_grid);

	static void on_update(engine::timestep time_step);

	static enemy& spawn_minion(glm::vec3 position);

	static void begin_wave(int amnt,float spacing = 2.f);

	static void render(const engine::ref<engine::shader>& shader);

	static int current_active() { return (int)s_current_active_minions.size(); };
	static int remaining() { return s_current_wave_remaining; };
private:
	static std::map<int,enemy> s_minions;
	static int s_next_id;
	static engine::ref<grid> s_level_grid;
	static std::deque<glm::vec3> s_current_path;
	static int s_current_wave_remaining;
	static float s_interval_accumulator;
	static float s_current_wave_interval;
	static std::stack<int> s_minion_buffer;//Holds the ID of created but inactive minions.
	static std::set<int> s_current_active_minions;//Holds the IDs of minions currently alive
};
