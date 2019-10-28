#pragma once
#include <engine.h>

class alpha_sphere
{
public:
	alpha_sphere(glm::vec3 start_colour, glm::vec3 end_colour, bool zap, float zap_level, float max_time);
	~alpha_sphere();

	void on_update(const engine::timestep& time_step);
	void on_render(engine::ref<engine::shader> shader);
	void activate(float radius, glm::vec3 position);

	static engine::ref<alpha_sphere> create(glm::vec3 start_colour, glm::vec3 end_colour, bool zap, float zap_level, float max_time);

private:

	float random();

	glm::vec3 m_position;
	bool s_active;
	float m_max_time;
	float m_timer;
	float m_zap_level;

	float m_scale;

	glm::vec3 m_start_colour;
	glm::vec3 m_end_colour;
	engine::ref<engine::material> m_material;
	engine::ref<engine::sphere> m_sphere;
	bool s_zap;
};
