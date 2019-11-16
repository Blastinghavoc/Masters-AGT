#include "pch.h"
#include "abstract_enemy.h"
#include <glm/gtx/norm.hpp>

void abstract_enemy::on_update(const engine::timestep& time_step)
{	
	//Remove any waypoints from the front of the queue if we're already there.
	while (!m_waypoints.empty() && m_object->position() == m_waypoints.front())
	{
		m_waypoints.pop_front();
	}

	m_box.on_update(m_object->position());	
}

glm::vec3 abstract_enemy::next_waypoint() const
{
	if (m_waypoints.empty())
	{
		throw std::runtime_error("Cannot get next waypoint: there is none!");
	}
	return m_waypoints.front();
}

float abstract_enemy::square_distance_to_next_waypoint() const
{
	return glm::distance2(m_object->position(), next_waypoint());
}

bool abstract_enemy::is_closer_to_goal_than(const abstract_enemy& e2) const
{
	if (waypoints_remaining() < e2.waypoints_remaining())
	{
		return true;
	}
	else if (waypoints_remaining() == e2.waypoints_remaining() && waypoints_remaining() > 0)
	{
		//Same number of waypoints remaining, so check whether the current minion is close to it's next waypoint.
		
		if (square_distance_to_next_waypoint() < e2.square_distance_to_next_waypoint())
		{
			return true;
		}
		
	}
	return false;
}
