#include "pch.h"
#include "projectile_manager.h"

std::vector<engine::ref<projectile>> projectile_manager::m_active_projectiles;
std::vector<engine::ref<projectile>> projectile_manager::m_enemy_projectile_pool;
std::vector<engine::ref<projectile>> projectile_manager::m_ally_projectile_pool;

void projectile_manager::init()
{
	m_enemy_projectile_pool.push_back(get_projectile(true));
	m_ally_projectile_pool.push_back(get_projectile(false));
}

void projectile_manager::launch_projectile(bool is_enemy, glm::vec3 start_position, glm::vec3 direction)
{
	auto proj = get_projectile(is_enemy);
	proj->object()->set_position(start_position);
	//TODO improve this; ensure correct orientation and allow variable speed.
	proj->object()->set_velocity(direction);
	m_active_projectiles.push_back(proj);
}

void projectile_manager::on_update(const engine::timestep& time_step)
{
	auto& active_enemies = enemy_manager::get_active_enemies();
	for (auto it = begin(m_active_projectiles); it != end(m_active_projectiles);) {
		//TODO
	}
}

void projectile_manager::on_render(const engine::ref<engine::shader>& shader)
{
	for (auto& proj : m_active_projectiles) {
		engine::renderer::submit(shader, proj->object());
		proj->get_trigger_box().on_render(shader);
	}
}

engine::ref<projectile> projectile_manager::get_projectile(bool is_enemy)
{
	if (is_enemy)
	{
		if (!m_enemy_projectile_pool.empty())
		{
			auto proj = m_enemy_projectile_pool.back();
			m_enemy_projectile_pool.pop_back();
			return proj;
		}
	}
	else {
		if (!m_ally_projectile_pool.empty())
		{
			auto proj = m_ally_projectile_pool.back();
			m_ally_projectile_pool.pop_back();
			return proj;
		}
	}

	return projectile::create(is_enemy);
}
