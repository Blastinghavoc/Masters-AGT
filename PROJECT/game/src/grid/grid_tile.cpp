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

void grid_tile::set_floor(const engine::ref<engine::game_object>& obj)
{
	m_floor = obj;
}

//Submit all contained objects for rendering, provided the pointers to them are valid
void grid_tile::render(const engine::ref<engine::shader>& shader)
{
	if (m_floor)
	{
		engine::renderer::submit(shader, m_floor);
	}

	for each (auto obj in m_borders)
	{
		if (obj)
		{
			engine::renderer::submit(shader, obj);
		}
	}

	for each (auto obj in m_corners)
	{
		if (obj)
		{
			engine::renderer::submit(shader, obj);
		}
	}

	if (m_ceiling)
	{
		engine::renderer::submit(shader, m_ceiling);
	}
}
