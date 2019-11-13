#include "pch.h"
#include "camera_jitter.h"
#include "glm/gtx/rotate_vector.hpp"

void camera_jitter::on_update(const engine::timestep& ts)
{
	if (m_active)
	{
		if (m_remaining > 0)
		{
			//See https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
			std::uniform_real_distribution<> dis(-m_intensity, m_intensity);
			glm::vec3 jitter = ts.seconds() * glm::vec3(dis(m_generator), dis(m_generator), dis(m_generator));
			m_camera->face(m_camera->front_vector() + jitter - m_previous);
			m_previous = jitter;
			m_remaining -= ts;

			if (m_remaining <= 0)
			{
				m_previous = { 0,0,0 };
				m_active = false;
			}
		}

	}
}

void camera_jitter::activate(float intensity, float duration)
{
	m_active = true;
	m_intensity = intensity;
	m_remaining = duration;
}
