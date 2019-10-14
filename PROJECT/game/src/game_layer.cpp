#include "game_layer.h"
#include "platform/opengl/gl_shader.h"
#include "engine/events/key_event.h"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

#define _USE_MATH_DEFINES
#include <math.h>
#include "intro_screen.h"


game_layer::game_layer() :
m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f),
m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height())
{
	// Hide the mouse and lock it inside the window
	//engine::input::anchor_mouse(true);
	engine::application::window().hide_mouse_cursor();

	// Initialise audio and play background music
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	m_audio_manager->load_sound("assets/audio/DST-impuretechnology.mp3", engine::sound_type::track, "music");  // Royalty free music from http://www.nosoapradio.us/
	//m_audio_manager->play("music");


	// Initialise the shaders, materials, and lights
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	auto mesh__material_shader = engine::renderer::shaders_library()->get("mesh_material");
	auto mesh_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");

	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 0.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gDirectionalLight.Base.Color", m_directionalLight.Color);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gDirectionalLight.Base.AmbientIntensity", m_directionalLight.AmbientIntensity);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gDirectionalLight.Direction", glm::normalize(m_directionalLight.Direction));
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gDirectionalLight.Base.DiffuseIntensity", m_directionalLight.DiffuseIntensity);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gMatSpecularIntensity", 0.5f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gSpecularPower", 5.f);

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gDirectionalLight.Base.Color", m_directionalLight.Color);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gDirectionalLight.Base.AmbientIntensity", m_directionalLight.AmbientIntensity);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gDirectionalLight.Direction", glm::normalize(m_directionalLight.Direction));
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gDirectionalLight.Base.DiffuseIntensity", m_directionalLight.DiffuseIntensity);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gMatSpecularIntensity", 0.5f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gSpecularPower", 5.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("transparency", 1.0f);

	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
		glm::ortho(0.f, (float)engine::application::window().width(), 0.f, (float)engine::application::window().height()));

	m_material = engine::material::create(32.0f, glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	//TODO REF Skybox texture from https://opengameart.org/content/space-skyboxes-0
	std::string skybox_path = "assets/textures/skyboxes/blue/";
	std::string skybox_extn = ".png";
	m_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create(skybox_path+"bkg1_front"+skybox_extn),
		  engine::texture_2d::create(skybox_path + "bkg1_right" + skybox_extn),
		  engine::texture_2d::create(skybox_path + "bkg1_back" + skybox_extn),
		  engine::texture_2d::create(skybox_path + "bkg1_left" + skybox_extn),
		  engine::texture_2d::create(skybox_path + "bkg1_top" + skybox_extn),
		  engine::texture_2d::create(skybox_path + "bkg1_bot" + skybox_extn)
		});

	// Load the terrain texture and create a terrain mesh. Create a terrain object. Set its properties
	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/terrain_grid.bmp") };
	engine::ref<engine::terrain> terrain_shape = engine::terrain::create(100.f, 0.5f, 100.f);
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { terrain_shape->mesh() };
	terrain_props.textures = terrain_textures;
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.bounding_shape = glm::vec3(100.f, 0.5f, 100.f);
	terrain_props.restitution = 0.92f;
	terrain_props.position = { 0,-.5f,0 };
	m_terrain = engine::game_object::create(terrain_props);


	std::string path = "assets/models/static/dungeon/";
	std::string extn = ".obj";
	//Display all dungeon pieces
	//generate_all_level_pieces(m_level_segments,path,extn);

	//Populate level grid
	for (int i = 0; i < 15; i++)
	{
		m_level_grid.set_border(0,i,orientation::east);
		m_level_grid.set_border(14, i, orientation::west);
		m_level_grid.set_border(i, 0, orientation::south);
		m_level_grid.set_border(i, 14, orientation::north);

		m_level_grid.set_corner(0, i, orientation::south_east);
		m_level_grid.set_corner(14, i, orientation::south_west);		
		m_level_grid.set_corner(i, 0, orientation::south_east);
		m_level_grid.set_corner(i, 14, orientation::north_east);
		

		for (int j = 0; j < 15; j++)
		{
			m_level_grid.set_floor(i, j);
		}
	}
	m_level_grid.set_corner(15, 15,south_east);
	glm::vec3 center = m_level_grid.grid_to_world_coords(7,7);
	m_level_grid.set_corner(7, 7, orientation::north_east);
	m_level_grid.set_corner(7, 7, orientation::south_east);
	m_level_grid.set_corner(7, 7, orientation::south_west);
	m_level_grid.set_corner(7, 7, orientation::north_west);

	//Initialise intro screen
	intro_screen::init(m_3d_camera,center);

	//Create grid square for debug
	engine::ref<engine::grid_square> grid_shape = engine::grid_square::create(0.05f);
	engine::game_object_properties grid_shape_props;
	grid_shape_props.position = { 0.f, 0.8f, 0.f };
	grid_shape_props.meshes = { grid_shape->mesh() };
	grid_shape_props.bounding_shape = glm::vec3(1.f);	
	m_grid_square = engine::game_object::create(grid_shape_props);

	engine::ref<engine::rhombicuboctahedron> shape = engine::rhombicuboctahedron::create();
	engine::game_object_properties shape_props;
	shape_props.position = { 10.f, 2.f, 10.f };
	shape_props.meshes = { shape->mesh() };
	shape_props.bounding_shape = glm::vec3(1.f);
	m_game_objects.push_back(engine::game_object::create(shape_props));

	//Create text manager
	m_text_manager = engine::text_manager::create();
}

