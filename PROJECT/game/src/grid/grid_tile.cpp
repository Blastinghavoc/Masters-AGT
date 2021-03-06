#include "grid_tile.h"

grid_tile::grid_tile()
{
}

grid_tile::~grid_tile()
{
}

void grid_tile::set_border(const engine::ref<engine::game_object>& obj, const orientation& facing)
{
	m_borders[facing] = obj;
}

void grid_tile::set_corner(const engine::ref<engine::game_object>& obj)
{
	m_corner = obj;
}

void grid_tile::set_floor(const engine::ref<engine::game_object>& obj)
{
	m_floor = obj;
}

void grid_tile::set_ceiling(const engine::ref<engine::game_object>& obj)
{
	m_ceiling = obj;
}

void grid_tile::del_border(const orientation& facing)
{
	m_borders.erase(facing);
}

void grid_tile::del_corner()
{
	m_corner = engine::ref<engine::game_object>();
}

//Submit all contained objects for rendering, provided the pointers to them are valid
void grid_tile::render(const engine::ref<engine::shader>& shader)
{
	if (m_floor)
	{
		engine::renderer::submit(shader, m_floor);
	}

	for each (auto pair in m_borders)
	{
		auto obj = pair.second;
		if (obj)
		{
			engine::renderer::submit(shader, obj);
		}
	}

	
	if (m_corner)
	{
		engine::renderer::submit(shader, m_corner);
	}
	

	if (m_ceiling)
	{
		engine::renderer::submit(shader, m_ceiling);
	}

	//DEBUG
	/*if (m_physics_object && m_physics_object->is_active())
	{
		m_physics_object->render_obb(glm::vec3(.5f),shader);
	}*/
}
