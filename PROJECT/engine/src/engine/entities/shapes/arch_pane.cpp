#include "pch.h"
#include "arch_pane.h"
#include "engine.h"
#define _USE_MATH_DEFINES
#include <math.h>

engine::arch_pane::arch_pane(float width, float square_height, float arch_height)
{
	std::vector<engine::mesh::vertex> face_vertices{};
	std::vector<uint32_t> face_indices{};
	uint32_t face_index = 0;

	float half_width = width / 2;

	float theta = M_PI;

	float total_height = arch_height + square_height;
	float square_tex = square_height / total_height;
	float arch_x = half_width / 1.5;
	float arch_y = square_height + arch_height / 1.5;
	float arch_x_tex = arch_x / width;
	float arch_y_tex = arch_y / total_height;

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
	bl = { {-half_width, 0, 0}, norm, { 0,0 } };
	br = { {half_width, 0, 0}, norm, { 1,0 } };
	tr = { {half_width, square_height, 0}, norm, { 1,square_tex } };
	tl = { {-half_width, square_height, 0}, norm, { 0,square_tex } };
	tmp_vertices = { bl,br,tr,tl };

	shape_utils::add_quads(2, theta, tmp_vertices, face_vertices, face_indices, face_index, axis_of_symmetry);

	//Front and back arch segment right
	norm = { 0,0,1 };
	axis_of_symmetry = { 0,1,0 };
	bl = { {0, square_height, 0}, norm, { 0.5f,square_tex } };
	br = { {half_width,square_height,0}, norm, { 1,square_tex } };
	tr = { {arch_x, arch_y, 0}, norm, { 1-arch_x_tex,arch_y_tex } };
	tl = { {0, total_height, 0}, norm, { 0.5f,1 } };
	tmp_vertices = { bl,br,tr,tl };
	shape_utils::add_quads(2, theta, tmp_vertices, face_vertices, face_indices, face_index, axis_of_symmetry);

	//Front and back arch segment left
	norm = { 0,0,1 };
	axis_of_symmetry = { 0,1,0 };
	bl = { {-half_width,square_height,0}, norm, { 0,square_tex } };
	br = { {0, square_height, 0}, norm, { 0.5f,square_tex } };
	tr = { {0, total_height, 0}, norm, { 0.5f,1 } };
	tl = { {-arch_x, arch_y, 0}, norm, { arch_x_tex,arch_y_tex } };
	tmp_vertices = { bl,br,tr,tl };
	shape_utils::add_quads(2, theta, tmp_vertices, face_vertices, face_indices, face_index, axis_of_symmetry);


	m_mesh = engine::mesh::create(face_vertices, face_indices);
}
