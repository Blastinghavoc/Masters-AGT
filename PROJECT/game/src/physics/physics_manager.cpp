#include "pch.h"
#include "physics_manager.h"

std::vector<engine::ref<engine::game_object>> physics_manager::m_physical_gameobjects{};
engine::ref<engine::bullet_manager> physics_manager::m_bullet_manager{};

void physics_manager::init()
{
	m_bullet_manager = engine::bullet_manager::create(m_physical_gameobjects);
}

void physics_manager::update(const engine::timestep& ts)
{
	m_bullet_manager->dynamics_world_update(m_physical_gameobjects, ts);
}
