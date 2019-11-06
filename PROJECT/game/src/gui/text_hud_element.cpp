#include "text_hud_element.h"

text_hud_element::text_hud_element(engine::ref<engine::text_manager> text_manager,std::string text, glm::vec2 relative_text_position):
	m_text_manager(text_manager),
	m_text(text),
	m_relative_text_position(relative_text_position)
{
}

void text_hud_element::render(engine::ref<engine::shader> shader)
{
	hud_element::render(shader);

	auto x = engine::application::window().width() * m_relative_text_position.x;
	auto y = engine::application::window().height() * m_relative_text_position.y;

	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	m_text_manager->render_text(text_shader, m_text, x, y,m_text_size,m_text_colour);
}


