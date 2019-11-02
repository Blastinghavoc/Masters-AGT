#include "animated_actor.h"

void animated_actor::switch_animation(std::string anim)
{
	if (m_animations.count(anim) > 0)
	{
		auto index = m_animations[anim];
		m_object->animated_mesh()->switch_animation(index);
		m_current_animation = index;

	}
	else {
		throw std::runtime_error("Failed to switch to animation '"+anim +"' because it does not exist");
	}
}

void animated_actor::switch_animation(uint32_t index)
{
	m_object->animated_mesh()->switch_animation(index);
	m_current_animation = index;
}

//Moves the actor, making it face in the movement direction. Expects direction to be normalized
void animated_actor::move(const glm::vec3& direction, const float& speed, const engine::timestep& time_step, const float& max_distance)
{
	//Increment position
	auto distance_travelled = speed * (float)time_step;

	//Constrain distance
	distance_travelled = fmin(distance_travelled, max_distance);

	m_object->set_position(m_object->position() + direction * distance_travelled);

	//orient in direction of travel
	m_object->set_rotation_amount(atan2(direction.x, direction.z));
	m_object->animated_mesh()->switch_root_movement(false);
}
