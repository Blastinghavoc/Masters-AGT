#include "example_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"

example_layer::example_layer() 
    :m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f), 
    m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height())


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
	auto mesh__material_shader = engine::renderer::shaders_library()->get("mesh_material");
	auto mesh_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	auto animated_mesh_shader = engine::renderer::shaders_library()->get("animated_mesh");

	m_directionalLight.Color = glm::vec3(.5f, .5f, .5f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 0.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

	m_pointLight.Color = glm::vec3(.5f, .5f, .5f);
	m_pointLight.AmbientIntensity = 0.25f;
	m_pointLight.DiffuseIntensity = 0.6f;
	m_pointLight.Position = glm::vec3(0, 2, 0);
	m_pointLight.Attenuation.Constant = .100f;
	m_pointLight.Attenuation.Linear = .001f;
	m_pointLight.Attenuation.Exp = .001f;

	m_spotLight.Color = glm::vec3(.5f, .5f, .5f);
	m_spotLight.AmbientIntensity = 0.25f;
	m_spotLight.DiffuseIntensity = 0.6f;
	m_spotLight.Position = glm::vec3(5, 2, 0);
	m_spotLight.Direction = glm::vec3(0, -1, 1);
	m_spotLight.Cutoff = 0.1f;
	m_spotLight.Attenuation.Constant = .100f;
	m_spotLight.Attenuation.Linear = .1f;
	m_spotLight.Attenuation.Exp = .01f;

	// set color texture unit
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("lighting_on", true);
	m_directionalLight.submit(animated_mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gMatSpecularIntensity", 0.5f);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gSpecularPower", 5.f);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("transparency", 1.0f);


	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("lighting_on", true);
	//m_directionalLight.submit(mesh__material_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gMatSpecularIntensity", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gSpecularPower", 10.f);


	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("lighting_on", true);
	m_directionalLight.submit(mesh_lighting_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gMatSpecularIntensity", 0.5f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gSpecularPower", 5.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("transparency", 1.0f);

	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
		glm::ortho(0.f, (float)engine::application::window().width(), 0.f, (float)engine::application::window().height()));

	//Materials
	m_material = engine::material::create(1.f, glm::vec3(.10f, .75f, 0.07f), glm::vec3(.05f, 1.f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);
	m_lightsource_material = engine::material::create(1.f,m_pointLight.Color,m_pointLight.Color, glm::vec3(0.5f, 0.5f, 0.5f),1.f);
	m_ballistic_material = engine::material::create(1.f, { .1f,1.f,1.f }, { .1f,1.f,1.f }, { .5f,.5f,.5f }, 1.f);

	// Skybox texture from http://www.vwall.it/wp-content/plugins/canvasio3dpro/inc/resource/cubeMaps/
	m_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create("assets/textures/skybox/SkyboxFront.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxRight.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxBack.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxLeft.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxTop.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxBottom.bmp", true)
		});

	//
	
	//engine::ref<engine::skinned_mesh> m_skinned_mesh = engine::skinned_mesh::create("assets/models/animated/square/square.fbx");
	engine::ref<engine::skinned_mesh> m_skinned_mesh = engine::skinned_mesh::create("assets/models/animated/mannequin/free3Dmodel.dae");
	//m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/walking.dae");
	//m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/idle.dae");
	//m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/jump.dae");
	//m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/standard_run.dae");
	//m_skinned_mesh->switch_root_movement(false);

	engine::game_object_properties mannequin_props;
	mannequin_props.animated_mesh = m_skinned_mesh;
	mannequin_props.scale = glm::vec3(1.f/ glm::max(m_skinned_mesh->size().x, glm::max(m_skinned_mesh->size().y, m_skinned_mesh->size().z)));
	mannequin_props.position = glm::vec3(3.0f, 0.5f, -5.0f);
	mannequin_props.type = 0;
	mannequin_props.bounding_shape = m_skinned_mesh->size() / 2.f * mannequin_props.scale.x;
	m_mannequin = engine::game_object::create(mannequin_props);

	// Load the terrain texture and create a terrain mesh. Create a terrain object. Set its properties
	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/terrain.bmp", false) };
	engine::ref<engine::terrain> terrain_shape = engine::terrain::create(100.f, 0.5f, 100.f);
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { terrain_shape->mesh() };
	terrain_props.textures = terrain_textures;
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.bounding_shape = glm::vec3(100.f, 0.5f, 100.f);
	terrain_props.restitution = 0.92f;
	m_terrain = engine::game_object::create(terrain_props);

    m_grid = engine::grid::create(100, 100, 5);

	// Load the cow model. Create a cow object. Set its properties
	engine::ref <engine::model> cow_model = engine::model::create("assets/models/static/cow4.3ds");
	engine::game_object_properties cow_props;
	cow_props.meshes = cow_model->meshes();
	cow_props.textures = cow_model->textures();
	float cow_scale = 1.f / glm::max(cow_model->size().x, glm::max(cow_model->size().y, cow_model->size().z));
	cow_props.position = { -4.f,0.5f, -5.f };
	cow_props.scale = glm::vec3(cow_scale);
	cow_props.bounding_shape = cow_model->size() / 2.f * cow_scale;
	m_cow = engine::game_object::create(cow_props);

	// Load the tree model. Create a tree object. Set its properties
	engine::ref <engine::model> tree_model = engine::model::create("assets/models/static/elm.3ds");
	engine::game_object_properties tree_props;
	tree_props.meshes = tree_model->meshes();
	tree_props.textures = tree_model->textures();
	float tree_scale = 3.f / glm::max(tree_model->size().x, glm::max(tree_model->size().y, tree_model->size().z));
	tree_props.position = { 4.f, 0.5f, -5.f };
	tree_props.bounding_shape = tree_model->size() / 2.f * tree_scale;
	tree_props.scale = glm::vec3(tree_scale);
	m_tree = engine::game_object::create(tree_props);

	engine::ref<engine::sphere> sphere_shape = engine::sphere::create(20, 40, 0.5f);
	engine::game_object_properties sphere_props;
	sphere_props.position = { -2.5f, 0.0f, -2.5f };
	sphere_props.meshes = { sphere_shape->mesh() };
	sphere_props.type = 1;
	sphere_props.bounding_shape = glm::vec3(0.5f);
	sphere_props.restitution = 0.92f;
	sphere_props.mass = 0.000001f;
	m_ball = engine::game_object::create(sphere_props);

	//Projectile
	m_ballistic.initialise(engine::game_object::create(sphere_props));

	m_game_objects.push_back(m_terrain);
	//m_game_objects.push_back(m_ball);
	//m_game_objects.push_back(m_cow);
	//m_game_objects.push_back(m_tree);
	//m_game_objects.push_back(m_pickup);
	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	m_text_manager = engine::text_manager::create();

	//m_skinned_mesh->switch_animation(1);

}

