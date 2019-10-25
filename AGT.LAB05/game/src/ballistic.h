#pragma once

#include <engine.h>
class ballistic
{
public:
	ballistic();
	~ballistic();
	void initialise(engine::ref<engine::game_object> object);
	void fire(const engine::perspective_camera& camera, float speed);
	void on_update(const engine::timestep& time_step);
	void on_render(const engine::ref<engine::shader>& shader);
	engine::ref<engine::game_object> object() const { return m_object; }
private:
	engine::ref<engine::game_object> m_object;
	float m_speed = 0.0f;
};
