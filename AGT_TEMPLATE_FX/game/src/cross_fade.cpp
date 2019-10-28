#include "pch.h"
#include "cross_fade.h"
#include "quad.h"

cross_fade::cross_fade(glm::vec3 colour, float max_time, float width, float height)
{
	m_max_time = max_time;
	m_material = engine::material::create(0.0f, colour, colour, colour, 1.0f);

	m_quad = quad::create(glm::vec2(width, height));
	s_active = false;
}

cross_fade::~cross_fade()
{}

void cross_fade::on_update(const engine::timestep& time_step)
{
	if (!s_active)
		return;

	m_timer += (float)time_step;

	m_material->set_transparency(0.5f - 0.5f * m_timer / m_max_time);

	if (m_timer > m_max_time)
		s_active = false;
}

void cross_fade::on_render(engine::ref<engine::shader> shader)
{
	if (!s_active)
		return;

	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, glm::vec3(0.f, 0.f, 0.1f));

	m_material->submit(shader);
	engine::renderer::submit(shader, m_quad->mesh(), transform);
}

void cross_fade::activate()
{
	s_active = true;
	m_material->set_transparency(1.0f);
	m_timer = 0.0f;
}

engine::ref<cross_fade> cross_fade::create(glm::vec3 colour, float max_time, float width, float height)
{
	return std::make_shared<cross_fade>(colour, max_time, width, height);
}
