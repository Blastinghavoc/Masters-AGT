#include "pch.h"
#include "rhombicuboctahedron.h"
#include <engine.h>
#define _USE_MATH_DEFINES
#include <math.h>

engine::rhombicuboctahedron::rhombicuboctahedron(float edge_length)
{
	if (edge_length <= 0.f)
	{		
		edge_length = 1.f;
	}

	float half_length = edge_length / 2.f;
	float tri_side_length = edge_length*cos(M_PI_4);
	float center_dist = half_length + tri_side_length;


	//TODO may need toduplicate vertices to have different texture coords and normals
	glm::vec3 norm{ 0,1,0 };
	std::vector<engine::mesh::vertex> vertices
	{
		//Position		normal		tex
		//Top
		{{-half_length,center_dist,-half_length},{0,1,0},{0,0} },//0
		{{-half_length,center_dist,+half_length},{0,1,0},{1,0} },//1
		{{+half_length,center_dist,+half_length},{0,1,0},{1,1} },//2
		{{+half_length,center_dist,-half_length},{0,1,0},{0,1} },//3

		//Bottom
		{{+half_length,-center_dist,-half_length},{0,-1,0},{0,0} },//4
		{{+half_length,-center_dist,+half_length},{0,-1,0},{1,0} },//5
		{{-half_length,-center_dist,+half_length},{0,-1,0},{1,1} },//6
		{{-half_length,-center_dist,-half_length},{0,-1,0},{0,1} },//7

		//Front
		{{-half_length,+half_length,center_dist},{0,0,1},{0,0} },//8
		{{-half_length,-half_length,center_dist},{0,0,1},{1,0} },//9
		{{+half_length,-half_length,center_dist},{0,0,1},{1,1} },//10
		{{+half_length,+half_length,center_dist},{0,0,1},{0,1} },//11

		//Back
		{{+half_length,+half_length,-center_dist},{0,0,-1},{0,0} },//12
		{{+half_length,-half_length,-center_dist},{0,0,-1},{1,0} },//13
		{{-half_length,-half_length,-center_dist},{0,0,-1},{1,1} },//14
		{{-half_length,+half_length,-center_dist},{0,0,-1},{0,1} },//15

		//Left (pos)
		{{center_dist,+half_length,+half_length},{1,0,0},{0,0} },//16
		{{center_dist,-half_length,+half_length},{1,0,0},{1,0} },
		{{center_dist,-half_length,-half_length},{1,0,0},{1,1} },
		{{center_dist,+half_length,-half_length},{1,0,0},{0,1} },

		//Right (neg)
		{{-center_dist,+half_length,-half_length},{-1,0,0},{0,0} },//20
		{{-center_dist,-half_length,-half_length},{-1,0,0},{1,0} },
		{{-center_dist,-half_length,+half_length},{-1,0,0},{1,1} },
		{{-center_dist,+half_length,+half_length},{-1,0,0},{0,1} },

	};

	std::vector<uint32_t> indices
	{
		 0,1,2,		0,2,3,//Top
		 4,5,6,		4,6,7,//Bottom
		 8,9,10,	8,10,11,//Front
		 12,13,14,	12,14,15,//Back

		 //sides
		 16,17,18,		16,18,19,
		 20,21,22,	20,22,23,

		 //Square faces

		 //Top-front
		 1,8,11,	1,11,2,
		 //Top-back
		 3,12,15,	3,15,0,
		 //Top-left
		 2,16,19,	2,19,3,
		 //Top-right
		 0,20,23,	0,23,1,
		 //Bottom-front
		 5,10,9,	5,9,6,
		 //Bottom-back
		 7,14,13,	7,13,4,
		 //Bottom-left
		 4,18,17,	4,17,5,
		 //Bottom-right
		 6,22,21,	6,21,7,

		 //Front-left
		 10,17,16,	10,16,11,
		 //Front-right
		 8,23,22,	8,22,9,
		 //Back-left
		 12,19,18,	12,18,13,
		 //Back-right
		 14,21,20,	14,20,15,

		 //Triangular corners

		 //top-front-left
		 2,11,16,
		 //top-front-right
		 1,23,8,
		 //top-back-left
		 3,19,12,
		 //top-back-right
		 0,15,20,
		 //bottom-front-left
		 5,17,10,
		 //bottom-front-right
		 6,9,22,
		 //bottom-back-left
		 4,13,18,
		 //bottom-back-right
		 7,21,14		 

	};

	m_mesh = engine::mesh::create(vertices, indices);
}

engine::rhombicuboctahedron::~rhombicuboctahedron()
{
}

engine::ref<engine::rhombicuboctahedron> engine::rhombicuboctahedron::create(float edge_length)
{
	return std::make_shared<engine::rhombicuboctahedron>(edge_length);
}
