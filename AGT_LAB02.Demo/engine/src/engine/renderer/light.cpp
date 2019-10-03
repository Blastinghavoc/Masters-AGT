#include "pch.h"
#include "light.h"
#include "platform/opengl/gl_shader.h"

engine::light::light(const glm::vec3 position, const glm::vec3 colour,
	const float ambient, const float diffuse, const float specular)
	: m_position(position), m_colour(colour), m_ambient(ambient), m_diffuse(diffuse), m_specular(specular)
{
}

engine::light::~light()
{
}

engine::ref<engine::light> engine::light::create(const glm::vec3 position, const glm::vec3 colour,
	const float ambient, const float diffuse, const float specular)
{
	return std::make_shared<light>(engine::light(position, colour, ambient, diffuse, specular));
}

void engine::light::submit(const engine::ref<engine::shader> shader)
{
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("light.position", m_position);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("light.colour", m_colour);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("light.ambient", m_ambient);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("light.diffuse", m_diffuse);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("light.specular", m_specular);
}
