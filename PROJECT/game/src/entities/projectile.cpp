#include "pch.h"
#include "projectile.h"

bool projectile::m_ally_prefab_ready;
bool projectile::m_enemy_prefab_ready;
engine::game_object_properties projectile::m_ally_prefab;
engine::game_object_properties projectile::m_enemy_prefab;

projectile::projectile(bool enemy) :m_is_enemy{ enemy }
{
	glm::vec3 projectile_size(.05f, .05f, .2f);
	glm::vec3 collision_shape(.05f, .05f, .05f);//Uniform collision shape to make rotation irrelevant.
	engine::game_object_properties props;
	engine::ref<engine::texture_2d> tex;

	bool had_prefab = true;
	if (enemy)
	{
		if (m_enemy_prefab_ready)
		{
			props = m_enemy_prefab;
		}
		else {
			tex = engine::texture_2d::create("assets/textures/laser_red.png", false);
			had_prefab = false;
		}
	}
	else {
		if (m_ally_prefab_ready)
		{
			props = m_ally_prefab;
		}
		else {
			tex = engine::texture_2d::create("assets/textures/laser_green.png", false);
			had_prefab = false;
		}
	}


	if (!had_prefab)
	{//Set up common properties if no prefab was available
		engine::ref<engine::cuboid> shape = engine::cuboid::create(projectile_size, false);
		props.position = { -10.f, 2.f, -10.f };
		props.meshes = { shape->mesh() };
		props.textures = { tex };
		props.offset = glm::vec3(0, -projectile_size.y, 0);
		props.bounding_shape = collision_shape;
		props.gravity_scale = 0;//These projectiles are not affected by gravity to simplify aiming.		
	}
	
	m_object = engine::game_object::create(props);
	m_object->set_angular_factor({ 0, 0, 0 });//Do not allow projectiles to be rotated
	m_box.set_box(props);
	m_lifetimer.start();
}

void projectile::on_update(const engine::timestep& time_step)
{
	if (!m_active)
	{
		return;
	}

	m_box.on_update(m_object->position());

	if (m_lifetimer.total() > m_max_lifetime)
	{//Deactivate projectile if it's been going for too long
		deactivate();
	}
	else {
		//orient in direction of travel. Same technique as for turrets, see turret.cpp
		auto dir = glm::normalize(m_object->velocity());

		auto position = m_object->position() - m_object->offset() * m_object->scale();

		glm::mat4 transform(1.f);
		transform = glm::inverse(glm::lookAt(position, position - dir, { 0,1,0 }));
		transform = glm::scale(transform, m_object->scale());
		m_transform = transform;
	}
}

void projectile::render(const engine::ref<engine::shader>& shader)
{
	engine::renderer::submit(shader, m_transform, m_object);
}

void projectile::reset()
{
	m_active = true;
	m_lifetimer.reset();
	m_object->set_active(true);
}

//Causes object to stop being updated, and hide from the physics simulation
void projectile::deactivate()
{
	m_active = false;
	m_object->set_active(false);
}
