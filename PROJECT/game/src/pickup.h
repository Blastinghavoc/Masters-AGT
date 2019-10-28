#pragma once
#include <engine.h>

/*
Class representing some object that can be pickup up by the player.
WIP, more functionality will be added later.
*/
class pickup
{
public:
	pickup();
	pickup(engine::ref<engine::game_object>& obj,float rotation_speed = .1f);
	~pickup();

	//Update any time dependent properties of the pickup. (currently just spin)
	void on_update(const engine::timestep& time_step);

	engine::ref<engine::game_object> object() const { return m_object; }

protected:
	engine::ref<engine::game_object> m_object;

	float m_rotation_speed;
};

