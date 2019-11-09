#include "pch.h"
#include "rhombicuboctahedron.h"
#include <engine.h>
#define _USE_MATH_DEFINES
#include <math.h>

engine::rhombicuboctahedron::rhombicuboctahedron(float edge_length)
{
	if (edge_length <= 0.f)
	{		
		throw std::runtime_error("Cannot create shape with negative dimensions");
	}

	float half_length = edge_length / 2.f;
	float tri_side_length = edge_length*(float)cos(M_PI_4);
	float center_dist = half_length + tri_side_length;

	//Vertices for square faces
	std::vector<engine::mesh::vertex> vertices_sqr
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

		//Top and bottom square faces
		//Top-front
		{{-half_length,center_dist,+half_length},{0,.5f,.5f},{0,0} },//24
		{{-half_length,+half_length,center_dist},{0,.5f,.5f},{1,0} },
		{{+half_length,+half_length,center_dist},{0,.5f,.5f},{1,1} },
		{{+half_length,center_dist,+half_length},{0,.5f,.5f},{0,1} },

		//Top-back
		{{+half_length,center_dist,-half_length},{0,.5f,-.5f},{0,0} },//28
		{{+half_length,+half_length,-center_dist},{0,.5f,-.5f},{1,0} },
		{{-half_length,+half_length,-center_dist},{0,.5f,-.5f},{1,1} },
		{{-half_length,center_dist,-half_length},{0,.5f,-.5f},{0,1} },

		//Top-left
		{{+half_length,center_dist,+half_length},{.5f,.5f,0},{0,0} },//32
		{{center_dist,+half_length,+half_length},{.5f,.5f,0},{1,0} },
		{{center_dist,+half_length,-half_length},{.5f,.5f,0},{1,1} },
		{{+half_length,center_dist,-half_length},{.5f,.5f,0},{0,1} },

		//Top-right
		{{-half_length,center_dist,-half_length},{-.5f,.5f,0},{0,0} },//36
		{{-center_dist,+half_length,-half_length},{-.5f,.5f,0},{1,0} },
		{{-center_dist,+half_length,+half_length},{-.5f,.5f,0},{1,1} },
		{{-half_length,center_dist,+half_length},{-.5f,.5f,0},{0,1} },

		//Bottom-front
		{{+half_length,-center_dist,+half_length},{0,-.5f,.5f},{0,0} },//40
		{{+half_length,-half_length,center_dist},{0,-.5f,.5f},{1,0} },
		{{-half_length,-half_length,center_dist},{0,-.5f,.5f},{1,1} },
		{{-half_length,-center_dist,+half_length},{0,-.5f,.5f},{0,1} },

		//Bottom-back
		{{-half_length,-center_dist,-half_length},{0,-5.f,-.5f},{0,0} },//44
		{{-half_length,-half_length,-center_dist},{0,-5.f,-.5f},{1,0} },
		{{+half_length,-half_length,-center_dist},{0,-5.f,-.5f},{1,1} },
		{{+half_length,-center_dist,-half_length},{0,-5.f,-.5f},{0,1} },

		//Bottom-left
		{{+half_length,-center_dist,-half_length},{.5f,-.5f,0},{0,0} },//48
		{{center_dist,-half_length,-half_length},{.5f,-.5f,0},{1,0} },
		{{center_dist,-half_length,+half_length},{.5f,-.5f,0},{1,1} },
		{{+half_length,-center_dist,+half_length},{.5f,-.5f,0},{0,1} },

		//Bottom-right
		{{-half_length,-center_dist,+half_length},{-.5f,-.5f,0},{0,0} },//52
		{{-center_dist,-half_length,+half_length},{-.5f,-.5f,0},{1,0} },
		{{-center_dist,-half_length,-half_length},{-.5f,-.5f,0},{1,1} },
		{{-half_length,-center_dist,-half_length},{-.5f,-.5f,0},{0,1} },

		//Middle band square faces.
		//Front-left
		{{+half_length,-half_length,center_dist},{.5f,0,.5f},{0,0} },//56
		{{center_dist,-half_length,+half_length},{.5f,0,.5f},{1,0} },
		{{center_dist,+half_length,+half_length},{.5f,0,.5f},{1,1} },
		{{+half_length,+half_length,center_dist},{.5f,0,.5f},{0,1} },

		//Front-right
		{{-half_length,+half_length,center_dist},{-.5f,0,.5f},{0,0} },//60
		{{-center_dist,+half_length,+half_length},{-.5f,0,.5f},{1,0} },
		{{-center_dist,-half_length,+half_length},{-.5f,0,.5f},{1,1} },
		{{-half_length,-half_length,center_dist},{-.5f,0,.5f},{0,1} },

		//Back-left
		{ {+half_length,+half_length,-center_dist},{.5f,0,-.5f},{0,0} },//64
		{ {center_dist,+half_length,-half_length},{.5f,0,-.5f},{1,0} },
		{ {center_dist,-half_length,-half_length},{.5f,0,-.5f},{1,1} },
		{ {+half_length,-half_length,-center_dist},{.5f,0,-.5f},{0,1} },

		//Back-right
		{ {-half_length,-half_length,-center_dist},{-.5f,0,-.5f},{0,0} },//68
		{ {-center_dist,-half_length,-half_length},{-.5f,0,-.5f},{1,0} },
		{ {-center_dist,+half_length,-half_length},{-.5f,0,-.5f},{1,1} },
		{ {-half_length,+half_length,-center_dist},{-.5f,0,-.5f},{0,1} },
	};

	//vertices for the triangular faces
	std::vector<engine::mesh::vertex> vertices_tri
	{		
		//Triangular corners
		//Top-front-left
		{ {+half_length,center_dist,+half_length},{1.f / 3.f,1.f / 3.f,1.f / 3.f},{.5f,1.f} },//72
		{ {+half_length,+half_length,center_dist},{1.f / 3.f,1.f / 3.f,1.f / 3.f},{0.f,0.f} },
		{ {center_dist,+half_length,+half_length},{1.f / 3.f,1.f / 3.f,1.f / 3.f},{1.f,0.f} },

		//Top-front-right
		{ {-half_length,center_dist,+half_length},{-1.f / 3.f,1.f / 3.f,1.f / 3.f},{.5f,1.f} },//75
		{ {-center_dist,+half_length,+half_length},{-1.f / 3.f,1.f / 3.f,1.f / 3.f},{0.f,0.f} },
		{ {-half_length,+half_length,center_dist},{-1.f / 3.f,1.f / 3.f,1.f / 3.f},{1.f,0.f} },

		//Top-back-left
		{ {+half_length,center_dist,-half_length},{1.f / 3.f,1.f / 3.f,-1.f / 3.f},{.5f,1.f} },//78
		{ {center_dist,+half_length,-half_length},{1.f / 3.f,1.f / 3.f,-1.f / 3.f},{0.f,0.f} },
		{ {+half_length,+half_length,-center_dist},{1.f / 3.f,1.f / 3.f,-1.f / 3.f},{1.f,0.f} },

		//Top-back-right
		{ {-half_length,center_dist,-half_length},{-1.f / 3.f,1.f / 3.f,-1.f / 3.f},{.5f,1.f} },//81
		{ {-half_length,+half_length,-center_dist},{-1.f / 3.f,1.f / 3.f,-1.f / 3.f},{0.f,0.f} },
		{ {-center_dist,+half_length,-half_length},{-1.f / 3.f,1.f / 3.f,-1.f / 3.f},{1.f,0.f} },

		//Bottom-front-left
		{ {+half_length,-center_dist,+half_length},{1.f / 3.f,-1.f / 3.f,1.f / 3.f},{.5f,1.f} },//84
		{ {center_dist,-half_length,+half_length},{1.f / 3.f,-1.f / 3.f,1.f / 3.f},{0.f,0.f} },
		{ {+half_length,-half_length,center_dist},{1.f / 3.f,-1.f / 3.f,1.f / 3.f},{1.f,0.f} },

		//Bottom-front-right
		{ {-half_length,-center_dist,+half_length},{-1.f / 3.f,-1.f / 3.f,1.f / 3.f},{.5f,1.f} },//87
		{ {-half_length,-half_length,center_dist},{-1.f / 3.f,-1.f / 3.f,1.f / 3.f},{0.f,0.f} },
		{ {-center_dist,-half_length,+half_length},{-1.f / 3.f,-1.f / 3.f,1.f / 3.f},{1.f,0.f} },

		//Bottom-back-left
		{ {+half_length,-center_dist,-half_length},{1.f / 3.f,-1.f / 3.f,-1.f / 3.f},{.5f,1.f} },//90
		{ {+half_length,-half_length,-center_dist},{1.f / 3.f,-1.f / 3.f,-1.f / 3.f},{0.f,0.f} },
		{ {center_dist,-half_length,-half_length},{1.f / 3.f,-1.f / 3.f,-1.f / 3.f},{1.f,0.f} },

		//Bottom-back-right
		{ {-half_length,-center_dist,-half_length},{-1.f / 3.f,-1.f / 3.f,-1.f / 3.f},{.5f,1.f} },//93
		{ {-center_dist,-half_length,-half_length},{-1.f / 3.f,-1.f / 3.f,-1.f / 3.f},{0.f,0.f} },
		{ {-half_length,-half_length,-center_dist},{-1.f / 3.f,-1.f / 3.f,-1.f / 3.f},{1.f,0.f} },

	};

	//Indices for the square faces
	std::vector<uint32_t> indices_sqr
	{
		 0,1,2,		0,2,3,//Top
		 4,5,6,		4,6,7,//Bottom
		 8,9,10,	8,10,11,//Front
		 12,13,14,	12,14,15,//Back		 
		 16,17,18,	16,18,19,//left
		 20,21,22,	20,22,23,//right

		 //Square faces

		 //Top-front
		 24,25,26,	24,26,27,
		 //Top-back
		 28,29,30,	28,30,31,
		 //Top-left
		 32,33,34,	32,34,35,
		 //Top-right
		 36,37,38,	36,38,39,
		 //Bottom-front
		 40,41,42,	40,42,43,
		 //Bottom-back
		 44,45,46,	44,46,47,
		 //Bottom-left
		 48,49,50,	48,50,51,
		 //Bottom-right
		 52,53,54,	52,54,55,

		 //Front-left
		 56,57,58,	56,58,59,
		 //Front-right
		 60,61,62,	60,62,63,
		 //Back-left
		 64,65,66,	64,66,67,
		 //Back-right
		 68,69,70,	68,70,71, 

	};

	//Indices for the triangular faces
	std::vector<uint32_t> indices_tri
	{
		//Triangular corners

		 //top-front-left
		 72,73,74,
		 //top-front-right
		 75,76,77,
		 //top-back-left
		 78,79,80,
		 //top-back-right
		 81,82,83,

		 //bottom-front-left
		 84,85,86,
		 //bottom-front-right
		 87,88,89,
		 //bottom-back-left
		 90,91,92,
		 //bottom-back-right
		 93,94,95

	};

	//adjusting the triangular indices to start from 0.
	for (size_t i = 0; i < indices_tri.size(); ++i)
	{
		indices_tri[i] -= 72;
	}

	m_meshes.push_back(engine::mesh::create(vertices_sqr, indices_sqr));
	m_meshes.push_back(engine::mesh::create(vertices_tri, indices_tri));
}

engine::rhombicuboctahedron::~rhombicuboctahedron()
{
}

engine::ref<engine::rhombicuboctahedron> engine::rhombicuboctahedron::create(float edge_length)
{
	return std::make_shared<engine::rhombicuboctahedron>(edge_length);
}
