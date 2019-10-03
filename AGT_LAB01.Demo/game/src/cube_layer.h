#pragma once

#pragma once
#include <engine.h>

class cube_layer : public engine::layer
{
public:
	cube_layer();
	~cube_layer();

	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void render_cube_sine(const glm::vec4& colour, const engine::ref<engine::shader>& colour_shader, int x, int y);
	void render_one_cube(glm::vec3& pos, const glm::vec4& colour, const engine::ref<engine::shader>& colour_shader);
	void on_event(engine::event& event) override;

private:

	engine::ref<engine::texture_2d>     m_skybox_texture{};
	engine::ref<engine::skybox>			m_skybox{};
	engine::ref<engine::game_object>	m_cube;

	engine::orthographic_camera       m_2d_camera;
	engine::perspective_camera        m_3d_camera;

	float wave_offset;
};
