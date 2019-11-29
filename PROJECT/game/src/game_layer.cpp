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

#include "btBulletDynamicsCommon.h"//For bt rigid body?

#include "sfx/sfx_manager.h"
#include "physics/physics_manager.h"
#include "gameplay/weapon_manager.h"
#include "gameplay/pickup_manager.h"
#include "gameplay/projectile_manager.h"


game_layer::game_layer() :
	//m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f),
	m_2d_camera(0, (float)engine::application::window().width(), 0, (float)engine::application::window().height()),
	m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height(), 45.f, 0.1f, 200.f)
{
	//Initialise the level grid.
	m_level_grid = std::make_shared<grid>(2.f, 0.01f);

	// Hide the mouse and lock it inside the window
	//engine::input::anchor_mouse(true);
	engine::application::window().hide_mouse_cursor();

	// Initialise audio and play background music
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	//Music from https://freesound.org/people/orangefreesounds/sounds/262940/
	m_audio_manager->load_sound("assets/audio/ambience.wav", engine::sound_type::track, "music");
	m_audio_manager->play("music");
	//Error sound from https://freesound.org/people/Autistic%20Lucario/sounds/142608/
	m_audio_manager->load_sound("assets/audio/error_sound.wav", engine::sound_type::event, "error");
	//Alert from https://freesound.org/people/willy_ineedthatapp_com/sounds/167337/
	m_audio_manager->load_sound("assets/audio/8_bit_alert.mp3", engine::sound_type::event, "alert");
	//Sound from https://freesound.org/people/TiesWijnen/sounds/338722/
	m_audio_manager->load_sound("assets/audio/grenade_explosion.mp3", engine::sound_type::spatialised, "grenade_explosion");
	//Sound from https://freesound.org/people/LeMudCrab/sounds/163458/
	m_audio_manager->load_sound("assets/audio/grenade_launch.wav", engine::sound_type::event, "grenade_launch");
	//Sound from https://freesound.org/people/wcoltd/sounds/417731/
	m_audio_manager->load_sound("assets/audio/laser.wav", engine::sound_type::spatialised, "laser");

	physics_manager::init();


	// Initialise the shaders, materials, and lights
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	auto mesh_material_shader = engine::renderer::shaders_library()->get("mesh_material");
	auto mesh_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");
	auto animated_mesh_shader = engine::renderer::shaders_library()->get("animated_mesh");

	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 0.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

	light_manager::init(&m_directionalLight);
	light_manager::day_duration = 60.f;

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
		{ engine::texture_2d::create(skybox_path + "bkg1_front" + skybox_extn,true),
		  engine::texture_2d::create(skybox_path + "bkg1_right" + skybox_extn,true),
		  engine::texture_2d::create(skybox_path + "bkg1_back" + skybox_extn,true),
		  engine::texture_2d::create(skybox_path + "bkg1_left" + skybox_extn,true),
		  engine::texture_2d::create(skybox_path + "bkg1_top" + skybox_extn,true),
		  engine::texture_2d::create(skybox_path + "bkg1_bot" + skybox_extn,true)
		});

	int max_grid_dimension = 11;
	auto cell_size = m_level_grid->cell_size();

	// Load the terrain texture and create a terrain mesh. Create a terrain object. Set its properties
	// Using my tiled_cuboid instead of the original terrain shape in order to have tiled textures.
	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/terrain_grid.bmp",false) };//Texture by me
	glm::vec3 terrain_dimensions{ max_grid_dimension + 2 * cell_size, 1.f, max_grid_dimension + 2 * cell_size };
	engine::ref<engine::tiled_cuboid> terrain_shape = engine::tiled_cuboid::create(terrain_dimensions, false, m_level_grid->cell_size());
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { terrain_shape->mesh() };
	terrain_props.textures = terrain_textures;
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.bounding_shape = terrain_dimensions;
	terrain_props.restitution = 0.92f;
	terrain_props.position = { 0,-terrain_dimensions.y,0 };
	m_terrain = engine::game_object::create(terrain_props);

	//---Populate level grid
	for (int i = 0; i < max_grid_dimension; i++)
	{
		m_level_grid->set_border(0,i,orientation::east);
		m_level_grid->set_border(max_grid_dimension-1, i, orientation::west);
		m_level_grid->set_border(i, 0, orientation::south);
		m_level_grid->set_border(i, max_grid_dimension-1, orientation::north);

		m_level_grid->set_state(max_grid_dimension, i, grid_tile::tile_state::border,true);

		m_level_grid->set_state(i,max_grid_dimension, grid_tile::tile_state::border,true);
		
		for (int j = 0; j < max_grid_dimension; j++)
		{
			//Forcibly create empty tiles
			m_level_grid->set_state(i, j,grid_tile::tile_state::empty,true);
		}
	}
	m_level_grid->set_corner(0, 0, orientation::south_east);
	m_level_grid->set_corner(max_grid_dimension, 0, orientation::south_east);
	m_level_grid->set_corner(0, max_grid_dimension, orientation::south_east);
	m_level_grid->set_corner(max_grid_dimension, max_grid_dimension, orientation::south_east);
	glm::vec3 center = m_level_grid->center_of(max_grid_dimension / 2, max_grid_dimension / 2);//The point in the center of the center grid square
	m_grid_center = center;

	m_level_grid->set_gateway(max_grid_dimension-2, max_grid_dimension-1, orientation::north_east);
	m_level_grid->set_gateway(max_grid_dimension-2, max_grid_dimension-1, orientation::north_west,(float)M_PI);
	m_level_grid->del_border(max_grid_dimension-2, max_grid_dimension-1, orientation::north);
	m_level_grid->set_start(max_grid_dimension - 2, max_grid_dimension);

	m_level_grid->set_gateway(1, 0, orientation::south_east);
	m_level_grid->set_gateway(1, 0, orientation::south_west,(float)M_PI);
	m_level_grid->del_border(1, 0, orientation::south);
	m_level_grid->set_end(1, -1);	

	//Add collision objects
	for (int i = 0; i < max_grid_dimension; i++)
	{
		for (int j = 0; j < max_grid_dimension; j++) {
			for (auto& obj : (*m_level_grid)[glm::vec2(i, j)].get_borders()) {
				physics_manager::add(obj);
			}
		}
	}

	//Save initial state of tiles
	m_level_grid->bake_tiles();


	//re-center terrain.
	m_terrain->set_position({ center.x,-terrain_dimensions.y,center.z });

	//Initialise intro screen
	intro_screen::init(m_3d_camera,center);

	//Primitive shapes
	{
		//rhombicuboctahedron
		engine::ref<engine::texture_2d> rhombi_texture_sqr = engine::texture_2d::create("assets/textures/rhombi_face_sqr.png", true);//These textures made by me
		engine::ref<engine::texture_2d> rhombi_texture_tri = engine::texture_2d::create("assets/textures/rhombi_face_tri.png", true);
		engine::ref<engine::rhombicuboctahedron> rhombi_shape = engine::rhombicuboctahedron::create();
		engine::game_object_properties shape_props;
		shape_props.position = { -10.f, 2.f, -10.f };
		shape_props.meshes = rhombi_shape->meshes();
		shape_props.textures = { rhombi_texture_sqr ,rhombi_texture_tri };
		shape_props.bounding_shape = glm::vec3(1.f);
		m_rhombi = engine::game_object::create(shape_props);

		//stepped pyramid
		engine::ref<engine::texture_2d> pyr_texture = engine::texture_2d::create("assets/textures/pale_brick.png", false);
		engine::ref<engine::texture_2d> pyr_texture_border = engine::texture_2d::create("assets/textures/pyramid_border.png", true);
		engine::ref<engine::stepped_pyramid> pyr_shape = engine::stepped_pyramid::create(15.f, 1.f, terrain_dimensions.x, 8, 0.1f, 8, 4.f);
		shape_props.meshes = pyr_shape->meshes();
		shape_props.textures = { pyr_texture, pyr_texture_border };
		shape_props.bounding_shape = glm::vec3(5.f);
		shape_props.position = m_terrain->position() - glm::vec3(0, terrain_dimensions.y, 0);
		shape_props.rotation_axis = { 0,0,1 };
		shape_props.rotation_amount = (float)M_PI;
		m_decorational_objects.push_back(engine::game_object::create(shape_props));

		//hollow cuboid
		engine::ref<engine::texture_2d> hollow_texture = engine::texture_2d::create("assets/textures/funky_cube.png", false);
		float size = 1.1f;
		engine::ref<engine::hollow_cuboid> hollow_shape = engine::hollow_cuboid::create({ size,size,size }, size / 2, 2 * size);
		shape_props.meshes = hollow_shape->meshes();
		shape_props.textures = { hollow_texture };
		shape_props.bounding_shape = glm::vec3(5.f);
		shape_props.position = center + glm::vec3(0, m_big_decor_height, 0);
		shape_props.rotation_amount = (float)M_PI;
		shape_props.rotation_axis = { 1,1,1 };
		m_decorational_objects.push_back(engine::game_object::create(shape_props));

		//Arch portals		
		engine::ref<engine::texture_2d> arch_tex = engine::texture_2d::create("assets/textures/portal.png", true);
		engine::ref < engine::arch_pane > arch_shape = engine::arch_pane::create(2, 2.2f, 1.2f);
		shape_props.meshes = { arch_shape->mesh() };
		shape_props.position = m_level_grid->center_of(m_level_grid->get_start()) + glm::vec3(0,0,-m_level_grid->cell_size()/2);
		shape_props.rotation_amount = 0;
		shape_props.textures = {arch_tex};
		m_transparent_decorational_objects.push_back({0.7f ,engine::game_object::create(shape_props) });
		shape_props.position = m_level_grid->center_of(m_level_grid->get_end()) + glm::vec3(0,0,m_level_grid->cell_size()/2);
		m_transparent_decorational_objects.push_back({0.7f ,engine::game_object::create(shape_props) });
	}

	//Create text manager
	m_text_manager = engine::text_manager::create();

	//start fps timer
	m_fps_timer.start();
	
	//------
	//Extra Lights
	//------
	engine::ref<engine::PointLight> point_light_1 = std::make_shared<engine::PointLight>();
	point_light_1->Color = glm::vec3(.25f, .75f, .5f);
	point_light_1->AmbientIntensity = 0.25f;
	point_light_1->DiffuseIntensity = 0.4f;
	point_light_1->Position = glm::vec3(cell_size * 1.5f, 5, 0);
	point_light_1->Attenuation.Constant = .001f;
	point_light_1->Attenuation.Linear = .1f;
	point_light_1->Attenuation.Exp = .01f;

	engine::ref<engine::PointLight> point_light_2 = std::make_shared<engine::PointLight>();
	*point_light_2 = *point_light_1;
	point_light_2->Position = glm::vec3(cell_size * max_grid_dimension - cell_size * 1.5f, 5, cell_size * max_grid_dimension);

	engine::ref<engine::PointLight> point_light_3 = std::make_shared<engine::PointLight>();
	*point_light_3 = *point_light_1;
	point_light_3->Position = center + glm::vec3(0, m_big_decor_height, 0);
	point_light_3->Attenuation.Constant = 0.01f;

	light_manager::point_lights.push_back(point_light_1);
	light_manager::point_lights.push_back(point_light_2);
	light_manager::point_lights.push_back(point_light_3);



	physics_manager::add(m_player.object());
	physics_manager::add(m_terrain);		

	turret_manager::init();
	enemy_manager::init(m_level_grid);
	gameplay_manager::init(&m_player,m_text_manager,&m_3d_camera,m_level_grid,m_audio_manager,&m_decorational_objects);
	sfx_manager::init(&m_3d_camera);
	weapon_manager::init();
	pickup_manager::init();
	projectile_manager::init();	
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
	m_rhombi_trig_vector = 2.f*glm::vec3(sin(m_rhombi_angle),cos(m_rhombi_angle),sin(tmp));

	light_manager::on_update(time_step);

	//Update the displayed fps counter every second.
	//REF NOTE: inspired by http://www.opengl-tutorial.org/miscellaneous/an-fps-counter/
	m_num_updates++;
	if (m_fps_timer.total() > 1.f)
	{
		m_updates_last_interval = m_num_updates;
		m_num_updates = 0;
		m_fps_timer.reset();
	}	

	//Most of the update work does not occur during the intro screen
	if (intro_screen::active())
	{
		intro_screen::update(m_3d_camera, time_step);
	}
	else {
		enemy_manager::on_update(time_step);
		turret_manager::update(time_step);

		//Freecam or player-based movement
		if (m_freecam)
		{
			m_3d_camera.on_update(time_step);
		}
		else {
			m_player.update_camera(m_3d_camera);
			m_player.on_update(time_step);
		}		
		pickup_manager::on_update(time_step, m_player.get_trigger_box());
		gameplay_manager::update(time_step);
		sfx_manager::on_update(time_step);

		projectile_manager::on_update(time_step);

		physics_manager::update(time_step);
		//Update for 3D audio effects
		m_audio_manager->update_with_camera(m_3d_camera);
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

	//update the lighting for this shader
	light_manager::submit(textured_lighting_shader);

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
	m_level_grid->render(textured_lighting_shader);

	//render all turrets
	turret_manager::render(textured_lighting_shader);

	//render all pickups
	pickup_manager::render(textured_lighting_shader);

	//Render multiple transformed, scaled and rotated rhombicuboctahedrons in the scene.
	{
		//over door 1
		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, light_manager::point_lights[0]->Position);
		transform = glm::rotate(transform,m_rhombi_angle, glm::vec3(1,0,0) );
		transform = glm::scale(transform,0.5f * glm::vec3(1.f));
		engine::renderer::submit(textured_lighting_shader, transform, m_rhombi);

		//over door 2
		transform = glm::mat4(1.f);
		transform = glm::translate(transform, light_manager::point_lights[1]->Position);
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(1, 0, 0));
		transform = glm::scale(transform, 0.5f * glm::vec3(1.f));
		engine::renderer::submit(textured_lighting_shader, transform, m_rhombi);

		auto big_center_position = light_manager::point_lights[2]->Position;
		//Large, centered
		transform = glm::mat4(1.f);
		transform = glm::translate(transform, big_center_position);
		transform = glm::scale(transform, glm::vec3(.5f));
		engine::renderer::submit(textured_lighting_shader, transform, m_rhombi);

		//rotating around center
		transform = glm::mat4(1.f);
		transform = glm::translate(transform, big_center_position+glm::vec3(m_rhombi_trig_vector.x, m_rhombi_trig_vector.y,m_rhombi_trig_vector.z));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(0, 0, 1));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(0, 1, 0));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(1,0,0));
		transform = glm::scale(transform, 0.25f * glm::vec3(1.f));
		engine::renderer::submit(textured_lighting_shader, transform, m_rhombi);

		//rotating around center
		transform = glm::mat4(1.f);
		transform = glm::translate(transform, big_center_position+glm::vec3(1.5f*m_rhombi_trig_vector.x,1.5f * m_rhombi_trig_vector.z, 1.5f * m_rhombi_trig_vector.y));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(0, 0, 1));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(0, 1, 0));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(1, 0, 0));
		transform = glm::scale(transform, 0.25f * glm::vec3(1.f));
		engine::renderer::submit(textured_lighting_shader, transform, m_rhombi);

		//rotating around center
		transform = glm::mat4(1.f);
		transform = glm::translate(transform, big_center_position+glm::vec3(-2.f * m_rhombi_trig_vector.z,2.f * m_rhombi_trig_vector.x, 2.f *m_rhombi_trig_vector.y));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(0, 0, 1));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(0, 1, 0));
		transform = glm::rotate(transform, -m_rhombi_angle, glm::vec3(1, 0, 0));
		transform = glm::scale(transform, 0.25f * glm::vec3(1.f));
		engine::renderer::submit(textured_lighting_shader, transform, m_rhombi);

	}

	//Render non-animated enemies
	enemy_manager::render_static(textured_lighting_shader);

	//Render decorational objects
	for (auto& obj : m_decorational_objects) {		
		engine::renderer::submit(textured_lighting_shader, obj);
	}

	projectile_manager::on_render(textured_lighting_shader);

	//Render weapons
	weapon_manager::on_render(textured_lighting_shader);

	engine::renderer::end_scene();

	//------------
	// Custom materials
	//------------
	// Set up material shader. (does not render textures, renders materials instead)
	const auto textured_material_shader = engine::renderer::shaders_library()->get("mesh_material");
	engine::renderer::begin_scene(m_3d_camera, textured_material_shader);
	//update the lighting for this shader
	light_manager::submit(textured_material_shader);

	m_material->submit(textured_material_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(textured_material_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	//Render Bounding boxes
	//m_player.object()->render_obb({1,0,0},textured_material_shader);
	//m_terrain->render_obb({ 1,0,0 }, textured_material_shader);	
	//enemy_manager::render_trigger_boxes(textured_material_shader);
	//weapon_manager::render_trigger_boxes(textured_material_shader);

	//Use a small rhombi as a marker for spotlights
	for (auto& sl : light_manager::spot_lights)
	{
		if (sl->On)
		{
			glm::mat4 transform(1.0f);
			//positioned slightly under the light so it is illuminated nicely
			transform = glm::translate(transform, sl->Position - glm::vec3(0,0.6f,0));
			transform = glm::scale(transform, 0.125f * glm::vec3(1.f));

			m_material->set_ambient(sl->Color);
			m_material->set_diffuse(sl->Color);
			m_material->submit(textured_material_shader);
			engine::renderer::submit(textured_material_shader,transform, m_rhombi);
		}
	}	

	//------------
	// Animated meshes
	//------------
	//Render animated meshes
	const auto animated_mesh_shader = engine::renderer::shaders_library()->get("animated_mesh");
	engine::renderer::begin_scene(m_3d_camera, animated_mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	//update the lighting for this shader
	light_manager::submit(animated_mesh_shader);

	//Render the player object
	m_player.on_render(animated_mesh_shader);	

	//Render animated enemies
	enemy_manager::render_animated(animated_mesh_shader);

	engine::renderer::end_scene();

	//-----
	//Render anything with transparency
	//----
	engine::renderer::begin_scene(m_3d_camera, textured_lighting_shader);
	//Transparent decor
	for (auto& pair : m_transparent_decorational_objects) {
		std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("transparency", pair.first);
		engine::renderer::submit(textured_lighting_shader, pair.second);
	}
	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("transparency", 1.f);

	//Render textured SFX
	sfx_manager::on_render_textured(textured_lighting_shader, m_3d_camera);

	engine::renderer::end_scene();

	engine::renderer::begin_scene(m_3d_camera, textured_material_shader);
	//Render materialed SFX
	sfx_manager::on_render_material(textured_material_shader);	

	engine::renderer::end_scene();


	//----------
	// Render text
	//----------
	if (intro_screen::active())
	{
		intro_screen::render(m_text_manager);
	}
	else {
		const auto text_shader = engine::renderer::shaders_library()->get("text_2D");

		hud_manager::render(m_2d_camera,textured_lighting_shader);

		//Debug ui, including camera position and facing, and FPS
		if (m_show_debug)
		{
			m_text_manager->render_text(text_shader, "Pos:{" + std::to_string(cam_pos.x) + "," + std::to_string(cam_pos.y)
				+ "," + std::to_string(cam_pos.z) + "}", 10.f, (float)engine::application::window().height() - 200.f, .5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
			m_text_manager->render_text(text_shader, "FPS:{" + std::to_string(m_updates_last_interval) + "}", 10.f, (float)engine::application::window().height() - 225.f, .5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
			m_text_manager->render_text(text_shader, "Facing:{" +std::to_string(m_3d_camera.front_vector().x) + "," + std::to_string(m_3d_camera.front_vector().y)
				+ "," + std::to_string(m_3d_camera.front_vector().z) + "}", 10.f, (float)engine::application::window().height() - 250.f, .5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
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
				gameplay_manager::next_build_phase();//Start the game!
				event.handled = true;
			}
			break;
		default:
			break;
		}		
	}


	//If the event's not already handled, pass it on to child objects of this layer.
	if (!event.handled)
	{
		if (!intro_screen::active())
		{//Gameplay handling when intro screen is not active.
			gameplay_manager::on_event(event);
			
			m_player.on_event(event);
		}
	}
}
