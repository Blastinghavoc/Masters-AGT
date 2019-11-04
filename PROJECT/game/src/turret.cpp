#include "turret.h"
#include <glm/gtx/rotate_vector.hpp>

turret::turret(glm::vec3 position)
{
	float base_height = .3f;
	float swivel_height = .7f;

	engine::ref <engine::model> model = engine::model::create("assets/models/static/turrets/modified/base.obj");
	engine::game_object_properties props;
	props.meshes = model->meshes();
	props.textures = model->textures();
	float scale = 1.f / glm::max(model->size().x, glm::max(model->size().y, model->size().z));
	props.position = position;
	props.scale = glm::vec3(scale);
	props.bounding_shape = model->size() / 2.f * scale;
	m_base = engine::game_object::create(props);

	model = engine::model::create("assets/models/static/turrets/modified/swivel.obj");
	props = {};
	props.meshes = model->meshes();
	props.textures = model->textures();
	props.position = position + glm::vec3(0,base_height,0);
	props.scale = glm::vec3(scale);
	props.bounding_shape = model->size() / 2.f * scale;
	m_swivel = engine::game_object::create(props);

	m_barrel_position = position + glm::vec3(0, swivel_height, 0);
	model = engine::model::create("assets/models/static/turrets/modified/barrel.obj");
	props = {};
	props.meshes = model->meshes();
	props.textures = model->textures();
	props.position = m_barrel_position;
	props.scale = glm::vec3(scale);
	props.bounding_shape = model->size() / 2.f * scale;
	m_barrel = engine::game_object::create(props);
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

	//TODO REF: https://stackoverflow.com/questions/26555040/yaw-pitch-and-roll-to-glmrotate
	float projectionLength = std::sqrt(dir.x * dir.x + dir.z * dir.z);
	float pitch = std::atan2(dir.y, projectionLength);

	//Rotate the base.
	m_swivel->set_rotation_axis({0,1,0});
	m_swivel->set_rotation_amount(yaw);

	//Custom transformation of the barrel to rotate it on two axes
	glm::mat4 barrel_transform(1.f);	
	barrel_transform = glm::inverse(glm::lookAt(m_barrel_position, m_barrel_position - dir, { 0,1,0 }));
	barrel_transform = glm::scale(barrel_transform, m_barrel->scale());
	m_barrel_transform = barrel_transform;

}