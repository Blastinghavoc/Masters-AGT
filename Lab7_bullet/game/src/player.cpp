#include "pch.h"
#include "player.h"
#include "engine/core/input.h"
#include "engine/key_codes.h"

player::player() : m_timer(0.0f), m_speed(1.f)
{
}

player::~player()
{}

void player::initialise(engine::ref<engine::game_object> object)
{
	m_object = object;
	m_object->set_forward(glm::vec3(0.f, 0.f, -1.f));
	m_object->set_position(glm::vec3(0.f, 0.5, 4.f));
	m_object->animated_mesh()->set_default_animation(1);
}

void player::on_update(const engine::timestep& time_step)
{
	/*m_object->set_position(m_object->position() += m_object->forward() * m_speed * (float)time_step);
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));*/

	auto speed_in_forward_direction = glm::dot(m_object->velocity(), m_object->forward()) / glm::length(m_object->forward());
	auto adjustment = m_object->forward() * (m_speed - speed_in_forward_direction);
	//Set the speed in the desired direction without affecting speed in other directions.

	m_object->set_velocity(m_object->velocity()+ adjustment);

	if (engine::input::key_pressed(engine::key_codes::KEY_1)) // left
		turn(1.0f * time_step);
	else if (engine::input::key_pressed(engine::key_codes::KEY_2)) // right
		turn(-1.0f * time_step);
	else if (engine::input::key_pressed(engine::key_codes::KEY_SPACE)) // right
		jump();
	else {
		turn(0.f);
	}

	m_object->animated_mesh()->on_update(time_step);

	if (m_timer > 0.0f)
	{
		m_timer -= (float)time_step;
		if (m_timer < 0.0f)
		{
			m_object->animated_mesh()->switch_root_movement(false);
			m_object->animated_mesh()->switch_animation(m_object->animated_mesh()->default_animation());
			m_speed = 1.0f;
		}
	}

}

void player::turn(float angle)
{
	m_object->set_angular_velocity(glm::vec3(0.f, angle * 100.f, 0.f));
	//m_object->set_forward(glm::rotate(m_object->forward(), angle, glm::vec3(0.f, 1.f, 0.f)));
}

void player::update_camera(engine::perspective_camera& camera)
{
	glm::vec3 cam_pos = m_object->position() - glm::normalize(m_object->forward()) * 3.f;
	cam_pos.y += 2.f;

	glm::vec3 cam_look_at = m_object->position() + glm::normalize(m_object->forward()) * 6.f;
	cam_look_at.y = 0.f;
	camera.set_view_matrix(cam_pos, cam_look_at);
}

void player::jump()
{
	m_object->animated_mesh()->switch_root_movement(true);
	m_object->animated_mesh()->switch_animation(3);
	//m_speed = 0.0f;
	m_object->set_velocity(m_object->velocity() + m_object->up() * m_speed);

	m_timer = m_object->animated_mesh()->animations().at(3)->mDuration;
}

