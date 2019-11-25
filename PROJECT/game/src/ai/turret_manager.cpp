#include "pch.h"
#include "turret_manager.h"
#include "enemy_manager.h"
#include "../gameplay/gameplay_manager.h"
#include "../sfx/sfx_manager.h"

std::map<turret_manager::vec3, engine::ref<turret>> turret_manager::m_turrets{};
std::vector<engine::ref<turret>> turret_manager::m_buffer;

void turret_manager::init()
{
	//Add a few turrets to the buffer
	m_buffer.push_back(turret::create());
	m_buffer.push_back(turret::create());
	m_buffer.push_back(turret::create());
}

void turret_manager::render(engine::ref<engine::shader> shader)
{
	for (auto& pair : m_turrets) {		
		pair.second->render(shader);
	}
}

bool turret_manager::place_turret(glm::vec3 position)
{
	auto map_pos = from_glm(position);
	if (m_turrets.count(map_pos) > 0)
	{
		return false;//can't place turrets on top of each other.
	}

	if (m_buffer.empty())
	{
		m_turrets[map_pos] = turret::create(position);
	}
	else {
		//If possible, use a turret from the buffer rather than creating a new one.
		auto t = m_buffer.back();
		t->set_position(position);
		m_turrets[map_pos] = t;
		m_buffer.pop_back();
	}

	return true;
}

bool turret_manager::remove_turret(glm::vec3 position)
{
	auto map_pos = from_glm(position);
	if (m_turrets.count(map_pos) > 0)
	{
		m_buffer.push_back(m_turrets[map_pos]);//return turret to bufer
		m_turrets.erase(map_pos);
		return true;
	}
	return false;//Cannot remove turret that does not exist.
}

void turret_manager::update(const engine::timestep& ts)
{
	//As long as this function is called after the enemy_manager update, the active enemies are guaranteed to be sorted.
	auto& enemies = enemy_manager::get_active_enemies();
	for (auto& pair : m_turrets) {
		auto& current_turret = pair.second;		
		for (auto& enemy: enemies) {
			//Attack the enemy that is closest to the goal and in range
			if (current_turret->is_in_range(enemy->position()))
			{
				current_turret->face(enemy->position());
				current_turret->update(ts);//Update the turret's cooldown
				auto damage = current_turret->fire();
				if (damage > 0)//Positive damage indicates turret fired successfully
				{
					enemy->deal_damage(damage);//Deals damage to the enemy if the turret is not on cooldown.
					gameplay_manager::audio_manager()->play("laser");
					sfx_manager::make_beam(current_turret->barrel_position(), enemy->targetting_point(), glm::vec3(.9f, .1f, .1f), 0.25f);
				}
				break;
			}
		}
	}
}
