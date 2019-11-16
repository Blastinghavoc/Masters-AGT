#pragma once

#include "engine.h"
class abstract_actor {
public:
	virtual ~abstract_actor() {};

	virtual void on_update(const engine::timestep& time_step) = 0;

	engine::ref<engine::game_object> object() const { return m_object; }
protected:
	//The gameobject associated with the actor
	engine::ref< engine::game_object> m_object;

	virtual void move(const glm::vec3& direction, const float& speed, const engine::timestep& time_step, const float& max_distance = INFINITY);
};
