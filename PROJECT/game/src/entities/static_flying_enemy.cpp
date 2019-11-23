#include "pch.h"
#include "static_flying_enemy.h"
#include <glm/gtx/norm.hpp>


static_flying_enemy::static_flying_enemy() :static_enemy{ }
{
	m_type = enemy_type::robot1;
}

static_flying_enemy::static_flying_enemy(int id, engine::ref<engine::game_object> obj, float altitude, enemy_type type) : static_enemy{ id,obj,type },
m_y_offset{altitude}
{
	
}

void static_flying_enemy::on_update(const engine::timestep& time_step)
{
	//Modified from abstract_enemy
	while (!m_waypoints.empty() && close_enough(ground_position(), m_waypoints.front()))
	{
		m_waypoints.pop_front();
	}

	m_box.on_update(m_object->position());


	if (m_frozen)
	{
		return;//can't move if frozen
	}

	if (!m_waypoints.empty())
	{
		//Try to go towards the first waypoint that we aren't already at
		auto target = m_waypoints.front();
		target.y += m_y_offset;//Account for the fact that this object may be flying

		auto line = target - m_object->position();
		auto direction = glm::normalize(line);

		move(direction, m_movement_speed, time_step, glm::length(line));

	}
}



float static_flying_enemy::square_distance_to_next_waypoint() const
{
	return glm::distance2(ground_position(), next_waypoint());;
}

glm::vec3 static_flying_enemy::ground_position() const
{
	return m_object->position() - glm::vec3(0, m_y_offset, 0);
}
