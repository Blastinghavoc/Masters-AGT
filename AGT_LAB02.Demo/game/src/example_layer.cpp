#include "example_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"

#include "pickup.h"

#define _USE_MATH_DEFINES
#include <math.h>

example_layer::example_layer() 
    :m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f), 
    m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height()),
	m_loading_screen(true)
{
    // Hide the mouse and lock it inside the window
    //engine::input::anchor_mouse(true);
    engine::application::window().hide_mouse_cursor();

	// Initialise audio and play background music
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	m_audio_manager->load_sound("assets/audio/bounce.wav", engine::sound_type::event, "bounce"); // Royalty free sound from freesound.org
	m_audio_manager->load_sound("assets/audio/DST-impuretechnology.mp3", engine::sound_type::track, "music");  // Royalty free music from http://www.nosoapradio.us/
	//m_audio_manager->play("music");


	// Initialise the shaders, materials and lights
    auto mesh_shader = engine::renderer::shaders_library()->get("mesh_static");
	auto mesh__material_shader = engine::renderer::shaders_library()->get("mesh_material");
	auto mesh_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

    std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind();
    std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("u_sampler", 0);

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("u_sampler", 0);

	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
		glm::ortho(0.f, (float)engine::application::window().width(), 0.f, (float)engine::application::window().height()));

	m_light = engine::light::create(glm::vec3(-60.f, 50.f, 40.f), glm::vec3(1.f, 1.f, 1.f), 0.1f, 1.f, 0.5f);
	m_material = engine::material::create(32.0f, glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f));

	//// Skybox texture from http://www.vwall.it/wp-content/plugins/canvasio3dpro/inc/resource/cubeMaps/
	//m_skybox = engine::skybox::create(50.f,
	//	{ engine::texture_2d::create("assets/textures/skybox/SkyboxFront.bmp"),
	//	  engine::texture_2d::create("assets/textures/skybox/SkyboxRight.bmp"),
	//	  engine::texture_2d::create("assets/textures/skybox/SkyboxBack.bmp"),
	//	  engine::texture_2d::create("assets/textures/skybox/SkyboxLeft.bmp"),
	//	  engine::texture_2d::create("assets/textures/skybox/SkyboxTop.bmp"),
	//	  engine::texture_2d::create("assets/textures/skybox/SkyboxBottom.bmp")
	//	});


	// Skybox texture from https://opengameart.org/content/mountain-skyboxes
	m_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create("assets/textures/skybox/mountain-skyboxes/Maskonaive2/posz.jpg"),
		  engine::texture_2d::create("assets/textures/skybox/mountain-skyboxes/Maskonaive2/posx.jpg"),
		  engine::texture_2d::create("assets/textures/skybox/mountain-skyboxes/Maskonaive2/negz.jpg"),
		  engine::texture_2d::create("assets/textures/skybox/mountain-skyboxes/Maskonaive2/negx.jpg"),
		  engine::texture_2d::create("assets/textures/skybox/mountain-skyboxes/Maskonaive2/posy.jpg"),
		  engine::texture_2d::create("assets/textures/skybox/mountain-skyboxes/Maskonaive2/negy.jpg")
		});

	// Load the terrain texture and create a terrain mesh. Create a terrain object. Set its properties
	//Asset from https://freestocktextures.com/texture/dry-and-cracked-soil,1015.html
	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/cracked_soil.jpg") };
	engine::ref<engine::terrain> terrain_shape = engine::terrain::create(100.f, 0.5f, 100.f);
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { terrain_shape->mesh() };
	terrain_props.textures = terrain_textures;
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.bounding_shape = glm::vec3(100.f, 0.5f, 100.f);
	terrain_props.restitution = 0.92f;
	m_terrain = engine::game_object::create(terrain_props);

	//Load the test model.
	engine::ref <engine::model> test_model = engine::model::create("assets/models/modular/Modular_Terrain_Hilly/Grass_Flat.obj");	
	engine::game_object_properties test_props;
	test_props.meshes = test_model->meshes();
	test_props.textures = test_model->textures();
	float test_scale = 1.f / glm::max(test_model->size().x, glm::max(test_model->size().y, test_model->size().z));
	test_props.position = { 0,2, 0 };
	test_props.scale = glm::vec3(test_scale);
	test_props.bounding_shape = test_model->size() / 2.f * test_scale;
	m_test_obj = engine::game_object::create(test_props);

	// Load the cow model. Create a cow object. Set its properties
	engine::ref <engine::model> cow_model = engine::model::create("assets/models/cow4.3ds");
	engine::game_object_properties cow_props;
	cow_props.meshes = cow_model->meshes();
	cow_props.textures = cow_model->textures();
	float cow_scale = 1.f / glm::max(cow_model->size().x, glm::max(cow_model->size().y, cow_model->size().z));
	cow_props.position = { -4.f,0.5f, -5.f };
	cow_props.scale = glm::vec3(cow_scale);
	cow_props.bounding_shape = cow_model->size() / 2.f * cow_scale;
	m_cow = engine::game_object::create(cow_props);

	// Load the tree model. Create a tree object. Set its properties
	engine::ref <engine::model> tree_model = engine::model::create("assets/models/elm.3ds");
	engine::game_object_properties tree_props;
	tree_props.meshes = tree_model->meshes();
	tree_props.textures = tree_model->textures();
	float tree_scale = 3.f / glm::max(tree_model->size().x, glm::max(tree_model->size().y, tree_model->size().z));
	tree_props.position = { 4.f, 0.5f, -5.f };
	tree_props.bounding_shape = tree_model->size() / 2.f * tree_scale;
	tree_props.scale = glm::vec3(tree_scale);
	m_tree = engine::game_object::create(tree_props);

	engine::ref<engine::sphere> sphere_shape = engine::sphere::create(10, 20, 0.5f);
	engine::game_object_properties sphere_props;
	sphere_props.position = { 0.f, 5.f, -5.f };
	sphere_props.meshes = { sphere_shape->mesh() };
	sphere_props.type = 1;
	sphere_props.bounding_shape = glm::vec3(0.5f);
	sphere_props.restitution = 0.92f;
	sphere_props.mass = 0.000001f;
	m_ball = engine::game_object::create(sphere_props);

	m_game_objects.push_back(m_terrain);
	m_game_objects.push_back(m_ball);
	//m_game_objects.push_back(m_cow);
	//m_game_objects.push_back(m_tree);
	//m_game_objects.push_back(m_pickup);
	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	m_text_manager = engine::text_manager::create();

	// Medkit texture from https://www.textures.com/download/manmadeboxes0007/105116
	engine::ref<engine::cuboid> pickup_shape = engine::cuboid::create(glm::vec3(0.5f), false);
	engine::ref<engine::texture_2d> pickup_texture = engine::texture_2d::create("assets/textures/medkit.jpg");
	engine::game_object_properties pickup_props;
	pickup_props.position = { 5.f, 1.f, 5.f };
	pickup_props.meshes = { pickup_shape->mesh() };
	pickup_props.textures = { pickup_texture };
	m_pickup = pickup::create(pickup_props);
	m_pickup->init();

	//Tetrahedron from lab 3
	std::vector<glm::vec3> tetrahedron_vertices;
	tetrahedron_vertices.push_back(glm::vec3(0.f, 10.f, 0.f));//0
	tetrahedron_vertices.push_back(glm::vec3(0.f, 0.f, 10.f));//1
	tetrahedron_vertices.push_back(glm::vec3(-10.f, 0.f, -10.f)); //2
	tetrahedron_vertices.push_back(glm::vec3(10.f, 0.f, -10.f)); //3
	engine::ref<engine::tetrahedron> tetrahedron_shape = engine::tetrahedron::create(tetrahedron_vertices);
	engine::game_object_properties tetrahedron_props;

	tetrahedron_props.position = { 0.f, 0.5f, -20.f };
	tetrahedron_props.meshes = { tetrahedron_shape-> mesh() };
	m_tetrahedron = engine::game_object::create(tetrahedron_props);

	//Back the camera up
	m_3d_camera.position({ 0,5,10 });
}

