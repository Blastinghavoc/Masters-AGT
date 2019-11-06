#include "hud_element.h"

hud_element::hud_element(glm::vec2 position, engine::ref<engine::quad> quad, engine::ref<engine::texture_2d> texture) :
	m_position(position),
	m_quad(quad),
	m_texture(texture)	
{
}

hud_element::~hud_element()
{
}

void hud_element::render(engine::ref<engine::shader> shader)
{
	if (!m_show)
		return;

	if (m_quad && m_texture)
	{
		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, glm::vec3(m_position.x, m_position.y, 0.1f));

		m_texture->bind();
		engine::renderer::submit(shader, m_quad->mesh(), transform);
	}
}
