#include "player.h"

player::player()
{	
}

player::~player()
{
}

void player::initialise(engine::ref<engine::game_object> object)
{
	m_object = object;
	m_object->set_forward(glm::vec3(0.f, 0.f, -1.f));
	m_object->set_position(glm::vec3(0.f, 0.5, 10.f));
	m_object->animated_mesh()->set_default_animation(1);
	m_object->set_rotation_axis({ 0,1,0 });
}
void player::on_update(const engine::timestep& time_step)
{
	if (engine::input::key_pressed(engine::key_codes::KEY_1))
	{
		turn(1.f * time_step);//turn left
	}
	if (engine::input::key_pressed(engine::key_codes::KEY_2))
	{
		turn(-1.f * time_step);//turn right
	}
	if (engine::input::key_pressed(engine::key_codes::KEY_SPACE))
	{
		jump();//jump
	}


	auto forward = m_object->forward();

	m_object->set_position(m_object->position() += forward * m_speed*
		(float)time_step);

	//orient in direction of travel
	m_object->set_rotation_amount(atan2(forward.x, forward.z));

	m_object->animated_mesh()->on_update(time_step);

	//Update the timer
	if (m_timer > 0.f)
	{
		m_timer -= (float)time_step;
		if (m_timer < 0.f)
		{
			m_object->animated_mesh()->switch_root_movement(false);
			m_object->animated_mesh()->switch_animation(m_object->animated_mesh()->default_animation());
			m_speed = 1.f;
		}
	}

}

void player::turn(float angle)
{
	m_object->set_forward(glm::rotate(m_object->forward(), angle, glm::vec3(0.f, 1.f,
		0.f)));
}

void player::update_camera(engine::perspective_camera& camera)
{
	glm::vec3 upward_unit{ 0,1,0 };
	auto forward_unit = glm::normalize(m_object->forward());

	glm::vec3 new_cam_pos = m_object->position() + (-1.f * forward_unit) + (1.f*upward_unit);

	glm::vec3 look_at = m_object->position() + 6.f * forward_unit;

	camera.set_view_matrix(new_cam_pos, look_at);
}

void player::jump()
{
	m_object->animated_mesh()->switch_root_movement(true);
	m_object->animated_mesh()->switch_animation(3);
	m_speed = 0.f;

	m_timer = m_object->animated_mesh()->animations().at(3)->mDuration;

}
