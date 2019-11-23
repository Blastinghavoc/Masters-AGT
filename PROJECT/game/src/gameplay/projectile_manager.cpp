#include "pch.h"
#include "projectile_manager.h"
#include "../physics/physics_manager.h"
#include "../gameplay/gameplay_manager.h"

std::vector<engine::ref<projectile>> projectile_manager::m_active_projectiles;
std::vector<engine::ref<projectile>> projectile_manager::m_enemy_projectile_pool;
std::vector<engine::ref<projectile>> projectile_manager::m_ally_projectile_pool;

void projectile_manager::init()
{
	m_enemy_projectile_pool.push_back(get_projectile(true));
	m_ally_projectile_pool.push_back(get_projectile(false));
}

/*
Assumes direction already normalized.
*/
void projectile_manager::launch_projectile(bool is_enemy, glm::vec3 start_position, glm::vec3 direction, float speed)
{
	auto proj = get_projectile(is_enemy);
	proj->reset();//Reset properties ready for launch
	proj->object()->set_position(start_position);	
	proj->object()->set_velocity(direction*speed);
	m_active_projectiles.push_back(proj);
}

void projectile_manager::on_update(const engine::timestep& ts)
{

	/*
	TODO: if projectile->enemy collisions are required, this needs to be checked
	here by going through all active enemies and checking their trigger boxes.
	Currently this feature is not required.
	*/

	for (auto it = begin(m_active_projectiles); it != end(m_active_projectiles);) {
		auto& proj = *it;
		proj->on_update(ts);

		if (proj->object()->is_colliding())
		{//Check what the projectile is colliding with
			auto& others = proj->object()->collision_objects();

			for (auto& other : others) {
				if (other == gameplay_manager::get_player()->object())
				{
					gameplay_manager::damage_player();
					break;
				}
			}
			//Irrespective of what the projectile is colliding with, it should be destroyed.
			//This stops projectiles going through walls, and other projectiles!
			proj->deactivate();
		}

		if (proj->is_active())
		{
			++it;
		}
		else {
			//No longer active, so move back relevant into pool				

			if (proj->is_enemy())
			{
				m_enemy_projectile_pool.push_back(proj);
			}
			else {
				m_ally_projectile_pool.push_back(proj);
			}

			it = m_active_projectiles.erase(it);
		}
	}
}

void projectile_manager::on_render(const engine::ref<engine::shader>& shader)
{
	for (auto& proj : m_active_projectiles) {
		proj->render(shader);
		//proj->object()->render_obb(glm::vec3(.1f, .1f, .9f), shader);
		//proj->get_trigger_box().on_render(shader);
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

	//A new projectile must be created
	auto proj = projectile::create(is_enemy);
	//Add it to the physics system
	physics_manager::add(proj->object());
	return proj;
}
