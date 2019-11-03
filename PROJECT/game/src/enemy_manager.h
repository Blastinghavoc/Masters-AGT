#pragma once

#include "enemy.h"
#include "grid/grid.h"

//Static class to manage the movement of enemies in the level
class enemy_manager {
public:
	static void init(engine::ref<grid> level_grid);

	static void on_update(engine::timestep time_step);

	static enemy& spawn_minion(glm::vec3 position);

	static void render(const engine::ref<engine::shader>& shader);
private:
	static std::map<int,enemy> s_minions;
	static int s_next_id;
	static engine::ref<grid> s_level_grid;
};