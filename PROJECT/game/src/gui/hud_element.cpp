#include "hud_element.h"

hud_element::hud_element(glm::vec2 position, glm::vec2 relative_scale, engine::ref<engine::texture_2d> texture) :
	m_texture(texture)	
{
	m_relative_position = position;
	m_quad = engine::quad::create(screen_position(relative_scale));
}

void hud_element::render(engine::ref<engine::shader> shader)
{
	if (!m_show)
		return;

	if (m_quad && m_texture)
	{
		auto screen_pos = screen_position();

		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, glm::vec3(screen_pos.x, screen_pos.y, 0.1f));

		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", m_transparency);

		m_texture->bind();
		engine::renderer::submit(shader, m_quad->mesh(), transform);
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
	}
}
