#pragma once
#include "../ai/enemy_manager.h"
#include "../entities/projectile.h"

/*
Class to manage the lifetime of projectiles fired by NPCs,
including turrets and enemies.
*/
class projectile_manager {
public:

	static void init();

	static void launch_projectile(bool is_enemy, glm::vec3 start_position, glm::vec3 direction,float speed = m_projectile_speed);

	static void on_update(const engine::timestep& time_step);

	static void on_render(const engine::ref<engine::shader>& shader);

	static size_t num_projectiles() { return m_active_projectiles.size(); };
	static size_t enemy_pool_size() { return m_enemy_projectile_pool.size(); };
	static size_t ally_pool_size() { return m_ally_projectile_pool.size(); };

private:
	static std::vector<engine::ref<projectile>> m_active_projectiles;
	static std::vector<engine::ref<projectile>> m_enemy_projectile_pool;
	static std::vector<engine::ref<projectile>> m_ally_projectile_pool;

	static engine::ref<projectile> get_projectile(bool is_enemy);

	static constexpr float m_projectile_speed = 2.f;

};
