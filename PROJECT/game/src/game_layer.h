#pragma once

#include <engine.h>
#include "engine/utils/timer.h"
#include "grid/grid.h"
#include "entities/player.h"
#include "ai/enemy_manager.h"
#include "gui/hud_manager.h"
#include "lighting/light_manager.h"
#include "gameplay/gameplay_manager.h"
#include "ai/turret_manager.h"

/*
The main layer for the game, constructs and initialises all subsystems, and sequences
updating and rendering.
*/
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

	//Used only for testing new models.
	engine::ref < engine::game_object > m_test_obj{};

	//Holds objects that are purely decorational.
	std::vector<engine::ref<engine::game_object>> m_decorational_objects{};
	//As above, but also holds a float representing the transparency of the object
	std::vector<std::pair<float, engine::ref<engine::game_object>>> m_transparent_decorational_objects{};

	engine::ref<engine::material>		m_material{};

	//The sun!
	engine::DirectionalLight            m_directionalLight;

	engine::ref<engine::audio_manager>  m_audio_manager{};
	engine::ref<engine::text_manager>	m_text_manager{};

	engine::orthographic_camera       m_2d_camera;
	engine::perspective_camera        m_3d_camera;

	glm::vec3 m_camera_start_pos{0.1f,3.f,0.1f};
	bool m_freecam{ false };

	//The grid object representing the level.
	engine::ref<grid> m_level_grid{};
	glm::vec3 m_grid_center{};

	//The Player instance
	player m_player{glm::vec3(3.f,.5f,0.f)};

	//Wether or not to show debug HUD
	bool m_show_debug{ false };

	//Debug info
	engine::timer m_fps_timer{};
	unsigned int m_num_updates{0};
	unsigned int m_updates_last_interval{0};

	//Data for the rhombicuboctahedrons being rendered with SoRT.
	engine::ref<engine::game_object> m_rhombi{};
	float m_rhombi_angle{ 1.f };
	glm::vec3 m_rhombi_trig_vector{0.f,0.f,0.f};
	float m_big_decor_height{ 12.f };

};

