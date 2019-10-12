#include "grid_tile.h"

grid_tile::grid_tile()
{
}

grid_tile::~grid_tile()
{
}

void grid_tile::render(const engine::ref<engine::shader>& shader)
{
	engine::renderer::submit(shader, m_floor);

	for each (auto obj in m_borders)
	{
		engine::renderer::submit(shader, obj);
	}

	for each (auto obj in m_corners)
	{
		engine::renderer::submit(shader, obj);
	}
	
	engine::renderer::submit(shader, m_ceiling);	
}
