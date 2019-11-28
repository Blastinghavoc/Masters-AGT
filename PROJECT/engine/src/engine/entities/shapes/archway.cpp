#include "pch.h"
#include "archway.h"
#include "engine.h"
#define _USE_MATH_DEFINES
#include <math.h>

engine::archway::archway(float width, float square_height, float arch_height,float depth)
{
	std::vector<engine::mesh::vertex> face_vertices{};
	std::vector<uint32_t> face_indices{};
	uint32_t face_index = 0;

	float half_width = width / 2;	
	float half_depth = depth / 2;

	float theta = M_PI;

	glm::vec3 norm;
	glm::vec3 axis_of_symmetry;
	engine::mesh::vertex bl{};//bottom left
	engine::mesh::vertex br{};//bottom right
	engine::mesh::vertex tr{};//top right
	engine::mesh::vertex tl{};//top left
	std::vector<engine::mesh::vertex> tmp_vertices{};

	//Front and back square faces
	norm = { 0,0,1 };
	axis_of_symmetry = { 0,1,0 };
	bl = { {-half_width, 0, half_depth}, norm, { 0,0 } };
	br = { {half_width, 0, half_depth}, norm, { 1,0 } };
	tr = { {half_width, square_height, half_depth}, norm, { 1,1 } };
	tl = { {-half_width, square_height, half_depth}, norm, { 0,1 } };
	tmp_vertices = { bl,br,tr,tl };

	shape_utils::add_quads(2, theta, tmp_vertices, face_vertices, face_indices, face_index, axis_of_symmetry);

	//Left and right square faces
	norm = { 1,0,0 };
	axis_of_symmetry = { 0,1,0 };
	bl = { {half_width, 0, half_depth}, norm, { 0,0 } };
	br = { {half_width, 0, -half_depth}, norm, { 1,0 } };
	tr = { {half_width, square_height, -half_depth}, norm, { 1,1 } };
	tl = { {half_width, square_height, half_depth}, norm, { 0,1 } };
	tmp_vertices = { bl,br,tr,tl };

	shape_utils::add_quads(2, theta, tmp_vertices, face_vertices, face_indices, face_index, axis_of_symmetry);

	//Front and back Triangular arch segment
	norm = { 0,0,1 };
	axis_of_symmetry = { 0,1,0 };
	bl = { {0, square_height, half_depth}, norm, { 0,0 } };
	br = { {half_width/2, square_height + arch_height/2, half_depth}, norm, { 1,0 } };
	tr = { {0, square_height + arch_height, half_depth}, norm, { 1,1 } };
	tmp_vertices = { bl,br,tr };
	shape_utils::add_tris(2, theta, tmp_vertices, face_vertices, face_indices, face_index, axis_of_symmetry);


	m_mesh = engine::mesh::create(face_vertices, face_indices);
}
