#pragma once
#include <engine.h>

enum orientation
{
	north = 0,
	east = 1,
	south = 2,
	west = 3,
	north_east = 4,
	south_east = 5,
	south_west = 6,
	north_west = 7,
};


glm::vec3 to_vec(orientation o);
