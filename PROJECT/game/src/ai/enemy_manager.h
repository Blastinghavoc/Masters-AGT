#pragma once

#include "../entities/abstract_enemy.h"
#include "../grid/grid.h"
#include <deque>
#include "../ai/pathfinder.h"
#include "../lighting/light_manager.h"
#include "../gameplay/wave_definition.h"

//Static class to manage the movement of enemies in the level
class enemy_manager {
public:
	static void init(engine::ref<grid> level_grid);

	static void on_update(const engine::timestep& time_step);

	static engine::ref<abstract_enemy> spawn_minion(enemy_type type,glm::vec3 position);

	static void begin_wave(wave_definition wave_def);

	static void render_animated(const engine::ref<engine::shader>& shader);

	static void render_static(const engine::ref<engine::shader>& shader);

	//For debug purposes only
	static void render_trigger_boxes(const engine::ref<engine::shader>& shader);

	static int current_active() { return (int)s_current_active_minions.size(); };
	static int remaining() { return s_current_wave_remaining; };

	//Obtain a vector of the currently active enemies
	static const std::vector<engine::ref<abstract_enemy>>& get_active_enemies() { return s_current_active_minions; };
private:
	static std::map<int, engine::ref<abstract_enemy>> s_minions;
	static int s_next_id;
	static engine::ref<grid> s_level_grid;
	static std::deque<glm::vec3> s_current_path;
	static int s_current_wave_remaining;
	static float s_interval_accumulator;
	static float s_current_wave_interval;
	//Holds pointers to the created but inactive minions of various types.
	static std::multimap<enemy_type,engine::ref<abstract_enemy>> s_minion_buffer;
	static std::vector<engine::ref<abstract_enemy>> s_current_active_minions;//Holds pointers to the minions currently alive
	//Holds the amount and type of enemies to spawn for a given wave.
	static std::deque<std::pair<int, enemy_type>> s_spawn_sequence;

	//Highlights the furthest forward enemy
	static engine::ref<engine::SpotLight> m_spot_light;
};