example_layer::~example_layer() {}

void example_layer::on_update(const engine::timestep& time_step) 
{
    m_3d_camera.on_update(time_step);

	m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));

	m_ballistic.on_update(time_step);

	//m_mannequin->animated_mesh()->on_update(time_step);

	//check_bounce();
} 

void example_layer::on_render() 
{
    engine::render_command::clear_color({0.0f, 0.0f, 0.0f, 1.0f}); 
    engine::render_command::clear();

    //const auto textured_shader = engine::renderer::shaders_library()->get("mesh_static");
    //engine::renderer::begin_scene(m_3d_camera, textured_shader);

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
	//engine::renderer::submit(textured_lighting_shader, m_skybox, skybox_tranform);

	//engine::renderer::submit(textured_lighting_shader, m_terrain);

	m_grid->on_render(textured_lighting_shader);

	//glm::mat4 tree_transform(1.0f);
	//tree_transform = glm::translate(tree_transform, glm::vec3(4.f, 0.5, -5.0f));
	//tree_transform = glm::rotate(tree_transform, m_tree->rotation_amount(), m_tree->rotation_axis());
	//tree_transform = glm::scale(tree_transform, m_tree->scale());
	//engine::renderer::submit(textured_lighting_shader, tree_transform, m_tree);
	
	//glm::mat4 cow_transform(1.0f);
	//cow_transform = glm::translate(cow_transform, m_cow->position());
	//cow_transform = glm::rotate(cow_transform, m_cow->rotation_amount(), m_cow->rotation_axis());
	//cow_transform = glm::scale(cow_transform, m_cow->scale());
	//engine::renderer::submit(textured_lighting_shader, cow_transform, m_cow);

    engine::renderer::end_scene();

	// Set up material shader. (does not render textures, renders materials instead)
	const auto material_shader = engine::renderer::shaders_library()->get("mesh_material");
	engine::renderer::begin_scene(m_3d_camera, material_shader);

	m_material->submit(material_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(material_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	/*std::dynamic_pointer_cast<engine::gl_shader>(material_shader)->
		set_uniform("gNumPointLights", (int)num_point_lights);
	m_pointLight.submit(material_shader, 0);*/

	std::dynamic_pointer_cast<engine::gl_shader>(material_shader)->
		set_uniform("gNumSpotLights", (int)num_spot_lights);
	m_spotLight.submit(material_shader, 0);

	int side_length = 10;
	for (size_t i = 0;  i < 100; i++)
	{
		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, {(1 * i)% side_length,1.f,i/ side_length });
		transform = glm::scale(transform, m_ball->scale());
		engine::renderer::submit(material_shader, m_ball->meshes().at(0),transform);
	}

	std::dynamic_pointer_cast<engine::gl_shader>(material_shader)->set_uniform("lighting_on", false);

	m_lightsource_material->submit(material_shader);
	//engine::renderer::submit(material_shader, m_ball->meshes().at(0), glm::translate(glm::mat4(1.f), m_pointLight.Position));
	engine::renderer::submit(material_shader, m_ball->meshes().at(0), glm::translate(glm::mat4(1.f), m_spotLight.Position));


	std::dynamic_pointer_cast<engine::gl_shader>(material_shader)->set_uniform("lighting_on", true);

	//ballistic
	m_ballistic_material->submit(material_shader);
	m_ballistic.on_render(material_shader);


	engine::renderer::end_scene();

	const auto animated_mesh_shader = engine::renderer::shaders_library()->get("animated_mesh");
	engine::renderer::begin_scene(m_3d_camera, animated_mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	glm::mat4 aniTransform = glm::mat4(1.0f);
	//engine::renderer::submit(animated_mesh_shader, m_mannequin);

	engine::renderer::end_scene();

	// Render text
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	m_text_manager->render_text(text_shader, "Orange Text", 10.f, (float)engine::application::window().height()-25.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
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

		if (e.key_code() == engine::key_codes::KEY_3)
		{
			m_spotLight.Position += glm::vec3{0, 0.1f, 0};
		}

		if (e.key_code() == engine::key_codes::KEY_4)
		{
			m_spotLight.Position -= glm::vec3{ 0, 0.1f, 0 };
		}

		if (e.key_code() == engine::key_codes::KEY_5)
		{
			m_ballistic.fire(m_3d_camera, 2.f);
		}
    } 
}

void example_layer::check_bounce()
{
	//if (m_prev_sphere_y_vel < 0.f && m_game_objects.at(1)->velocity().y > 0.f)
		//m_audio_manager->play("bounce");
	m_prev_sphere_y_vel = m_game_objects.at(1)->velocity().y;
}
