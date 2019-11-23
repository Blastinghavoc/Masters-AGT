#include "pch.h"
#include "beam.h"

beam::beam():
	m_material{ 0.0f, glm::vec3(1.0f), glm::vec3(0.f), glm::vec3(0.f), 0.0f }
{
	m_timer.start();	
}

void beam::activate(glm::vec3 start, glm::vec3 end, glm::vec3 rgb, float duration,float width,int nesting)
{
	m_active = true;
	m_timer.reset();

	engine::mesh::vertex s = engine::mesh::vertex(start, glm::vec3(0,1,0), glm::vec2(0));
	engine::mesh::vertex e = engine::mesh::vertex(end, glm::vec3(0, 1, 0), glm::vec2(0));

	m_line = engine::mesh::create({ s,e }, { 0,1 });

	m_colour = rgb;
	m_duration = duration;
	m_width = width;
	m_nesting = nesting;
}

void beam::on_update(const engine::timestep& ts)
{
	if (m_active)
	{
		if (m_timer.total() > m_duration)
		{
			m_active = false;
		}
	}
}

void beam::on_render(engine::ref<engine::shader> shader)
{
	if (m_active)
	{
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", false);

		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("u_transform", glm::mat4(1.0f));

		m_material.set_ambient(m_colour);

		float additional_width = m_width / m_nesting;
		float current_width = additional_width;
		for (int i = 1; i <= m_nesting; ++i) {
			engine::renderer_api::line_width(current_width);
			current_width += additional_width;
			float f = i / (float)m_nesting;
			m_material.set_transparency(.8f * (1 - f) +.1f);
			m_material.submit(shader);

			m_line->va()->bind();
			engine::renderer_api::draw_indexed_lines(m_line->va());
		}

		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", true);
	}
}
