#include "pch.h"
#include "renderer.h"
#include "platform/opengl/gl_shader.h"

engine::renderer::scene_data* engine::renderer::s_scene_data = new scene_data;
engine::renderer* engine::renderer::s_instance = new renderer();

void engine::renderer::init()
{
    s_instance->m_shader_library = std::make_unique<shader_library>();
    renderer_api::init();

    renderer::shaders_library()->load("assets/shaders/mesh_static.glsl");
    //renderer::shaders_library()->load("assets/shaders/mesh_animated.glsl");
	renderer::shaders_library()->load("assets/shaders/mesh_lighting.glsl");
	renderer::shaders_library()->load("assets/shaders/colour.glsl");
}

void engine::renderer::begin_scene(camera& camera, const ref<shader>& shader)
{
    s_scene_data->view_projection_matrix = camera.view_projection_matrix();
    s_scene_data->shader = shader;
    shader->bind();
    std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("u_view_projection", s_scene_data->view_projection_matrix);
}

void engine::renderer::end_scene()
{
    s_scene_data->shader->unbind(); 
}

void engine::renderer::submit(
    const ref<shader>& shader, 
    const ref<vertex_array>& vertex_array, 
    const glm::mat4& transform /*= glm::mat4(1.f)*/)
{
    std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("u_transform", transform);

    vertex_array->bind();
    render_command::submit(vertex_array);
}

void engine::renderer::submit(
    const ref<shader>& shader,
    const ref<mesh>& mesh,
    const glm::mat4& transform /*= glm::mat4(1.f)*/)
{
    submit(shader, mesh->va(), transform);
}

void engine::renderer::submit(
    const ref<shader>& shader, 
	const ref<game_object>& object)
{
	glm::mat4 transform = glm::mat4(1.0f);
	object->transform(transform);
	object->bind_textures();
    const bool has_meshes = !object->meshes().empty();
	if (has_meshes)
	{
		auto model_meshes = object->meshes();
		for (const auto& mesh : model_meshes)
		{
			submit(shader, mesh, transform);
		}
	}
}

void engine::renderer::submit(
	const ref<shader>& shader,
	const ref<skybox>& skybox,
	const glm::mat4& transform)
{
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("skybox_rendering", true);
	if (skybox->textures().size() == skybox->meshes().size())
	{
		uint32_t i = 0;
		auto model_meshes = skybox->meshes();
		for (const auto& mesh : model_meshes)
		{
			skybox->textures().at(i)->bind();
			submit(shader, mesh, transform);
			i++;
		}
	}
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("skybox_rendering", false);
}
