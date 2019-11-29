#pragma once
#include "abstract_actor.h"
#include "trigger_box.h"

/*
Class representing some object that can be pickup up by the player.
The actual pickup can be any game_object; this class just defines
the trigger for it being picked up, and a rotation effect while the pickup
is active.
*/
class pickup : public abstract_actor
{
public:
	pickup();
	pickup(engine::ref<engine::game_object> obj,float rotation_speed = .1f);
	~pickup();

	//Update any time dependent properties of the pickup.
	void on_update(const engine::timestep& time_step) override;

	trigger_box& get_trigger_box() { return m_box; };

	bool is_active() { return m_active; };
	void set_active(bool val) { m_active = val; };
private:

	trigger_box m_box{};
	float m_rotation_speed;//in radians per second
	bool m_active = false;
};

