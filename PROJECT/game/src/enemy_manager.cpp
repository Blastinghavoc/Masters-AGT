#include "enemy_manager.h"

//Static initializers
std::map<int,enemy> enemy_manager::s_minions;
int enemy_manager::s_next_id = 0;
engine::ref<grid> enemy_manager::s_level_grid{};
std::deque<glm::vec3> enemy_manager::s_current_path{};
int enemy_manager::s_current_wave_remaining = 0;
float enemy_manager::s_interval_accumulator = 0;
float enemy_manager::s_current_wave_interval = 2.f;
std::stack<int> enemy_manager::s_minion_buffer{};
std::set<int> enemy_manager::s_current_active_minions{};

void enemy_manager::init(engine::ref<grid> level_grid)
{
	s_level_grid = level_grid;

	//Add one enemy to the buffer, allowing the enemy prefab to be initialised.
	s_minion_buffer.push(spawn_minion({0,0,0}).get_id());

}

void enemy_manager::on_update(engine::timestep time_step)
{
	if (s_current_wave_remaining > 0)
	{
		s_interval_accumulator += time_step;
		if (s_interval_accumulator > s_current_wave_interval)
		{
			enemy* new_minion;
			if (s_minion_buffer.empty())
			{
				new_minion = &spawn_minion(s_current_path.front());
			}
			else {
				new_minion = &s_minions[s_minion_buffer.top()];
				new_minion->object()->set_position(s_current_path.front());
				s_minion_buffer.pop();
			}

			new_minion->set_path(s_current_path);
			s_current_active_minions.emplace(new_minion->get_id());			
			s_interval_accumulator -= s_current_wave_interval;
			--s_current_wave_remaining;
		}
	}
	else {
		if (s_current_active_minions.empty() && gameplay_manager::is_wave_active())
		{
			//wave is active but no more to spawn, and none left active -> next wave!
			gameplay_manager::next_build_phase();
			return;
		}
	}


	for (auto& minion_iterator = begin(s_current_active_minions); minion_iterator != end(s_current_active_minions);) {
		auto& current_minion = s_minions[*minion_iterator];
		current_minion.on_update(time_step);
		//If the minion has reached the goal, deactivate it and signal the gameplay manager.
		if (current_minion.object()->position() == s_current_path.back())
		{
			s_minion_buffer.push(*minion_iterator);
			minion_iterator = s_current_active_minions.erase(minion_iterator);//Erase and return next.
			gameplay_manager::damage_portal();

		}
		else {
			//Otherwise, move on to the next.
			++minion_iterator;
		}
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
	//Render only minions that are currently active
	for (auto& minion_id : s_current_active_minions) {
		engine::renderer::submit(shader, s_minions[minion_id].object());
	}
}
