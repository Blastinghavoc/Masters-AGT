#include "pch.h"
#include "turret_manager.h"

std::map<turret_manager::vec3, engine::ref<turret>> turret_manager::m_turrets{};
std::vector<engine::ref<turret>> turret_manager::m_buffer;

void turret_manager::init()
{
	//Add a few turrets to the buffer
	m_buffer.push_back(turret::create());
	m_buffer.push_back(turret::create());
	m_buffer.push_back(turret::create());
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

	if (m_buffer.empty())
	{
		m_turrets[map_pos] = turret::create(position);
	}
	else {
		//If possible, use a turret from the buffer rather than creating a new one.
		auto t = m_buffer.back();
		t->set_position(position);
		m_turrets[map_pos] = t;
		m_buffer.pop_back();
	}

	return true;
}

bool turret_manager::remove_turret(glm::vec3 position)
{
	auto map_pos = from_glm(position);
	if (m_turrets.count(map_pos) > 0)
	{
		m_buffer.push_back(m_turrets[map_pos]);//return turret to bufer
		m_turrets.erase(map_pos);
		return true;
	}
	return false;//Cannot remove turret that does not exist.
}
