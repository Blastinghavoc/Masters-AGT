#include "enemy_manager.h"

//Static initializers
std::map<int,enemy> enemy_manager::s_minions;
int enemy_manager::s_next_id = 0;
engine::ref<grid> enemy_manager::s_level_grid{};
std::deque<glm::vec3> enemy_manager::s_current_path{};
int enemy_manager::s_current_wave_remaining = 0;
float enemy_manager::s_interval_accumulator = 0;
float enemy_manager::s_current_wave_interval = 2.f;
std::stack<enemy*> enemy_manager::s_minion_buffer{};
std::vector<enemy*> enemy_manager::s_current_active_minions{};
engine::ref<engine::SpotLight> enemy_manager::m_spot_light{};

void enemy_manager::init(engine::ref<grid> level_grid)
{
	s_level_grid = level_grid;

	//Add one enemy to the buffer, allowing the enemy prefab to be initialised.
	auto minion = &spawn_minion({ 0,0,0 });
	s_minion_buffer.push(minion);

	m_spot_light = std::make_shared<engine::SpotLight>();
	m_spot_light->Color = glm::vec3(.75f, .1f, .1f);
	m_spot_light->AmbientIntensity = 0.4f;
	m_spot_light->DiffuseIntensity = 0.6f;
	m_spot_light->Position = glm::vec3{0,0,0};
	m_spot_light->Attenuation.Constant = .1f;
	m_spot_light->Attenuation.Linear = .1f;
	m_spot_light->Attenuation.Exp = .01f;
	m_spot_light->Direction = glm::vec3(0, -1, 0);
	m_spot_light->Cutoff = .9f;
	m_spot_light->On = false;

	light_manager::spot_lights.push_back(m_spot_light);
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
				new_minion = s_minion_buffer.top();
				new_minion->object()->set_position(s_current_path.front());
				s_minion_buffer.pop();
			}

			new_minion->set_path(s_current_path);
			s_current_active_minions.push_back(new_minion);	
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

	if (s_current_active_minions.size() > 0)
	{
		auto furthest_forward_alive = *begin(s_current_active_minions);
		for (auto& minion_iterator = begin(s_current_active_minions); minion_iterator != end(s_current_active_minions);) {
			auto& current_minion = *minion_iterator;
			current_minion->on_update(time_step);
			//If the minion has reached the goal, deactivate it and signal the gameplay manager.
			if (current_minion->object()->position() == s_current_path.back())
			{
				s_minion_buffer.push(current_minion);
				minion_iterator = s_current_active_minions.erase(minion_iterator);//Erase and return next.
				gameplay_manager::damage_portal();

			}
			else {
				//Otherwise, move on to the next.

				//Check if this minion is closer to the goal than any we've seen so far
				if (current_minion->waypoints_remaining() < furthest_forward_alive->waypoints_remaining())
				{
					furthest_forward_alive = current_minion;
				}
				++minion_iterator;
			}
		}

		m_spot_light->Position = furthest_forward_alive->position();
		m_spot_light->Position.y += 4.f;
		m_spot_light->On = true;
	}
	else {
		m_spot_light->On = false;
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
	for (auto& minion_ptr : s_current_active_minions) {
		engine::renderer::submit(shader, minion_ptr->object());
	}
}

//Render the trigger boxes of the enemies
void enemy_manager::render_trigger_boxes(const engine::ref<engine::shader>& shader)
{
	for (auto& minion_ptr : s_current_active_minions) {
		minion_ptr->get_trigger_box().on_render(shader);
	}
}
