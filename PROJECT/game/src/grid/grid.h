#pragma once
#include <engine.h>
#include "grid_tile.h"

//Two dimensional grid of game objects
class grid {
public:
	grid();
	~grid();


private:
	std::map<glm::vec2, grid_tile> m_tiles{};
};
