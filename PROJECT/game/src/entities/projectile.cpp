#include "pch.h"
#include "projectile.h"

bool projectile::m_ally_prefab_ready;
bool projectile::m_enemy_prefab_ready;
engine::game_object_properties projectile::m_ally_prefab;
engine::game_object_properties projectile::m_enemy_prefab;

projectile::projectile(bool enemy) :m_is_enemy{ enemy }
{
	engine::game_object_properties props;
	if (enemy)
	{
		if (m_enemy_prefab_ready)
		{
			props = m_enemy_prefab;

		}
		else {
			engine::ref<engine::texture_2d> tex = engine::texture_2d::create("assets/textures/laser_red.png", false);
			engine::ref<engine::cuboid> shape = engine::cuboid::create({0.5f,.5f,1.f},false);			
			props.position = { -10.f, 2.f, -10.f };
			props.meshes = { shape->mesh };
			props.textures = { tex };
			props.bounding_shape = { 0.5f,.5f,1.f };
		}
	}
	else {
		if (m_ally_prefab_ready)
		{
			props = m_ally_prefab;
		}
		else {
			engine::ref<engine::texture_2d> tex = engine::texture_2d::create("assets/textures/laser_green.png", false);
			engine::ref<engine::cuboid> shape = engine::cuboid::create({ 0.5f,.5f,1.f }, false);
			props.position = { -10.f, 2.f, -10.f };
			props.meshes = { shape->mesh };
			props.textures = { tex };
			props.bounding_shape = { 0.5f,.5f,1.f };
		}
	}
	m_object = engine::game_object::create(props);
	m_box.set_box(props);
}

void projectile::on_update(const engine::timestep& time_step)
{
}
