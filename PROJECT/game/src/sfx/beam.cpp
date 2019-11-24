#include "pch.h"
#include "beam.h"
#include <gl\GL.h>

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


/*
NOTE:
It turns out that line_width is (via glLineWidth) is limited to quite a low value (10 on my machine).
This means that this method is not appropriate for thick beams, which would probably require some
3D mesh (a capsule would be a good choice) instead. For now though, this is sufficient.

This also means that there is no point in nesting beams more than two times, as the effect becomes
unnoticeable.
*/
void beam::on_render(engine::ref<engine::shader> shader)
{
	if (m_active)
	{
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", false);

		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("u_transform", glm::mat4(1.0f));

		m_material.set_ambient(m_colour);

		engine::renderer_api::line_width(0.2f*m_width);
		m_material.set_transparency(0.8f);
		m_material.submit(shader);
		m_line->va()->bind();
		engine::renderer_api::draw_indexed_lines(m_line->va());

		m_material.set_ambient(m_colour*0.8f);
		engine::renderer_api::line_width(m_width);
		m_material.set_transparency(0.4f);
		m_material.submit(shader);
		m_line->va()->bind();
		engine::renderer_api::draw_indexed_lines(m_line->va());		


		/*float additional_width = m_width / m_nesting;
		float current_width = additional_width;
		for (int i = 1; i <= m_nesting; ++i) {
			engine::renderer_api::line_width(current_width);
			current_width += additional_width;
			float f = i / (float)m_nesting;
			m_material.set_transparency(.8f * (1 - f) +.1f);
			m_material.submit(shader);

			m_line->va()->bind();
			engine::renderer_api::draw_indexed_lines(m_line->va());
		}*/

		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", true);
	}
}
