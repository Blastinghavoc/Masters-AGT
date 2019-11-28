#include "turret.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>

turret::turret(glm::vec3 position)
{
	engine::ref <engine::model> model = engine::model::create("assets/models/static/turrets/modified/base.obj");
	engine::game_object_properties props;
	props.meshes = model->meshes();
	props.textures = model->textures();
	float scale = m_scale_multiplier / glm::max(model->size().x, glm::max(model->size().y, model->size().z));
	props.position = position;
	props.scale = glm::vec3(scale);
	props.bounding_shape = model->size() / 2.f * scale;
	m_base = engine::game_object::create(props);

	model = engine::model::create("assets/models/static/turrets/modified/swivel.obj");
	props = {};
	props.meshes = model->meshes();
	props.textures = model->textures();
	props.position = position + glm::vec3(0, m_scale_multiplier * m_base_height,0);
	props.scale = glm::vec3(scale);
	props.bounding_shape = model->size() / 2.f * scale;
	m_swivel = engine::game_object::create(props);

	m_barrel_position = position + glm::vec3(0, m_scale_multiplier * m_swivel_height, 0);
	model = engine::model::create("assets/models/static/turrets/modified/barrel.obj");
	props = {};
	props.meshes = model->meshes();
	props.textures = model->textures();
	props.position = m_barrel_position;
	props.scale = glm::vec3(scale);
	props.bounding_shape = model->size() / 2.f * scale;
	m_barrel = engine::game_object::create(props);
	
	face(m_barrel_position+glm::vec3( 0,0,1 ));//Face north
}

turret::~turret()
{
}

void turret::render(const engine::ref<engine::shader>& shader)
{
	engine::renderer::submit(shader, m_base);
	engine::renderer::submit(shader, m_swivel);
	engine::renderer::submit(shader, m_barrel_transform, m_barrel);
}

void turret::face(glm::vec3 target)
{
	auto dir = glm::normalize(target - m_barrel_position);

	float yaw = atan2(dir.x, dir.z);	

	//Rotate the base.
	m_swivel->set_rotation_axis({0,1,0});
	m_swivel->set_rotation_amount(yaw);

	//Custom transformation of the barrel to rotate it on two axes
	//REF: https://gamedev.stackexchange.com/questions/150120/rotate-an-object-to-face-a-point-with-glmlookat
	glm::mat4 barrel_transform(1.f);	
	barrel_transform = glm::inverse(glm::lookAt(m_barrel_position, m_barrel_position - dir, { 0,1,0 }));
	barrel_transform = glm::scale(barrel_transform, m_barrel->scale());
	m_barrel_transform = barrel_transform;

}

void turret::update(const engine::timestep& ts)
{
	if (m_cooldown_remaining > 0)
	{
		m_cooldown_remaining -= ts.seconds();
		if (m_cooldown_remaining <= 0)
		{
			m_can_fire = true;
		}
	}
}

float turret::fire()
{
	if (m_can_fire)
	{
		m_can_fire = false;
		m_cooldown_remaining = m_cooldown_time;
		return m_damage;
	}
	return 0.0f;
}

void turret::set_position(glm::vec3 position)
{
	m_base->set_position(position);
	m_swivel->set_position(position + glm::vec3(0, m_scale_multiplier * m_base_height, 0));
	m_barrel_position = position + glm::vec3(0, m_scale_multiplier * m_swivel_height, 0);
	m_barrel->set_position(m_barrel_position);
	face(m_barrel_position + glm::vec3(0, 0, 1));//Face north
}

bool turret::is_in_range(glm::vec3 target)
{
	return glm::distance2(target, m_barrel_position) < m_range_squared;
}
