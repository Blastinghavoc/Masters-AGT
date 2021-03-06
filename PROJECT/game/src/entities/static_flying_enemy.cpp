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
	m_brain.on_update(time_step, (*this));//Delegate control to the "brain"
	update_trigger_box();//Update trigger box after any movements.
}

float static_flying_enemy::square_distance_to_next_waypoint() const
{
	return glm::distance2(ground_position(), next_waypoint());;
}

//Extension of reset to also reset the AI
void static_flying_enemy::reset()
{
	abstract_enemy::reset();
	m_brain.reset();
}

/*
Overriden to acount for altitude. Flying enemies count as being at a waypoint
if they are above it at their correct altitude.
*/
void static_flying_enemy::update_waypoints()
{
	while (!m_waypoints.empty() && close_enough(ground_position(), m_waypoints.front()))
	{
		m_waypoints.pop_front();
	}
}

glm::vec3 static_flying_enemy::ground_position() const
{
	return m_object->position() - glm::vec3(0, m_y_offset, 0);
}
