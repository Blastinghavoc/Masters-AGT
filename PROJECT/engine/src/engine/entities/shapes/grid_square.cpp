#include "pch.h"
#include "grid_square.h"
#include <engine.h>

engine::grid_square::grid_square(float width)
{
	if (width > 1.f)
	{
		//reset to default if invald
		width = .1f;
	}

	glm::vec3 norm{ 0,1,0 };
	std::vector<mesh::vertex> vertices
	{
		//Position		normal		tex
		//top-left corner
		{{0,0,0},norm,{0,0}},
		{{0,0,width},norm,{1,0}},
		{{width,0,width},norm,{1,1}},
		{{width,0,0},norm,{0,1}},

		//top-right corner
		{{1-width,0,0},norm,{0,0}},
		{{1 - width,0, width},norm,{1,0}},
		{{1,0,width},norm,{1,1}},
		{{1,0,0},norm,{0,1}},

		//bottom-left corner
		{{0,0,1 - width},norm,{0,0}},
		{{0,0,1},norm,{1,0}},
		{{width,0,1},norm,{1,1}},
		{{width,0,1 - width},norm,{0,1}},

		//bottom-right corner
		{{1 - width,0,1 - width},norm,{0,0}},
		{{1 - width,0,1},norm,{1,0}},
		{{1,0,1},norm,{1,1}},
		{{1,0,1 - width},norm,{0,1}},


	};

	std::vector<uint32_t> indices
	{
		 0,1,2,		0,2,3,
		 4,5,6,		4,6,7,
		 8,9,10,	8,10,11,
		 12,13,14,	12,14,15,

		 //sides
		 3,5,4,		3,2,5,
		 11,13,12,	11,10,13,
		 1,8,11,	1,11,2,
		 5,12,15,	5,14,6

	};

	m_mesh = engine::mesh::create(vertices, indices);
}

engine::grid_square::~grid_square()
{
}

engine::ref<engine::grid_square> engine::grid_square::create(float width)
{
	return std::make_shared<engine::grid_square>(width);
}
