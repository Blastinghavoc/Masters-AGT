#include "enemy_manager.h"

//Static initializers
std::map<int,enemy> enemy_manager::s_minions;
int enemy_manager::s_next_id = 0;
engine::ref<grid> enemy_manager::s_level_grid{};
std::deque<glm::vec3> enemy_manager::s_current_path{};
int enemy_manager::s_current_wave_remaining = 0;
float enemy_manager::s_interval_accumulator = 0;
float enemy_manager::s_current_wave_interval = 2.f;

void enemy_manager::init(engine::ref<grid> level_grid)
{
	s_level_grid = level_grid;
}

void enemy_manager::on_update(engine::timestep time_step)
{
	if (s_current_wave_remaining > 0)
	{
		s_interval_accumulator += time_step;
		if (s_interval_accumulator > s_current_wave_interval)
		{

			auto& new_minion = spawn_minion(s_current_path.front());
			new_minion.set_path(s_current_path);
			s_interval_accumulator -= s_current_wave_interval;
			--s_current_wave_remaining;
		}
	}


	for (auto& pair : s_minions) {
		pair.second.on_update(time_step);
	}
}

enemy& enemy_manager::spawn_minion(glm::vec3 position)
{
	s_minions[s_next_id] = enemy(s_next_id, position);
	return s_minions[s_next_id++];
}

void enemy_manager::begin_wave(int amnt, float spacing)
{
	s_current_wave_remaining = amnt;
	s_current_wave_interval = spacing;
	s_current_path = pathfinder::find_path(*s_level_grid);
}

void enemy_manager::render(const engine::ref<engine::shader>& shader)
{
	for (auto& pair : s_minions) {
		engine::renderer::submit(shader, pair.second.object());
	}
}
