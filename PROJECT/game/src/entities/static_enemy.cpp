#include "pch.h"
#include "static_enemy.h"


static_enemy::static_enemy() :abstract_enemy{ -1 }
{
	m_type = enemy_type::robot1;
}

static_enemy::static_enemy(int id, engine::ref<engine::game_object> obj, enemy_type type) : abstract_enemy{ id }
{
	m_type = type;
	m_object = obj;
	m_box.set_box(m_object);
}

void static_enemy::on_update(const engine::timestep& time_step)
{
	abstract_enemy::on_update(time_step);

	if (m_frozen)
	{
		return;//can't move if frozen
	}

	if (!m_waypoints.empty())
	{
		//Try to go towards the first waypoint that we aren't already at
		auto target = m_waypoints.front();

		auto line = target - m_object->position();
		auto direction = glm::normalize(line);

		move(direction, m_movement_speed, time_step, glm::length(line));

	}
	
}
