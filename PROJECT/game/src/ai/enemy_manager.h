#pragma once

#include "../entities/abstract_enemy.h"
#include "../grid/grid.h"
#include <deque>
#include "../ai/pathfinder.h"
#include "../lighting/light_manager.h"
#include "../gameplay/wave_definition.h"

/*Static class to manage the movement of enemies in the level
Spawns enemies according to a wave_definition, and detects when enemies
die or reach the exit.
*/
class enemy_manager {
public:
	static void init(engine::ref<grid> level_grid);

	static void on_update(const engine::timestep& time_step);

	static void begin_wave(wave_definition wave_def);

	/*
	Two different rendering methods, because animated enemies need a different
	shader than static enemies.
	*/
	static void render_animated(const engine::ref<engine::shader>& shader);
	static void render_static(const engine::ref<engine::shader>& shader);

	//For debug purposes only
	static void render_trigger_boxes(const engine::ref<engine::shader>& shader);

	//Number of enemies currently alive, and remaining this wave.
	static int current_active() { return (int)s_current_active_minions.size(); };
	static int remaining() { return s_current_wave_remaining; };

	//Obtain a vector of the currently active enemies, sorted by distance to the exit
	static const std::vector<engine::ref<abstract_enemy>>& get_active_enemies() { return s_current_active_minions; };
private:
	//Map of enemy IDs to enemies. This is the definitve store of enemies.
	static std::map<int, engine::ref<abstract_enemy>> s_minions;

	//id to be assigned to next spawned minion.
	static int s_next_id;

	//Reference to the level grid, for pathfinding.
	static engine::ref<grid> s_level_grid;

	//Current path obtained from pathfinder
	static std::deque<glm::vec3> s_current_path;

	//Enemies remaining to spawn for the current wave
	static int s_current_wave_remaining;

	//Timer between spawning enemies
	static float s_interval_accumulator;
	static float s_current_wave_interval;

	//Holds pointers to the created but inactive minions of various types.
	static std::multimap<enemy_type,engine::ref<abstract_enemy>> s_minion_buffer;

	//Holds pointers to the minions currently alive
	static std::vector<engine::ref<abstract_enemy>> s_current_active_minions;

	//Holds the amount and type of enemies to spawn for a given wave.
	static std::deque<std::pair<int, enemy_type>> s_spawn_sequence;

	//Highlights the furthest forward enemy
	static engine::ref<engine::SpotLight> m_spot_light;

	//---private methods

	//Spawn a new minion of the desired type.
	static engine::ref<abstract_enemy> spawn_minion(enemy_type type,glm::vec3 position);
};
