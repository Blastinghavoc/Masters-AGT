#include "example_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"

example_layer::example_layer() 
    :m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f), 
    m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height()),
	m_position{0.0f},
	m_speed{1.0f}
{
    // hide the mouse and lock it inside the window
    //engine::application::window().hide_mouse_cursor();

	auto colour_shader = engine::renderer::shaders_library()->get("colour");
	std::dynamic_pointer_cast<engine::gl_shader>(colour_shader)->bind();

	m_skybox = engine::skybox::create(50.f,
		std::vector<engine::ref<engine::texture_2d>>{ engine::texture_2d::create("assets/textures/black.png") });

	float radius = 2.f;
	uint32_t stacks = 20;
	uint32_t slices = 20;

	engine::ref<engine::sphere> m_sphere_shape = engine::sphere::create(stacks, slices, radius);
	engine::game_object_properties sphere_props;
	sphere_props.meshes = { m_sphere_shape->mesh() };
	sphere_props.position = { 0.0f, 1.5f, -10.0f };

	m_sphere = engine::game_object::create(sphere_props);
}

example_layer::~example_layer() {}

void example_layer::on_update(const engine::timestep& time_step)
{
	if (m_position > 10.0f || m_position < -10) {
		m_speed = -m_speed;
	}
	m_position += m_speed * time_step;
} 

void example_layer::on_render() 
{
    engine::render_command::clear_color({0.0f, 0.0f, 0.0f, 1.0f}); 
    engine::render_command::clear();

	const auto colour_shader = engine::renderer::shaders_library()->get("colour");
	engine::renderer::begin_scene(m_3d_camera, colour_shader);

	glm::vec4 sphere_colour_1 = glm::vec4(1.f, 0.f, 0.f, 1.f);
	glm::vec3 sphere_position_1 = glm::vec3(m_position, 1.5f, -20.0f);

	glm::vec4 sphere_colour_2 = glm::vec4(0.f, 0.f, 1.f, 1.f);
	glm::vec3 sphere_position_2 = glm::vec3(1.f, m_position, -20.0f);

	glm::vec4 sphere_colour_3 = glm::vec4(0.f, 1.f, 0.f, 1.f);
	glm::vec3 sphere_position_3 = glm::vec3(0.f, 0.f, m_position -20.f);

	glm::vec4 sphere_colour_4 = glm::vec4(0.f, 1.f, 1.f, 1.f);
	glm::vec3 sphere_position_4 = glm::vec3(m_position, m_position, m_position - 20.f);


	std::dynamic_pointer_cast<engine::gl_shader>(colour_shader)->set_uniform("a_color", sphere_colour_1);
	glm::mat4 transform_1 = glm::translate(glm::mat4(1.0f), sphere_position_1);
	engine::renderer::submit(colour_shader, m_sphere->meshes().at(0), transform_1);

	std::dynamic_pointer_cast<engine::gl_shader>(colour_shader)->set_uniform("a_color", sphere_colour_2);
	glm::mat4 transform_2 = glm::translate(glm::mat4(1.0f), sphere_position_2);
	engine::renderer::submit(colour_shader, m_sphere->meshes().at(0), transform_2);

	std::dynamic_pointer_cast<engine::gl_shader>(colour_shader)->set_uniform("a_color", sphere_colour_3);
	glm::mat4 transform_3 = glm::translate(glm::mat4(1.0f), sphere_position_3);
	engine::renderer::submit(colour_shader, m_sphere->meshes().at(0), transform_3);

	std::dynamic_pointer_cast<engine::gl_shader>(colour_shader)->set_uniform("a_color", sphere_colour_4);
	glm::mat4 transform_4 = glm::translate(glm::mat4(1.0f), sphere_position_4);
	engine::renderer::submit(colour_shader, m_sphere->meshes().at(0), transform_4);

    engine::renderer::end_scene();
}

void example_layer::on_event(engine::event& event) 
{ 
	if (event.event_type() == engine::event_type_e::key_pressed)
	{
		auto& e = dynamic_cast<engine::key_pressed_event&>(event);
		if (e.key_code() == engine::key_codes::KEY_TAB)
		{
			engine::render_command::toggle_wireframe();
		}
		if (e.key_code() == engine::key_codes::KEY_UP)
		{
			m_speed += (m_speed > 0)? 1 : -1;			
		}
		if (e.key_code() == engine::key_codes::KEY_DOWN)
		{
			m_speed -= (m_speed > 0) ? 1 : -1;
		}
		if (m_speed > 100.0f)
		{
			m_speed = 100.0f;
		}
		else if (m_speed < -100.0f)
		{
			m_speed = -100.0f;
		}
	}
}
