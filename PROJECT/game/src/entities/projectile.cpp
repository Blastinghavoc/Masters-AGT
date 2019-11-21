#include "pch.h"
#include "projectile.h"

bool projectile::m_ally_prefab_ready;
bool projectile::m_enemy_prefab_ready;
engine::game_object_properties projectile::m_ally_prefab;
engine::game_object_properties projectile::m_enemy_prefab;

projectile::projectile(bool enemy) :m_is_enemy{ enemy }
{
	glm::vec3 projectile_size(.1f, .1f, .2f);
	engine::game_object_properties props;
	if (enemy)
	{
		if (m_enemy_prefab_ready)
		{
			props = m_enemy_prefab;

		}
		else {
			engine::ref<engine::texture_2d> tex = engine::texture_2d::create("assets/textures/laser_red.png", false);
			engine::ref<engine::cuboid> shape = engine::cuboid::create(projectile_size,false);
			props.position = { -10.f, 2.f, -10.f };
			props.meshes = { shape->mesh() };
			props.textures = { tex };
			props.bounding_shape = projectile_size;
		}
	}
	else {
		if (m_ally_prefab_ready)
		{
			props = m_ally_prefab;
		}
		else {
			engine::ref<engine::texture_2d> tex = engine::texture_2d::create("assets/textures/laser_green.png", false);
			engine::ref<engine::cuboid> shape = engine::cuboid::create(projectile_size, false);
			props.position = { -10.f, 2.f, -10.f };
			props.meshes = { shape->mesh() };
			props.textures = { tex };
			props.bounding_shape = projectile_size;
		}
	}
	m_object = engine::game_object::create(props);
	m_box.set_box(props);	
}

void projectile::on_update(const engine::timestep& time_step)
{
	if (!m_active)
	{
		return;
	}

	m_box.on_update(m_object->position());


}

void projectile::reset()
{
}
