#include "pch.h"
#include "abstract_actor.h"

//Moves the actor in the specified direction (expected to be normalized)
void abstract_actor::move(const glm::vec3& direction, const float& speed, const engine::timestep& time_step, const float& max_distance, bool face_direction)
{
	//Increment position
	auto distance_travelled = speed * (float)time_step;

	//Constrain distance
	distance_travelled = fmin(distance_travelled, max_distance);

	m_object->set_position(m_object->position() + direction * distance_travelled);

	if (face_direction)
	{
		//orient in direction of travel (assumes rotation axis is y axis)		
		m_object->set_rotation_amount(atan2(direction.x, direction.z));
	}
}
