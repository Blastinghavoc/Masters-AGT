#include "pch.h"
#include "turret_manager.h"

std::map<turret_manager::vec3, engine::ref<turret>> turret_manager::m_turrets{};

void turret_manager::init()
{
}

void turret_manager::render(engine::ref<engine::shader> shader)
{
	for (auto& pair : m_turrets) {
		pair.second->face({ 0,0,0 });
		pair.second->render(shader);
	}
}

bool turret_manager::place_turret(glm::vec3 position)
{
	auto map_pos = from_glm(position);
	if (m_turrets.count(map_pos) > 0)
	{
		return false;//can't place turrets on top of each other
	}
	m_turrets[map_pos] = turret::create(position);
	return true;
}

bool turret_manager::remove_turret(glm::vec3 position)
{
	auto map_pos = from_glm(position);
	if (m_turrets.count(map_pos) > 0)
	{
		m_turrets.erase(map_pos);
		return true;
	}
	return false;//Cannot remove turret that does not exist.
}
