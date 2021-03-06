#include "enemy_manager.h"
#include "../gameplay/pickup_manager.h"
#include "../gameplay/gameplay_manager.h"
#include "../entities/enemy_factory.h"

//Static variable initializers
std::map<int, engine::ref<abstract_enemy>> enemy_manager::s_minions;
int enemy_manager::s_next_id = 0;
engine::ref<grid> enemy_manager::s_level_grid{};
std::deque<glm::vec3> enemy_manager::s_current_path{};
int enemy_manager::s_current_wave_remaining = 0;
float enemy_manager::s_interval_accumulator = 0;
float enemy_manager::s_current_wave_interval = 2.f;
std::multimap<enemy_type, engine::ref<abstract_enemy>> enemy_manager::s_minion_buffer{};
std::vector<engine::ref<abstract_enemy>> enemy_manager::s_current_active_minions{};
engine::ref<engine::SpotLight> enemy_manager::m_spot_light{};
std::deque<std::pair<int, enemy_type>> enemy_manager::s_spawn_sequence;

void enemy_manager::init(engine::ref<grid> level_grid)
{
	s_level_grid = level_grid;

	//Add one enemy to the buffer, allowing the enemy prefab to be initialised.
	auto minion = spawn_minion(enemy_type::animated_humanoid,{ 0,0,0 });
	s_minion_buffer.emplace(enemy_type::animated_humanoid,minion);

	//Create the spotligh to track the frontrunner enemy
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

void enemy_manager::on_update(const engine::timestep& time_step)
{
	//Spawn new enemies if there are some remaining this wave
	if (s_current_wave_remaining > 0)
	{
		s_interval_accumulator += time_step;
		if (s_interval_accumulator > s_current_wave_interval)//If enough time has passed between enemies
		{
			//Spawn minion 
			auto& spawn_instruction = s_spawn_sequence.front();
			if (spawn_instruction.first <= 0)
			{//If we've spawned all the enemies in one instruction, get the next
				s_spawn_sequence.pop_front();
				spawn_instruction = s_spawn_sequence.front();
			}			

			s_current_active_minions.push_back(spawn_minion(spawn_instruction.second, s_current_path.front()));
			--spawn_instruction.first;//Spawned one of the enemies
			s_interval_accumulator -= s_current_wave_interval;
			--s_current_wave_remaining;
		}
	}
	else {//no enemies left to spawn
		if (s_current_active_minions.empty() && gameplay_manager::is_wave_active())
		{
			//wave is active but no more to spawn, and none left active -> next wave!
			gameplay_manager::next_build_phase();
			return;
		}
	}

	//Update any active minions
	if (!s_current_active_minions.empty())
	{
		/*
		Two pass algorithm: first remove any minions that have died or reached the goal,
		then update all minions that are still active.
		This has the advantage that minion AI can always have access to an up-to-date
		set of all the other active minions.
		*/
		for (auto& minion_iterator = begin(s_current_active_minions); minion_iterator != end(s_current_active_minions);) {
			auto& current_minion = *minion_iterator;			
			//If the minion has reached the goal, deactivate it and signal the gameplay manager.
			if (current_minion->waypoints_remaining() == 0)
			{
				//Move back to buffer
				s_minion_buffer.emplace(current_minion->type(),current_minion);

				minion_iterator = s_current_active_minions.erase(minion_iterator);//Erase and return next.

				gameplay_manager::damage_portal();

			}
			else if (current_minion->health() <= 0) {
				//If the minion has died, deactivate it and add some score

				//Move back to buffer
				s_minion_buffer.emplace(current_minion->type(), current_minion);

				gameplay_manager::add_score((int)current_minion->max_health());//Score diretly tied to enemy health.

				pickup_manager::roll_for_powerup(current_minion->ground_position());//Maybe drop a powerup

				minion_iterator = s_current_active_minions.erase(minion_iterator);//Erase and return next.
			}
			else {
				//Otherwise, move on to the next.
				++minion_iterator;
			}
		}

		//Update general data for AI before individual updates
		{
			flyer_ai::update_shared_data();
		}

		//Second pass, update each active minion
		for (auto& current_minion: s_current_active_minions) {
			current_minion->on_update(time_step);
		}

		//If there's still minions left active, highlight the furthest forward, and sort them by estimated distance from the goal
		if (!s_current_active_minions.empty())
		{
			std::sort(begin(s_current_active_minions), end(s_current_active_minions), &abstract_enemy::is_closer_to_goal);

			auto& furthest_forward_alive = s_current_active_minions.front();

			m_spot_light->Position = furthest_forward_alive->position();
			m_spot_light->Position.y += 4.f;
			m_spot_light->On = true;
		}
	}
	else {
		//If there are no active minions, turn off the spotlight
		m_spot_light->On = false;
	}
}

/*Spawn a minion, hopefully by recycling one from the buffer.
If there are none available, a new one is created.
*/
engine::ref<abstract_enemy> enemy_manager::spawn_minion(enemy_type type, glm::vec3 position)
{

	engine::ref<abstract_enemy> new_minion;
	if (s_minion_buffer.count(type) <1)//None in buffer, so create a new one
	{
		new_minion = enemy_factory::create(s_next_id, position, type);
		s_minions[s_next_id] = new_minion;
		++s_next_id;
	}
	else {
		//remove enemy of correct type from buffer.
		auto iterator = s_minion_buffer.find(type);
		new_minion = iterator->second;
		new_minion->object()->set_position(position);
		s_minion_buffer.erase(iterator);
	}

	//Reset minion gameplay stats, and set path
	new_minion->reset();
	new_minion->set_path(s_current_path);	
	
	return new_minion;
}

//Initialise variables for a new wave.
void enemy_manager::begin_wave(wave_definition wave_def)
{
	s_current_wave_remaining = wave_def.num_enemies;
	s_current_wave_interval = wave_def.enemy_spacing;
	s_spawn_sequence = wave_def.enemies;
	s_current_path = pathfinder::find_path(*s_level_grid);
}

void enemy_manager::render_animated(const engine::ref<engine::shader>& shader)
{
	//Render only minions that are currently active, and are animated
	for (auto& minion_ptr : s_current_active_minions) {
		if (minion_ptr->type() == enemy_type::animated_humanoid)
		{
			engine::renderer::submit(shader, minion_ptr->object());
		}
	}
}

void enemy_manager::render_static(const engine::ref<engine::shader>& shader)
{
	//Render active static enemies
	for (auto& minion_ptr : s_current_active_minions) {
		if (minion_ptr->type() != enemy_type::animated_humanoid)
		{
			engine::renderer::submit(shader, minion_ptr->object());
		}
	}
}

//Render the trigger boxes of the enemies
void enemy_manager::render_trigger_boxes(const engine::ref<engine::shader>& shader)
{
	for (auto& minion_ptr : s_current_active_minions) {
		minion_ptr->get_trigger_box().on_render(shader);
	}
}
