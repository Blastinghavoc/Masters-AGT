#include "orientation.h"

glm::vec3 to_vec(orientation o)
{
	switch (o)
	{
	case north:
		return { 0,0,1 };
	case south:
		return { 0,0,-1 };
	case east:
		return { -1,0,0 };
	case west:
		return { 1,0,0 };
	case north_east:
		return glm::normalize(to_vec(north) + to_vec(east));
	case north_west:
		return glm::normalize(to_vec(north) + to_vec(west));
	case south_east:
		return glm::normalize(to_vec(south) + to_vec(east));
	case south_west:
		return glm::normalize(to_vec(south) + to_vec(west));

	default:
		throw std::exception();
	}
}
