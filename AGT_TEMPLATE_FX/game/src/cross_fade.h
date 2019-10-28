#pragma once
#include <engine.h>

class quad;

class cross_fade
{
public:
	cross_fade(glm::vec3 colour, float max_time, float width, float height);
	~cross_fade();

	void on_update(const engine::timestep& time_step);
	void on_render(engine::ref<engine::shader> shader);
	void activate();

	static engine::ref<cross_fade> create(glm::vec3 colour, float max_time, float width, float height);

private:
	bool s_active;

	float m_timer;
	float m_max_time;

	engine::ref<engine::material> m_material;
	engine::ref<quad> m_quad;
};
