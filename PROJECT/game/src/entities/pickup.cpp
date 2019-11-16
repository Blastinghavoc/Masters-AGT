#include "pickup.h"

pickup::pickup()
{
}

pickup::pickup(engine::ref<engine::game_object>& obj,float rotation_speed) :m_object{obj},
m_rotation_speed{rotation_speed}
{

}

pickup::~pickup()
{
}

void pickup::on_update(const engine::timestep& time_step)
{
	//Rotate the object
	m_object->set_rotation_amount(fmod(m_object->rotation_amount() + m_rotation_speed * time_step, 2* engine::PI));
}
