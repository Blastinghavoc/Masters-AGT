#include "pch.h"
#include "tiled_cuboid.h"
#include <engine.h>

//Modification of cuboid constructor to support texture tiling.
engine::tiled_cuboid::tiled_cuboid(glm::vec3 half_extents, bool inwards, glm::vec2 tile_scale) : m_half_extents(half_extents), m_inwards(inwards)
{
	float orientation = 1;
	if (inwards)
		orientation = -1;

	//Construct vertices the same way as for the basic cuboid
	std::vector<mesh::vertex> vertices
	{
		//front
		//                                 position														 normal					      tex coord       
		{ {-1.f * m_half_extents.x, -1.f * m_half_extents.y,  1.f * m_half_extents.z },		{ 0.0f, 0.0f,  1.0f * orientation},		{ 0.f, 0.f } },
		{ { 1.f * m_half_extents.x, -1.f * m_half_extents.y,  1.f * m_half_extents.z },		{ 0.0f, 0.0f,  1.0f * orientation},		{ 1.f, 0.f } },
		{ { 1.f * m_half_extents.x,  1.f * m_half_extents.y,  1.f * m_half_extents.z },		{ 0.0f, 0.0f,  1.0f * orientation},		{ 1.f, 1.f } },
		{ {-1.f * m_half_extents.x,  1.f * m_half_extents.y,  1.f * m_half_extents.z },		{ 0.0f, 0.0f,  1.0f * orientation},		{ 0.f, 1.f } },

		//right
		{ { 1.f * m_half_extents.x, -1.f * m_half_extents.y,  1.f * m_half_extents.z },		{ 1.0f * orientation, 0.0f, 0.0f},		{ 0.f, 0.f } },
		{ { 1.f * m_half_extents.x, -1.f * m_half_extents.y, -1.f * m_half_extents.z },		{ 1.0f * orientation, 0.0f, 0.0f},		{ 1.f, 0.f } },
		{ { 1.f * m_half_extents.x,  1.f * m_half_extents.y, -1.f * m_half_extents.z },		{ 1.0f * orientation, 0.0f, 0.0f},		{ 1.f, 1.f } },
		{ { 1.f * m_half_extents.x,  1.f * m_half_extents.y,  1.f * m_half_extents.z },		{ 1.0f * orientation, 0.0f, 0.0f},		{ 0.f, 1.f } },

		//back
		{ { 1.f * m_half_extents.x, -1.f * m_half_extents.y, -1.f * m_half_extents.z },		{ 0.0f, 0.0f, -1.0f * orientation},		{ 0.f, 0.f } },
		{ {-1.f * m_half_extents.x, -1.f * m_half_extents.y, -1.f * m_half_extents.z },		{ 0.0f, 0.0f, -1.0f * orientation},		{ 1.f, 0.f } },
		{ {-1.f * m_half_extents.x,  1.f * m_half_extents.y, -1.f * m_half_extents.z },		{ 0.0f, 0.0f, -1.0f * orientation},		{ 1.f, 1.f } },
		{ { 1.f * m_half_extents.x,  1.f * m_half_extents.y, -1.f * m_half_extents.z },		{ 0.0f, 0.0f, -1.0f * orientation},		{ 0.f, 1.f } },

		//left
		{ {-1.f * m_half_extents.x, -1.f * m_half_extents.y, -1.f * m_half_extents.z },		{-1.0f * orientation, 0.0f, 0.0f},		{ 0.f, 0.f } },
		{ {-1.f * m_half_extents.x, -1.f * m_half_extents.y,  1.f * m_half_extents.z },		{-1.0f * orientation, 0.0f, 0.0f},		{ 1.f, 0.f } },
		{ {-1.f * m_half_extents.x,  1.f * m_half_extents.y,  1.f * m_half_extents.z },		{-1.0f * orientation, 0.0f, 0.0f},		{ 1.f, 1.f } },
		{ {-1.f * m_half_extents.x,  1.f * m_half_extents.y, -1.f * m_half_extents.z },		{-1.0f * orientation, 0.0f, 0.0f},		{ 0.f, 1.f } },

		//top
		{ {-1.f * m_half_extents.x,  1.f * m_half_extents.y,  1.f * m_half_extents.z },		{ 0.0f,  1.0f * orientation, 0.0f},		{ 0.f, 0.f } },
		{ { 1.f * m_half_extents.x,  1.f * m_half_extents.y,  1.f * m_half_extents.z },		{ 0.0f,  1.0f * orientation, 0.0f},		{ 1.f, 0.f } },
		{ { 1.f * m_half_extents.x,  1.f * m_half_extents.y, -1.f * m_half_extents.z },		{ 0.0f,  1.0f * orientation, 0.0f},		{ 1.f, 1.f } },
		{ {-1.f * m_half_extents.x,  1.f * m_half_extents.y, -1.f * m_half_extents.z },		{ 0.0f,  1.0f * orientation, 0.0f},		{ 0.f, 1.f } },

		//bottom
		{ {-1.f * m_half_extents.x, -1.f * m_half_extents.y, -1.f * m_half_extents.z },		{ 0.0f, -1.0f * orientation, 0.0f},		{ 0.f, 0.f } },
		{ { 1.f * m_half_extents.x, -1.f * m_half_extents.y, -1.f * m_half_extents.z },		{ 0.0f, -1.0f * orientation, 0.0f},		{ 1.f, 0.f } },
		{ { 1.f * m_half_extents.x, -1.f * m_half_extents.y,  1.f * m_half_extents.z },		{ 0.0f, -1.0f * orientation, 0.0f},		{ 1.f, 1.f } },
		{ {-1.f * m_half_extents.x, -1.f * m_half_extents.y,  1.f * m_half_extents.z },		{ 0.0f, -1.0f * orientation, 0.0f},		{ 0.f, 1.f } },
	};

	//Scaling texture coords to support tiling
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		vertices[i].tex_coords *= tile_scale;
	}

	const std::vector<uint32_t> indices
	{
		 0,  1,  2,		 0,  2,  3,  //front
		 4,  5,  6,		 4,  6,  7,  //right
		 8,  9, 10,		 8, 10, 11,  //back
		12, 13, 14,		12, 14, 15,  //left
		16, 17, 18,		16, 18, 19,  //upper
		20, 21, 22,		20, 22, 23   //bottom
	};

	m_mesh = engine::mesh::create(vertices, indices);
}

engine::tiled_cuboid::~tiled_cuboid() {}

engine::ref<engine::tiled_cuboid> engine::tiled_cuboid::create(glm::vec3 half_extents, bool inwards, glm::vec2 tile_scale)
{
	return std::make_shared<engine::tiled_cuboid>(half_extents, inwards,tile_scale);
}
