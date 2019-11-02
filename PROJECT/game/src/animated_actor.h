#pragma once

#include <engine.h>
#include "glm/gtx/rotate_vector.hpp"

/*
Abstract class for animated actors (game entities)
*/
class animated_actor
{
public:
	virtual void on_update(const engine::timestep& time_step) = 0;

	engine::ref<engine::game_object> object() const { return m_object; }

protected:
	//The gameobject associated with the actor
	engine::ref< engine::game_object> m_object;

	//Change the animation to play
	void switch_animation(std::string anim);
	void switch_animation(uint32_t index);

	void move(const glm::vec3& direction,const float& speed, const engine::timestep& time_step,const float& max_distance = INFINITY);

	//Name of the current animation
	uint32_t m_current_animation;

	std::map<std::string, uint32_t> m_animations{};
};

