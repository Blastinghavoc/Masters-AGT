#pragma once

#include "engine.h"

//Hud element based on cross-fade example
class hud_element {
public:
	hud_element() {};
	hud_element(glm::vec2 position, engine::ref<engine::quad> quad, engine::ref<engine::texture_2d> texture);

	~hud_element();

	virtual void render(engine::ref<engine::shader> shader);

	void show() { m_show = true; };
	void hide() { m_show = false; };

	void set_quad(engine::ref<engine::quad> quad) { m_quad=quad; };
	void set_position(glm::vec2 position) { m_position = position; };
	void set_texture(engine::ref<engine::texture_2d> texture) { m_texture = texture; };

private:
	glm::vec2 m_position{};
	engine::ref<engine::quad> m_quad{};
	engine::ref<engine::texture_2d> m_texture{};
	bool m_show = true;
};
