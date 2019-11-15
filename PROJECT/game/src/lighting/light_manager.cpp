#include "pch.h"
#include "light_manager.h"
#include <glm/gtx/rotate_vector.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

engine::DirectionalLight* light_manager::m_sun_ptr;
std::vector<engine::ref<engine::PointLight>> light_manager::point_lights;
std::vector<engine::ref<engine::SpotLight>> light_manager::spot_lights;
float light_manager::m_sun_angle = 0.f;
float light_manager::day_duration = 60.f;
bool light_manager::m_sun_underground = false;

void light_manager::init(engine::DirectionalLight* sun)
{
	m_sun_ptr = sun;
}

void light_manager::on_update(const engine::timestep& ts)
{
	/*m_sun_angle = fmod(m_sun_angle + (float)(M_PI / day_duration) * ts,2* (float)M_PI);
	m_sun_underground = m_sun_angle > M_PI;	
	m_sun_ptr->Direction = glm::rotateZ(glm::vec3(1,0,0), -m_sun_angle);*/
	//Decided to forgoe day-night cycle in favour of permanent "mood lighting"
	m_sun_underground = true;

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
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->
		set_uniform("gNumSpotLights", (int)spot_lights.size());
	int tmp_point = 0;
	int tmp_spot = 0;
	
	//Turn the point lights on if the sun is underground, and off if it's not
	for (auto& point_light : point_lights) {
		point_light->On = m_sun_underground;
		point_light->submit(shader, tmp_point++);
	}	

	//Spot lights have their own behaviour, and can be on during the day
	for (auto& spot_light : spot_lights) {
		spot_light->submit(shader, tmp_spot++);
	}
}
