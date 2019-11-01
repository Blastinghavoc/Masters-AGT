#include "game_layer.h"
#include "platform/opengl/gl_shader.h"
#include "engine/events/key_event.h"
#include "engine/events/mouse_event.h"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

#define _USE_MATH_DEFINES
#include <math.h>

#include "intro_screen.h"


game_layer::game_layer() :
m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f),
m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height(),45.f,0.1f,200.f)
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
	auto mesh_material_shader = engine::renderer::shaders_library()->get("mesh_material");
	auto mesh_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");
	auto animated_mesh_shader = engine::renderer::shaders_library()->get("animated_mesh");

	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 0.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("lighting_on", true);
	m_directionalLight.submit(animated_mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gMatSpecularIntensity",
		0.5f);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gSpecularPower", 5.f);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("transparency", 1.0f);

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_material_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_material_shader)->set_uniform("lighting_on", true);
	m_directionalLight.submit(mesh_material_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_material_shader)->set_uniform("gMatSpecularIntensity",
		1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_material_shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("lighting_on", true);
	m_directionalLight.submit(mesh_lighting_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gMatSpecularIntensity",
		0.5f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gSpecularPower", 5.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("transparency", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
		glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
		(float)engine::application::window().height()));
	m_material = engine::material::create(1.0f, glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(1.0f, 0.1f, 0.07f),
		glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	//REF Skybox texture from https://opengameart.org/content/space-skyboxes-0
	std::string skybox_path = "assets/textures/skyboxes/blue/";
	std::string skybox_extn = ".png";
	m_skybox = engine::skybox::create(100.f,
		{ engine::texture_2d::create(skybox_path+"bkg1_front"+skybox_extn,true),
		  engine::texture_2d::create(skybox_path + "bkg1_right" + skybox_extn,true),
		  engine::texture_2d::create(skybox_path + "bkg1_back" + skybox_extn,true),
		  engine::texture_2d::create(skybox_path + "bkg1_left" + skybox_extn,true),
		  engine::texture_2d::create(skybox_path + "bkg1_top" + skybox_extn,true),
		  engine::texture_2d::create(skybox_path + "bkg1_bot" + skybox_extn,true)
		});

	// Load the terrain texture and create a terrain mesh. Create a terrain object. Set its properties
	// Using my tiled_cuboid instead of the original terrain shape in order to have tiled textures.
	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/terrain_grid.bmp",false) };//Texture by me
	glm::vec3 terrain_dimensions{ 19.f, 0.5f, 19.f };
	engine::ref<engine::tiled_cuboid> terrain_shape = engine::tiled_cuboid::create(terrain_dimensions, false, {terrain_dimensions.x,terrain_dimensions.z});
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { terrain_shape->mesh() };
	terrain_props.textures = terrain_textures;
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.bounding_shape = terrain_dimensions;
	terrain_props.restitution = 0.92f;
	terrain_props.position = { 0,-.5f,0 };
	m_terrain = engine::game_object::create(terrain_props);

	//Testing object
	//engine::ref <engine::model> test_model = engine::model::create("assets/models/static/gold/gold_05_modified.obj");	
	//engine::game_object_properties test_props;
	//test_props.meshes = test_model->meshes();
	/*test_props.textures = { engine::texture_2d::create("assets/models/static/gold/g_diffuse.tga",false) };
	float test_scale = 1.f / glm::max(test_model->size().x, glm::max(test_model->size().y, test_model->size().z));
	test_props.position = { -4.f,0.f, -5.f };
	test_props.scale = glm::vec3(test_scale);
	test_props.bounding_shape = test_model->size() / 2.f * test_scale;
	m_test_obj = engine::game_object::create(test_props);*/

	std::string path = "assets/models/static/dungeon/";
	std::string extn = ".obj";
	//Display all dungeon pieces
	//generate_all_level_pieces(m_level_segments,path,extn);

	//Populate level grid, and test its functionality.
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
	m_level_grid.set_corner(15, 15, orientation::south_east);
	glm::vec3 center = m_level_grid.grid_to_world_coords(7,7) + glm::vec3(m_level_grid.cell_size()/2,0,m_level_grid.cell_size()/2);//The point in the center of the center grid square
	m_level_grid.set_corner(7, 7, orientation::north_east);
	m_level_grid.set_corner(7, 7, orientation::south_east);
	m_level_grid.set_corner(7, 7, orientation::south_west);
	m_level_grid.set_corner(7, 7, orientation::north_west);

	m_level_grid.set_gateway(13, 14, orientation::north_east);
	m_level_grid.set_gateway(13, 14, orientation::north_west,(float)M_PI);
	m_level_grid.del_border(13, 14, orientation::north);

	m_level_grid.set_gateway(1, 0, orientation::south_east);
	m_level_grid.set_gateway(1, 0, orientation::south_west,(float)M_PI);
	m_level_grid.del_border(1, 0, orientation::south);

	m_level_grid.bake_tiles();

	m_level_grid.place_block(10, 10);
	m_level_grid.place_block(11, 10);
	m_level_grid.place_block(12, 10);
	m_level_grid.remove_block(12, 10);
	m_level_grid.place_block(10, 9);
	m_level_grid.place_block(11, 9);
	m_level_grid.place_block(10, 8);
	m_level_grid.place_block(11, 8);
	m_level_grid.place_block(9, 9);
	m_level_grid.place_block(9, 8);
	m_level_grid.place_block(9, 10);
	//end of grid testing.

	//re-center terrain.
	m_terrain->set_position({ center.x,-terrain_dimensions.y,center.z });

	//Initialise intro screen
	intro_screen::init(m_3d_camera,center);

	//Primitive shapes
	{
		engine::ref<engine::texture_2d> rhombi_texture_sqr = engine::texture_2d::create("assets/textures/rhombi_face_sqr.png", true);//These textures made by me
		engine::ref<engine::texture_2d> rhombi_texture_tri = engine::texture_2d::create("assets/textures/rhombi_face_tri.png", true);
		engine::ref<engine::rhombicuboctahedron> rhombi_shape = engine::rhombicuboctahedron::create();
		engine::game_object_properties shape_props;
		shape_props.position = { -10.f, 2.f, -10.f };
		shape_props.meshes = rhombi_shape->meshes();
		shape_props.textures = { rhombi_texture_sqr ,rhombi_texture_tri };
		shape_props.bounding_shape = glm::vec3(1.f);
		m_rhombi = engine::game_object::create(shape_props);

		engine::ref<engine::texture_2d> pyr_texture = engine::texture_2d::create("assets/textures/pyramid_face.png", true);
		engine::ref<engine::texture_2d> pyr_texture_border = engine::texture_2d::create("assets/textures/pyramid_border.png", true);
		engine::ref<engine::stepped_pyramid> pyr_shape = engine::stepped_pyramid::create(15.f, 1.f, 19.f, 8,0.1f,4);
		shape_props.meshes = pyr_shape->meshes();
		shape_props.textures = { pyr_texture, pyr_texture_border };
		shape_props.bounding_shape = glm::vec3(5.f);
		shape_props.position = m_terrain->position() - glm::vec3(0,terrain_dimensions.y,0);
		shape_props.rotation_axis = { 0,0,1 };
		shape_props.rotation_amount = (float)M_PI;
		m_decorational_objects.push_back(engine::game_object::create(shape_props));
	}

	//Create pickups
	engine::ref <engine::model> pickup_model = engine::model::create("assets/models/static/gold/gold_05_modified.obj");	
	engine::game_object_properties pickup_props;
	pickup_props.meshes = pickup_model->meshes();
	pickup_props.textures = { engine::texture_2d::create("assets/models/static/gold/g_diffuse.tga",false) };
	float pickup_scale = .5f / glm::max(pickup_model->size().x, glm::max(pickup_model->size().y, pickup_model->size().z));
	pickup_props.scale = glm::vec3(pickup_scale);
	pickup_props.bounding_shape = pickup_model->size() / 2.f * pickup_scale;
	pickup_props.rotation_axis = { 0,1,0 };
	pickup_props.position = center;
	m_pickups.push_back(pickup(engine::game_object::create(pickup_props),1.f));
	pickup_props.position = center + glm::vec3(1, 0, 0);
	m_pickups.push_back(pickup(engine::game_object::create(pickup_props), -1.f));
	pickup_props.position = center + glm::vec3(-1, 0, 0);
	m_pickups.push_back(pickup(engine::game_object::create(pickup_props), -1.f));
	pickup_props.position = center + glm::vec3(0, 0, 1);
	m_pickups.push_back(pickup(engine::game_object::create(pickup_props), -1.f));
	pickup_props.position = center + glm::vec3(0, 0, -1);
	m_pickups.push_back(pickup(engine::game_object::create(pickup_props), -1.f));

	//Create text manager
	m_text_manager = engine::text_manager::create();

	//start fps timer
	m_fps_timer.start();
}

