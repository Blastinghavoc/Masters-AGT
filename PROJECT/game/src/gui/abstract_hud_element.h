#pragma once

#include "engine.h"

class abstract_hud_element {
public:
	virtual ~abstract_hud_element() {};

	virtual void render(engine::ref<engine::shader> shader) = 0;

	void show() { m_show = true; };
	void hide() { m_show = false; };
	void set_relative_position(glm::vec2 position) { m_relative_position = position; };

protected:	
	bool m_show = true;
	glm::vec2 m_relative_position{0,0};

	glm::vec2 screen_position() {
		return screen_position(m_relative_position);
	};

	static glm::vec2 screen_position(glm::vec2 vec) {
		auto x = engine::application::window().width() * vec.x;
		auto y = engine::application::window().height() * vec.y;
		return { x,y };
	};
};
