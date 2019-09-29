#include "cube_layer.h"

#include "platform/opengl/gl_shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"

cube_layer::cube_layer()
	:m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f),
	m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height())	
{
	// hide the mouse and lock it inside the window
	engine::application::window().hide_mouse_cursor();

	auto colour_shader = engine::renderer::shaders_library()->get("colour");
	std::dynamic_pointer_cast<engine::gl_shader>(colour_shader)->bind();

	m_skybox = engine::skybox::create(50.f,
		std::vector<engine::ref<engine::texture_2d>>{ engine::texture_2d::create("assets/textures/black.png") });

	float size = 1.f;
	glm::vec3 half_extents(size / 2, size / 2, size / 2);

	engine::ref<engine::cuboid> m_cube_shape = engine::cuboid::create(half_extents,false);
	engine::game_object_properties cube_props;
	cube_props.meshes = { m_cube_shape->mesh() };
	cube_props.position = { 0.0f, 1.5f, -10.0f };

	m_cube = engine::game_object::create(cube_props);
}

cube_layer::~cube_layer()
{
}

void cube_layer::on_update(const engine::timestep& time_step)
{
	m_3d_camera.on_update(time_step);
}

void cube_layer::on_render()
{
	engine::render_command::clear_color({ 0.0f, 0.0f, 0.0f, 1.0f });
	engine::render_command::clear();

	const auto colour_shader = engine::renderer::shaders_library()->get("colour");
	engine::renderer::begin_scene(m_3d_camera, colour_shader);

	glm::vec4 cube_colour(1, 0, 0, 1);

	render_cube_sine(cube_colour,colour_shader,40,40);

	//Render a light? Doesn't work.
	/*glm::vec3 light_position = glm::vec3(0, 0, 20);
	render_one_cube(light_position, glm::vec4(0, 1, 0, 1), colour_shader);
	auto light = engine::light::create(light_position, glm::vec3(0, 1, 0), 100, 100, 100);
	light->submit(colour_shader);*/

	engine::renderer::end_scene();
}

void cube_layer::render_cube_sine(const glm::vec4& colour,const engine::ref<engine::shader>& colour_shader,int x, int y) {
	for (int i = -x/2; i < x/2; i++)
	{
		for (int j = -y/2; j < y/2; j++) {
			render_one_cube(glm::vec3(i,j,sin(sqrt(i*i+j*j))),colour, colour_shader);
		}
	}
}

void cube_layer::render_one_cube(glm::vec3& pos , const glm::vec4& colour, const engine::ref<engine::shader>& colour_shader) {
	std::dynamic_pointer_cast<engine::gl_shader>(colour_shader)->set_uniform("a_color", colour);
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos);
	engine::renderer::submit(colour_shader, m_cube->meshes().at(0), transform);
}

void cube_layer::on_event(engine::event& event)
{
	if (event.event_type() == engine::event_type_e::key_pressed)
	{
		auto& e = dynamic_cast<engine::key_pressed_event&>(event);		
		switch (e.key_code())
		{
			case engine::key_codes::KEY_TAB:
				engine::render_command::toggle_wireframe();
				break;
			
			default:
				break;
		}
	}
}
