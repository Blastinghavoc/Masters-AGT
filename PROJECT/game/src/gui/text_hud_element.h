#pragma once
#include "hud_element.h"

class text_hud_element : public hud_element {
public:
	text_hud_element(engine::ref<engine::text_manager> text_manager,std::string text = "", glm::vec2 relative_text_position = { 0,0 });
	~text_hud_element() {};

	void render(engine::ref<engine::shader> shader) override;

	void set_colour(glm::vec4 colour) { m_text_colour = colour; };
	void set_text_size(float size) { m_text_size = size; };

private:
	engine::ref<engine::text_manager> m_text_manager;
	std::string m_text;
	glm::vec2 m_relative_text_position;
	float m_text_size{1.f};
	glm::vec4 m_text_colour{ 1.f, 0.5f, 0.f, 1.f };
};