game_layer::~game_layer()
{
}

void game_layer::on_update(const engine::timestep& time_step)
{
	//increase rhombi angle
	float tmp = m_rhombi_angle;
	m_rhombi_angle = (float) fmod(m_rhombi_angle + 0.05f, 2 * M_PI);
	tmp = (float) fmod(tmp + 0.025f, 2 * M_PI);
	m_rhombi_trig_vector = 1.5f*glm::vec3(sin(m_rhombi_angle),cos(m_rhombi_angle),sin(tmp));

	//Crude sun simulation. TODO move into sepparate class to manage day/night cycle and other lights.
	//m_directionalLight.Direction = glm::rotateZ(m_directionalLight.Direction,(float)(M_PI /60)*time_step);

	//Update the displayed fps counter every second.
	//REF NOTE: inspired by http://www.opengl-tutorial.org/miscellaneous/an-fps-counter/
	m_num_updates++;
	if (m_fps_timer.total() > 1.f)
	{
		m_updates_last_interval = m_num_updates;
		m_num_updates = 0;
		m_fps_timer.reset();
	}

	for (auto& pick: m_pickups)
	{
		pick.on_update(time_step);
	}

	//Most of the update work does not occur during the intro screen
	if (intro_screen::active())
	{
		intro_screen::update(m_3d_camera, time_step);
	}
	else {
		//Freecam or play-based movement
		if (m_freecam)
		{
			m_3d_camera.on_update(time_step);
		}
		else {
			m_player.on_update(time_step);
			m_player.update_camera(m_3d_camera);
		}
	}
}

