#pragma once

#include "engine.h"
#include "abstract_hud_element.h"

//Hud element based on cross-fade example
class hud_element: public abstract_hud_element {
public:
	hud_element() {};
	hud_element(glm::vec2 relative_position, glm::vec2 relative_scale, engine::ref<engine::texture_2d> texture);

	~hud_element() {};

	void render(engine::ref<engine::shader> shader) override;

	void set_quad(engine::ref<engine::quad> quad) { m_quad=quad; };	
	void set_texture(engine::ref<engine::texture_2d> texture) { m_texture = texture; };

	static engine::ref<hud_element> create(glm::vec2 relative_position, glm::vec2 relative_scale, engine::ref<engine::texture_2d> texture) {
		return std::make_shared<hud_element>(relative_position, relative_scale,texture);
	};

private:
	engine::ref<engine::quad> m_quad{};
	engine::ref<engine::texture_2d> m_texture{};
};