example_layer::~example_layer() {}

void example_layer::on_update(const engine::timestep& time_step) 
{
	if (m_loading_screen)
	{
		update_camera_for_loading_screen(time_step);
	}
	else {
		m_3d_camera.on_update(time_step);
	}

	m_pickup->update(m_3d_camera.position(), time_step);

	m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));
}

void example_layer::update_camera_for_loading_screen(const engine::timestep& time_step) {
	m_3d_camera.look_at(glm::vec3(0,0,0)-m_3d_camera.position());
	m_3d_camera.move(engine::perspective_camera::e_direction::left, time_step);
}

void example_layer::on_render() 
{
    engine::render_command::clear_color({0.2f, 0.3f, 0.3f, 1.0f}); 
    engine::render_command::clear();

    //const auto textured_shader = engine::renderer::shaders_library()->get("mesh_static");
    //engine::renderer::begin_scene(m_3d_camera, textured_shader);

	const auto textured_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");
	engine::renderer::begin_scene(m_3d_camera, textured_lighting_shader);

	// Set up some of the scene's parameters in the shader
	m_light->submit(textured_lighting_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("view_pos", m_3d_camera.position());
	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("shininess", 1.0f);

	// Position the skybox centred on the player and render it
	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_camera.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(textured_lighting_shader, m_skybox, skybox_tranform);

	engine::renderer::submit(textured_lighting_shader, m_terrain);
	engine::renderer::submit(textured_lighting_shader, m_tree);

	glm::mat4 cow_transform(1.0f);
	cow_transform = glm::translate(cow_transform, m_cow->position());
	cow_transform = glm::rotate(cow_transform, m_cow->rotation_amount(), m_cow->rotation_axis());
	cow_transform = glm::scale(cow_transform, m_cow->scale());
	engine::renderer::submit(textured_lighting_shader, cow_transform, m_cow);

	//Reset matrix for another cow!
	cow_transform = glm::mat4(1.f);
	cow_transform = glm::translate(cow_transform, m_cow->position()+glm::vec3(0,.5f,0));
	cow_transform = glm::rotate(cow_transform, m_cow->rotation_amount(), m_cow->rotation_axis());
	cow_transform = glm::scale(cow_transform, m_cow->scale());
	engine::renderer::submit(textured_lighting_shader, cow_transform, m_cow);

	//Reset matrix for the creepy staring cow.
	cow_transform = glm::mat4(1.f);
	cow_transform = glm::translate(cow_transform,glm::vec3(0, 1.f, 5.f));

	auto cow_pos = glm::vec3(0, .5f, 5.f);
	auto cam_pos = m_3d_camera.position();
	auto cow_to_cam_vec = cam_pos - cow_pos;

	float y_angle = atan2(cow_to_cam_vec.x, cow_to_cam_vec.z);

	//REF: https://stackoverflow.com/questions/26555040/yaw-pitch-and-roll-to-glmrotate
	float projectionLength = std::sqrt(cow_to_cam_vec.x * cow_to_cam_vec.x + cow_to_cam_vec.z * cow_to_cam_vec.z);
	float pitch = std::atan2(cow_to_cam_vec.y, projectionLength);

	cow_transform = glm::rotate(cow_transform, y_angle, glm::vec3(0,1,0));
	cow_transform = glm::rotate(cow_transform, -pitch, glm::vec3(1, 0, 0));
	cow_transform = glm::scale(cow_transform, m_cow->scale());
	engine::renderer::submit(textured_lighting_shader, cow_transform, m_cow);


	//Pickup stuff
	if (m_pickup->active())
	{
		m_pickup->textures().at(0)->bind();
		glm::mat4 pickup_transform(1.0f);
		pickup_transform = glm::translate(pickup_transform, m_pickup->position());
		pickup_transform = glm::rotate(pickup_transform, m_pickup->rotation_amount(), m_pickup->rotation_axis());
		engine::renderer::submit(textured_lighting_shader, m_pickup->meshes().at(0), pickup_transform);
	}

	//Render test object
	glm::mat4 test_transform(1.0f);
	test_transform = glm::translate(test_transform, m_test_obj->position());
	test_transform = glm::rotate(test_transform, m_test_obj->rotation_amount(), m_test_obj->rotation_axis());
	test_transform = glm::scale(test_transform, m_test_obj->scale());
	engine::renderer::submit(textured_lighting_shader, test_transform, m_test_obj);


    engine::renderer::end_scene();

	// Set up material shader. (does not render textures, renders materials instead)
	const auto textured_material_shader = engine::renderer::shaders_library()->get("mesh_material");
	engine::renderer::begin_scene(m_3d_camera, textured_material_shader);

	m_light->submit(textured_material_shader);
	m_material->submit(textured_material_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(textured_material_shader)->set_uniform("view_pos", m_3d_camera.position());

	engine::renderer::submit(textured_material_shader, m_ball);

	//tetrahedron
	engine::renderer::submit(textured_material_shader, m_tetrahedron);

	engine::renderer::end_scene();

	// Render text
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	m_text_manager->render_text(text_shader, "Orange Text", 10.f, (float)engine::application::window().height()-25.f, 0.5f, glm::vec3(1.f,0.5f,0.f));
} 

void example_layer::on_event(engine::event& event) 
{ 
    if(event.event_type() == engine::event_type_e::key_pressed) 
    { 
        auto& e = dynamic_cast<engine::key_pressed_event&>(event); 
        if(e.key_code() == engine::key_codes::KEY_TAB) 
        { 
            engine::render_command::toggle_wireframe();
        }

		if (e.key_code() == engine::key_codes::KEY_ENTER)
		{
			m_loading_screen = false;
			//Reset camera
			m_3d_camera = engine::perspective_camera((float)engine::application::window().width(), (float)engine::application::window().height());
		}
    } 
}
