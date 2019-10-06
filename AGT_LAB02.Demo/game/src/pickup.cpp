#include "pickup.h"

#define _USE_MATH_DEFINES
#include <math.h>

pickup::pickup(const engine::game_object_properties props) : engine::game_object(props)
{
}

pickup::~pickup()
{
}

void pickup::init()
{
	m_is_active = true;
}

void pickup::update(glm::vec3 c, float dt)
{
	set_rotation_amount(fmod(rotation_amount()+ (dt*1.5f),2*M_PI));

	auto c_to_p = position() - c;
	float dist = glm::length(c_to_p);
	//LOG_INFO("dist = {}", dist);
	if (dist < 2.f)
	{
		m_is_active = false;
	}
	/*else {
		m_is_active = true;
	}*/
}

engine::ref<pickup> pickup::create(const engine::game_object_properties& props)
{
	return std::make_shared<pickup>(props);
}