void game_layer::on_render()
{
	const auto cam_pos = m_3d_camera.position();

	engine::render_command::clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });
	engine::render_command::clear();

	//------------
	// Meshes
	//------------
	const auto textured_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");
	engine::renderer::begin_scene(m_3d_camera, textured_lighting_shader);

	// Set up some of the scene's parameters in the shader
	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	//update the directional light for this shader
	m_directionalLight.submit(textured_lighting_shader);

	// Position the skybox centred on the player and render it
	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_camera.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(textured_lighting_shader, m_skybox, skybox_tranform);

	//Render terrain
	engine::renderer::submit(textured_lighting_shader, m_terrain);

	//render all children of the level grid
	m_level_grid.render(textured_lighting_shader);

	//render all pickups
	for (auto& pick : m_pickups)
	{
		engine::renderer::submit(textured_lighting_shader, pick.object());
	}

	//Render multiple transformed, scaled and rotated rhombicuboctahedrons in the scene.
	{
		//over door 1
		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, glm::vec3(3.f,5.f , 0.0f));
		transform = glm::rotate(transform,m_rhombi_angle, glm::vec3(1,0,0) );
		transform = glm::scale(transform,0.5f * glm::vec3(1.f));
		engine::renderer::submit(textured_lighting_shader, transform, m_rhombi);

		//over door 2
		transform = glm::mat4(1.f);
		transform = glm::translate(transform, glm::vec3(27.f, 5.f, 30.0f));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(1, 0, 0));
		transform = glm::scale(transform, 0.5f * glm::vec3(1.f));
		engine::renderer::submit(textured_lighting_shader, transform, m_rhombi);

		//Large, centered
		transform = glm::mat4(1.f);
		transform = glm::translate(transform, glm::vec3(15.f, 10.f, 15.0f));		
		transform = glm::scale(transform, glm::vec3(1.f));
		engine::renderer::submit(textured_lighting_shader, transform, m_rhombi);

		//rotating around center
		transform = glm::mat4(1.f);
		transform = glm::translate(transform, glm::vec3(15.f+m_rhombi_trig_vector.x, 10.f+m_rhombi_trig_vector.y, 15.0f + m_rhombi_trig_vector.z));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(0, 0, 1));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(0, 1, 0));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(1,0,0));
		transform = glm::scale(transform, 0.25f * glm::vec3(1.f));
		engine::renderer::submit(textured_lighting_shader, transform, m_rhombi);

		//rotating around center
		transform = glm::mat4(1.f);
		transform = glm::translate(transform, glm::vec3(15.f + 1.5f*m_rhombi_trig_vector.x, 10.f + 1.5f * m_rhombi_trig_vector.z, 15.0f+ 1.5f * m_rhombi_trig_vector.y));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(0, 0, 1));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(0, 1, 0));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(1, 0, 0));
		transform = glm::scale(transform, 0.25f * glm::vec3(1.f));
		engine::renderer::submit(textured_lighting_shader, transform, m_rhombi);

		//rotating around center
		transform = glm::mat4(1.f);
		transform = glm::translate(transform, glm::vec3(15.f + -2.f * m_rhombi_trig_vector.z, 10.f + 2.f * m_rhombi_trig_vector.x, 15.0f + 2.f *m_rhombi_trig_vector.y));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(0, 0, 1));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(0, 1, 0));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(1, 0, 0));
		transform = glm::scale(transform, 0.25f * glm::vec3(1.f));
		engine::renderer::submit(textured_lighting_shader, transform, m_rhombi);

	}

	//Render test object
	/*engine::renderer::submit(textured_lighting_shader,m_test_obj);*/
	for (auto& obj : m_decorational_objects) {
		engine::renderer::submit(textured_lighting_shader, obj);
	}

	engine::renderer::end_scene();

	//------------
	// Custom materials
	//------------
	// Set up material shader. (does not render textures, renders materials instead)
	const auto textured_material_shader = engine::renderer::shaders_library()->get("mesh_material");
	engine::renderer::begin_scene(m_3d_camera, textured_material_shader);

	m_material->submit(textured_material_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(textured_material_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	//update the directional light for this shader
	m_directionalLight.submit(textured_material_shader);

	//------------
	// Animated meshes
	//------------
	//Render animated meshes
	const auto animated_mesh_shader = engine::renderer::shaders_library()->get("animated_mesh");
	engine::renderer::begin_scene(m_3d_camera, animated_mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());
	//update the directional light for this shader
	m_directionalLight.submit(animated_mesh_shader);

	//Render the player object
	engine::renderer::submit(animated_mesh_shader, m_player.object());

	engine::renderer::end_scene();


	// Render text
	if (intro_screen::active())
	{
		intro_screen::render(m_text_manager);
	}
	else {
		const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
		m_text_manager->render_text(text_shader, "Placeholder UI", 10.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
		//Debug ui, including camera position and facing, and FPS
		if (m_show_debug)
		{
			m_text_manager->render_text(text_shader, "Pos:{" + std::to_string(cam_pos.x) + "," + std::to_string(cam_pos.y)
				+ "," + std::to_string(cam_pos.z) + "}", 10.f, (float)engine::application::window().height() - 50.f, .5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
			m_text_manager->render_text(text_shader, "FPS:{" + std::to_string(m_updates_last_interval) + "}", 10.f, (float)engine::application::window().height() - 75.f, .5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
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
			event.handled = true;
			break;
		case engine::key_codes::KEY_F1://Toggle debug
			m_show_debug = !m_show_debug;
			event.handled = true;
			break;
		case engine::key_codes::KEY_F2://Toggle freecam
			m_freecam = !m_freecam;
			event.handled = true;
			break;
		case engine::key_codes::KEY_ENTER://Deactivate intro screen if active.	
			if (intro_screen::active())
			{
				intro_screen::active(false);
				m_3d_camera.position(m_camera_start_pos);
				m_3d_camera.face(orientation::north.to_vec());
				event.handled = true;
			}
			break;			
		default:
			break;
		}		
	}
	else if (!intro_screen::active() && event.event_type() == engine::event_type_e::mouse_button_pressed)
	{//Mouse button handling when intro screen is not active.
		auto& e = dynamic_cast<engine::mouse_button_pressed_event&>(event);
		switch (e.mouse_button())
		{
		case engine::mouse_button_codes::MOUSE_BUTTON_1:
			mouse1_event_handler();
			break;
		case engine::mouse_button_codes::MOUSE_BUTTON_2:
			mouse2_event_handler();
			break;
		default:
			break;
		}
	}

	//If the event's not already handled, pass it on to child objects of this layer.
	if (!event.handled)
	{
		//Currently only player deals with events.
		m_player.on_event(event);
	}
}

/*
TODO: Add logic for weapons as well, and switching between build and combat mode.
Refactor duplicated code.
*/
//Remove a block at the targeted position (if possible)
void game_layer::mouse1_event_handler()
{
	auto fv = m_3d_camera.front_vector();
	if (fv.y > 0)
	{
		//Not looking at the ground
		return;
	}
	auto cam_pos = m_3d_camera.position();
	auto delta_y = m_terrain->position().y - cam_pos.y;
	auto ground_pos = cam_pos + (delta_y / fv.y) * fv;
	auto grid_coords = m_level_grid.world_to_grid_coords(ground_pos);
	m_level_grid.remove_block(grid_coords.first,grid_coords.second);
}

//Place a block at the targeted position if posible
void game_layer::mouse2_event_handler()
{
	auto fv = m_3d_camera.front_vector();
	if (fv.y > 0)
	{
		//Not looking at the ground
		return;
	}
	auto cam_pos = m_3d_camera.position();
	auto delta_y = m_terrain->position().y - cam_pos.y;
	auto ground_pos = cam_pos + (delta_y / fv.y) * fv;
	auto grid_coords = m_level_grid.world_to_grid_coords(ground_pos);
	m_level_grid.place_block(grid_coords.first, grid_coords.second);
}

/*
Generate gameobjects for all the models in a directory and arrange them in the scene.
Only used for debug/dev purposes, to visualise all the models together.
*/
void game_layer::generate_all_level_pieces(std::vector<engine::ref<engine::game_object>>& level_segments,const std::string& path,const std::string& extn) {
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
REF https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
*/
void game_layer::get_all_models_in_directory(std::vector < std::string >& vec,const std::string& path,const std::string& extension) {
	for (const auto& entry : fs::directory_iterator(path)) {
		if (extension == entry.path().extension().string())
		{
			vec.push_back(entry.path().filename().replace_extension().string());
		}
	}
}
