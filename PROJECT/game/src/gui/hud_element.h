#pragma once

#include "engine.h"
#include "abstract_hud_element.h"

/*
Textured HUD element based on cross-fade example.
Extended to support z ordering, and to use relative positioning inherited from
abstract_hud_element
*/
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

	void set_transparency(float trans) { m_transparency = trans; };
	const float& get_transparency() { return m_transparency; };
	void set_z_order(float z) { m_z = z; };
private:
	engine::ref<engine::quad> m_quad{};
	engine::ref<engine::texture_2d> m_texture{};
	float m_transparency{1.f};
	float m_z{ 0.1f };
};
