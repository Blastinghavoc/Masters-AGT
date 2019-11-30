#include "orientation.h"

//Static initializers
const orientation orientation::north(orientation::north_val, true);
const orientation orientation::east(orientation::east_val, true);
const orientation orientation::south(orientation::south_val, true);
const orientation orientation::west(orientation::west_val, true);
const orientation orientation::north_east(orientation::north_east_val, false);
const orientation orientation::south_east(orientation::south_east_val, false);
const orientation orientation::south_west(orientation::south_west_val, false);
const orientation orientation::north_west(orientation::north_west_val, false);

//Convert orientation to vector representation
glm::vec3 orientation::to_vec() const
{
	switch (this->m_value)
	{
	case north_val:
		return { 0,0,1 };
	case south_val:
		return { 0,0,-1 };
	case east_val:
		return { -1,0,0 };
	case west_val:
		return { 1,0,0 };
	case north_east_val:
		return north.to_vec() + east.to_vec();
	case north_west_val:
		return north.to_vec() + west.to_vec();
	case south_east_val:
		return south.to_vec() + east.to_vec();
	case south_west_val:
		return south.to_vec() + west.to_vec();

	default:
		throw std::exception();
	}
}

//Invert orientation
orientation orientation::invert() const
{
	switch (this->m_value)
	{
	case north_val:
		return south;
		break;
	case east_val:
		return west;
		break;
	case south_val:
		return north;
		break;
	case west_val:
		return east;
		break;
	case north_east_val:
		return south_west;
		break;
	case south_east_val:
		return north_west;
		break;
	case south_west_val:
		return north_east;
		break;
	case north_west_val:
		return south_east;
		break;
	default:
		//Not possible
		throw std::exception();
		break;
	}
}

//Return the composite components of this cardinal orientation
std::vector<orientation> orientation::composite_components() const
{
	switch (this->m_value)
	{
	case north_val:
		return { north_east,north_west };
		break;
	case east_val:
		return { north_east,south_east };
		break;
	case south_val:
		return { south_east,south_west };
		break;
	case west_val:
		return { north_west,south_west };
		break;
	default:
		//Undefined if this is not a cardinal orientation
		throw std::exception();
		break;
	}	
}

//Return the cardinal components of this cardinal direction
std::vector<orientation> orientation::cardinal_components() const
{
	switch (this->m_value)
	{
	case north_east_val:
		return { north, east };
		break;
	case south_east_val:
		return { south, east };
		break;
	case south_west_val:
		return { south, west };
		break;
	case north_west_val:
		return { north, west };
		break;
	default:
		//Undefined if this is not a composite orientation
		throw std::exception();
		break;
	}
}

orientation::orientation(unsigned short val,bool is_card) :m_value{val},
m_is_cardinal{is_card}
{	
}

orientation::~orientation()
{
}
