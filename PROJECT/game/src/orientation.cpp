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
		return to_vec(north) + to_vec(east);
	case north_west:
		return to_vec(north) + to_vec(west);
	case south_east:
		return to_vec(south) + to_vec(east);
	case south_west:
		return to_vec(south) + to_vec(west);

	default:
		throw std::exception();
	}
}

orientation invert(orientation o)
{
	switch (o)
	{
	case north:
		return south;
		break;
	case east:
		return west;
		break;
	case south:
		return north;
		break;
	case west:
		return east;
		break;
	case north_east:
		return south_west;
		break;
	case south_east:
		return north_west;
		break;
	case south_west:
		return north_east;
		break;
	case north_west:
		return south_east;
		break;
	default:
		//Not possible
		throw std::exception();
		break;
	}
}

std::vector<orientation> cardinal_to_composite(orientation o)
{	
	switch (o)
	{
	case north:
		return { north_east,north_west };
		break;
	case east:
		return { north_east,south_east };
		break;
	case south:
		return { south_east,south_west };
		break;
	case west:
		return { north_west,south_west };
		break;	
	default:
		throw std::exception();
		break;
	}
}

std::vector<orientation> composite_components(orientation o)
{
	switch (o)
	{	
	case north_east:
		return { north, east };
		break;
	case south_east:
		return { south, east };
		break;
	case south_west:
		return { south, west };
		break;
	case north_west:
		return { north, west };
		break;
	default:
		throw std::exception();
		break;
	}
}
