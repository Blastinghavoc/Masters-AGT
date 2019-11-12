#include "text_hud_element.h"

glm::vec4 text_hud_element::default_colour{1,1,1,1};

text_hud_element::text_hud_element(engine::ref<engine::text_manager> text_manager, std::string text, glm::vec2 relative_text_position) :
	m_text_manager(text_manager),
	m_text(text)
{
	m_relative_position = relative_text_position;
	m_text_colour = default_colour;
}

//Doesn't actually use the passed shader.
void text_hud_element::render(engine::ref<engine::shader> shader)
{
	if (!m_show)
		return;

	auto screen_pos = screen_position();

	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	m_text_manager->render_text(text_shader, m_text, screen_pos.x, screen_pos.y,m_text_size,m_text_colour);
}


