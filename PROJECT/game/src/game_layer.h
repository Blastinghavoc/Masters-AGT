#pragma once

#include <engine.h>

class game_layer : public engine::layer
{
public:
	game_layer();
	~game_layer();

	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;

private:

	engine::ref<engine::skybox>			m_skybox{};
	engine::ref<engine::game_object>	m_terrain{};
	engine::ref<engine::game_object>	m_grid_square{};

	std::vector<engine::ref < engine::game_object>> m_level_segments{};

	engine::ref<engine::material>		m_material{};

	engine::DirectionalLight            m_directionalLight;

	std::vector<engine::ref<engine::game_object>>     m_game_objects{};

	engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<engine::audio_manager>  m_audio_manager{};
	engine::ref<engine::text_manager>	m_text_manager{};

	engine::orthographic_camera       m_2d_camera;
	engine::perspective_camera        m_3d_camera;

	std::vector < engine::ref<engine::game_object>> m_complete_wall_segment;

	bool m_show_debug{ false };
};

void generate_all_level_pieces(std::vector<engine::ref<engine::game_object>>& level_segments, const std::string& path, const std::string& extn);

void get_all_models_in_directory(std::vector<std::string>& vec, const std::string& path, const std::string& extension);
