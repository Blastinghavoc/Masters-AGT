#pragma once
#include "abstract_hud_element.h"

class text_hud_element : public abstract_hud_element {
public:
	text_hud_element(engine::ref<engine::text_manager> text_manager,std::string text = "", glm::vec2 relative_text_position = { 0,0 });
	~text_hud_element() {};

	void render(engine::ref<engine::shader> shader) override;

	void set_colour(glm::vec4 colour) { m_text_colour = colour; };
	void set_text_size(float size) { m_text_size = size; };
	void set_text(std::string text) { m_text = text; };

	static engine::ref<text_hud_element> create(engine::ref<engine::text_manager> text_manager, std::string text = "", glm::vec2 relative_text_position = { 0,0 }) {
		return std::make_shared<text_hud_element>(text_manager, text, relative_text_position);
	};

	static glm::vec4 default_colour;


private:
	engine::ref<engine::text_manager> m_text_manager;
	std::string m_text;
	float m_text_size{1.f};
	glm::vec4 m_text_colour{};
};