game_layer::~game_layer()
{
}

void game_layer::on_update(const engine::timestep& time_step)
{
	if (intro_screen::active())
	{
		intro_screen::update(m_3d_camera, time_step);
	}
	else {
		m_3d_camera.on_update(time_step);
		m_fps = 1 / time_step;
	}
}

void game_layer::on_render()
{
	const auto cam_pos = m_3d_camera.position();

	engine::render_command::clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });
	engine::render_command::clear();

	const auto textured_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");
	engine::renderer::begin_scene(m_3d_camera, textured_lighting_shader);

	// Set up some of the scene's parameters in the shader
	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	// Position the skybox centred on the player and render it
	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_camera.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(textured_lighting_shader, m_skybox, skybox_tranform);

	engine::renderer::submit(textured_lighting_shader, m_terrain);

	m_level_grid.render(textured_lighting_shader);

	for each (auto var in m_game_objects)
	{
		engine::renderer::submit(textured_lighting_shader, var);
	}

	engine::renderer::end_scene();

	// Set up material shader. (does not render textures, renders materials instead)
	const auto textured_material_shader = engine::renderer::shaders_library()->get("mesh_material");
	engine::renderer::begin_scene(m_3d_camera, textured_material_shader);

	m_material->submit(textured_material_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(textured_material_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	//Shows a 1m debug grid. Not super efficient, and uses lots of magic numbers
	//TODO maybe improve?
	if (m_show_debug)
	{
		int row_width = 10;
		const glm::vec3 base_position{ floor(cam_pos.x)-row_width/2,0.6f,floor(cam_pos.z)-row_width/2 };
		for (int i = 0; i < row_width*row_width; i++)
		{
			auto offset = base_position + glm::vec3(i % row_width, 0.f, i / row_width);
			glm::mat4 transform(1.0f);
			transform = glm::translate(transform, offset);
			transform = glm::rotate(transform, 0.f, {0,1,0});
			transform = glm::scale(transform, {1,1,1});
			engine::renderer::submit(textured_material_shader, transform, m_grid_square);
		}
	}


	// Render text

	if (intro_screen::active())
	{
		intro_screen::render(m_text_manager);
	}
	else {
		const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
		m_text_manager->render_text(text_shader, "Placeholder UI", 10.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
		if (m_show_debug)
		{
			m_text_manager->render_text(text_shader, "Pos:{" + std::to_string(cam_pos.x) + "," + std::to_string(cam_pos.y)
				+ "," + std::to_string(cam_pos.z) + "}", 10.f, (float)engine::application::window().height() - 50.f, .5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
			m_text_manager->render_text(text_shader, "FPS:{" + std::to_string((int) m_fps) + "}", 10.f, (float)engine::application::window().height() - 75.f, .5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
			m_text_manager->render_text(text_shader, "Facing:{" +std::to_string(m_3d_camera.front_vector().x) + "," + std::to_string(m_3d_camera.front_vector().y)
				+ "," + std::to_string(m_3d_camera.front_vector().z) + "}", 10.f, (float)engine::application::window().height() - 100.f, .5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
		}
	}
}

void game_layer::on_event(engine::event& event)
{
	if (event.event_type() == engine::event_type_e::key_pressed)
	{		

		auto& e = dynamic_cast<engine::key_pressed_event&>(event);
		switch (e.key_code())
		{
		case engine::key_codes::KEY_TAB:
			engine::render_command::toggle_wireframe();
			break;
		case engine::key_codes::KEY_F1:
			m_show_debug = !m_show_debug;
			break;

		case engine::key_codes::KEY_ENTER:			
			if (intro_screen::active())
			{
				intro_screen::active(false);
				m_3d_camera.position(m_camera_start_pos);
				m_3d_camera.face(to_vec(north));
			}
			break;
		default:
			break;
		}
		
	}
}

void generate_all_level_pieces(std::vector<engine::ref<engine::game_object>>& level_segments,const std::string& path,const std::string& extn) {
	//Display all dungeon pieces
	std::vector<std::string> models;	
	get_all_models_in_directory(models, path, extn);
	for (int i = 0; i < models.size(); i++)
	{
		// Load the level segment model. Create object. Set its properties
		engine::ref <engine::model> model = engine::model::create(path + models[i] + extn);
		engine::game_object_properties props;
		props.meshes = model->meshes();
		props.textures = model->textures();
		float scale = 1.f / glm::max(model->size().x, glm::max(model->size().y, model->size().z));
		props.position = { -4 * (i % 6),1.f, -4 * (i / 6) };
		props.scale = glm::vec3(scale);
		props.bounding_shape = model->size() / 2.f * scale;
		level_segments.push_back(engine::game_object::create(props));
	}
}

/*
Populates vec with the names of all files with the given extension in the directory described by path
*/
void get_all_models_in_directory(std::vector < std::string >& vec,const std::string& path,const std::string& extension) {	
	//TODO REF https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
	for (const auto& entry : fs::directory_iterator(path)) {
		if (extension == entry.path().extension().string())
		{
			vec.push_back(entry.path().filename().replace_extension().string());
		}
	}
}
