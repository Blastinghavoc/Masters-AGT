#include "pch.h"
#include "light_manager.h"
#include <glm/gtx/rotate_vector.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

engine::DirectionalLight* light_manager::m_sun_ptr;
std::vector<engine::PointLight> light_manager::point_lights;
float light_manager::m_sun_angle = 0.f;
float light_manager::day_duration = 60.f;
bool light_manager::m_sun_underground = false;

void light_manager::init(engine::DirectionalLight* sun)
{
	m_sun_ptr = sun;
	//m_sun_ptr->Direction = { 1,0,0 };
}

//m_directionalLight.AmbientIntensity = 0.25f;
//m_directionalLight.DiffuseIntensity = 0.6f;

void light_manager::on_update(const engine::timestep& ts)
{
	m_sun_angle = fmod(m_sun_angle + (float)(M_PI / day_duration) * ts,2* M_PI);
	m_sun_underground = m_sun_angle > M_PI;
	float adjusted_angle = /*(m_sun_angle > M_PI) ? (float)M_PI :*/ m_sun_angle;
	m_sun_ptr->Direction = glm::rotateZ(glm::vec3(1,0,0), -adjusted_angle);

	if (m_sun_underground)
	{
		m_sun_ptr->DiffuseIntensity = 0.f;

	}
	else {
		m_sun_ptr->DiffuseIntensity = 0.6f;
	}
}

void light_manager::submit(engine::ref<engine::shader> shader)
{
	m_sun_ptr->submit(shader);

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->
		set_uniform("gNumPointLights", (int)point_lights.size());
	int tmp = 0;
	if (m_sun_underground)
	{
		//Turn the lights on if the sun is underground
		for (auto& point_light : point_lights) {
			point_light.submit(shader, tmp++);
		}
	}
	else {
		engine::PointLight off;
		for (size_t i = 0; i < point_lights.size(); ++i)
		{
			off.submit(shader, tmp++);
		}
	}
}
