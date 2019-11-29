#pragma once

#include <engine.h>
#include "glm/gtx/rotate_vector.hpp"
#include "abstract_actor.h"

/*
Abstract class for animated game entities.
Provides a mapping between strings and animation indices, and related functions to
simplify implementation of derived classes.

Note virtual inheritance ensuring a single instance of abstract_actor for derived classes.
*/
class animated_actor : public virtual abstract_actor
{
public:
	virtual ~animated_actor(){};

protected:
	//Change the animation to play
	void switch_animation(std::string anim);
	void switch_animation(uint32_t index);

	//Index of the current animation
	uint32_t m_current_animation;

	std::map<std::string, uint32_t> m_animations{};
};

