#pragma once

#include <engine.h>
#include "glm/gtx/rotate_vector.hpp"
#include "abstract_actor.h"

/*
Abstract class for animated actors (game entities)
*/
class animated_actor : public virtual abstract_actor
{
public:
	virtual ~animated_actor(){};

protected:
	//Change the animation to play
	void switch_animation(std::string anim);
	void switch_animation(uint32_t index);

	//Name of the current animation
	uint32_t m_current_animation;

	std::map<std::string, uint32_t> m_animations{};
};

