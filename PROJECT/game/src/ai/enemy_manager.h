#pragma once

#include "../entities/enemy.h"
#include "../grid/grid.h"
#include <deque>
#include "../ai/pathfinder.h"
#include "../gameplay/gameplay_manager.h"
#include "../lighting/light_manager.h"

//Static class to manage the movement of enemies in the level
class enemy_manager {
public:
	static void init(engine::ref<grid> level_grid);

	static void on_update(engine::timestep time_step);

	static enemy& spawn_minion(glm::vec3 position);

	static void begin_wave(int amnt,float spacing = 2.f);

	static void render(const engine::ref<engine::shader>& shader);

	//For debug purposes only
	static void render_trigger_boxes(const engine::ref<engine::shader>& shader);

	static int current_active() { return (int)s_current_active_minions.size(); };
	static int remaining() { return s_current_wave_remaining; };

	//Obtain a vector of the currently active enemies
	static std::vector<enemy*>& get_active_enemies() { return s_current_active_minions; };
private:
	static std::map<int,enemy> s_minions;
	static int s_next_id;
	static engine::ref<grid> s_level_grid;
	static std::deque<glm::vec3> s_current_path;
	static int s_current_wave_remaining;
	static float s_interval_accumulator;
	static float s_current_wave_interval;
	static std::stack<enemy*> s_minion_buffer;//Holds pointers to the created but inactive minions.
	static std::vector<enemy*> s_current_active_minions;//Holds pointers to the minions currently alive

	//Highlights the furthest forward enemy
	static engine::ref<engine::SpotLight> m_spot_light;
};
