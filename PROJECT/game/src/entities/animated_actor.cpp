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

