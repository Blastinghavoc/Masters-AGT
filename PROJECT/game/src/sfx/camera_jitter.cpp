#include "pch.h"
#include "camera_jitter.h"


void camera_jitter::on_update(engine::timestep& ts)
{
	if (m_active)
	{
		//See https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
		std::uniform_real_distribution<> dis(-m_intensity, m_intensity);
		glm::vec3 jitter(dis(m_generator), dis(m_generator), dis(m_generator));
		m_camera->face(m_camera->front_vector() + jitter);

	}
}

void camera_jitter::activate(float intensity, float duration)
{
	m_active = true;
	m_intensity = intensity;
	m_remaining = duration;
}
