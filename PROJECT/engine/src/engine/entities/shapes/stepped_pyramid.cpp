#include "pch.h"
#include "stepped_pyramid.h"
#define _USE_MATH_DEFINES
#include <math.h>

engine::stepped_pyramid::stepped_pyramid(float height, float top_radius, float bottom_radius, int num_steps,float border_fraction,int num_sides)
{
	if (height <= 0.f || top_radius <= 0.f || bottom_radius<= 0.f)
	{
		throw std::runtime_error("Cannot create shape with negative dimensions");
	}

	if (num_steps < 1)
	{
		throw std::runtime_error("Cannot create stepped pyramid with less than one step");
	}

	if (num_sides < 3)
	{
		throw std::runtime_error("Cannot create stepped pyramid with less than 3 sides");
	}

	const float step_height = height / num_steps;

	bool make_border = false;
	float border_width = 0.f;

	if (border_fraction > 0.f)
	{
		if (border_fraction > 1.f)
		{
			throw std::runtime_error("Cannot creat stepped pyramid with a border fraction greater than 1");
		}
		make_border = true;
		border_width = step_height * border_fraction;
	}

	//Change in radius for each step
	const float delta_radius = (top_radius - bottom_radius) / num_steps;

	float current_radius = bottom_radius;
	float theta = (float)(M_PI / num_sides);
	const float two_theta = theta * 2;//Angle to rotate by when changing faces
	float current_height = 0.f;

	const auto axis_of_symmetry = glm::vec3(0, 1, 0);

	//Faces
	std::vector<engine::mesh::vertex> face_vertices{};
	std::vector<uint32_t> face_indices{};
	uint32_t face_index = 0;

	//Borders
	std::vector<engine::mesh::vertex> border_vertices{};
	std::vector<uint32_t> border_indices{};
	uint32_t border_index = 0;

	float current_half_length = 0.f;
	float next_radius = 0.f;
	float next_half_length = 0.f;
	float next_height = 0.f;
	float x = 0.f;
	float z = 0.f;
	float next_x = 0.f;
	float next_y = 0.f;	
	float x_prime = 0.f;
	glm::vec3 norm{};
	//Front facing vertices, will be rotated to each required facing
	engine::mesh::vertex bl{};//bottom left
	engine::mesh::vertex br{};//bottom right
	engine::mesh::vertex tr{};//top right
	engine::mesh::vertex tl{};//top left
	std::vector<engine::mesh::vertex> tmp_vertices{};

	for (int step = 0; step < num_steps; ++step) {
		//Update variables
		current_half_length = current_radius * tan(theta);
		next_radius = current_radius + delta_radius;
		next_half_length = next_radius * tan(theta);
		next_height = current_height + step_height;

		x = current_half_length;
		z = current_radius;
		next_x = next_half_length;
		next_y = next_height;
		if (make_border)
		{
			x -= border_width;
			next_x -= border_width;
			next_y -= border_width;
			z -= border_width;
		}
		x_prime = z * tan(theta);

		//---Sides

		norm={ 0,0,1 };
		
		bl={ {-x, current_height, current_radius}, norm, { 0,0 } };
		br={ {x, current_height, current_radius}, norm, { 0,1 } };
		tr={ {x, next_y, current_radius}, norm, { 1,1 } };
		tl={ {-x, next_y, current_radius}, norm, { 1,0 } };
		tmp_vertices={ bl,br,tr,tl };

		shape_utils::add_quads(num_sides, two_theta, tmp_vertices, face_vertices, face_indices, face_index, axis_of_symmetry);

		//---Top faces (trapezium shaped)	

		norm = { 0,1,0 };//Normal now facing up
		bl={ {-x_prime, next_height, z}, norm, { 0,0 } };
		br={ {x_prime, next_height, z}, norm, { 0,1 } };
		tr={ {next_half_length, next_height, next_radius}, norm, { 1,1 } };
		tl={ {-next_half_length, next_height, next_radius}, norm, { 1,0 } };
		tmp_vertices={ bl,br,tr,tl };
		shape_utils::add_quads(num_sides, two_theta, tmp_vertices, face_vertices, face_indices, face_index, axis_of_symmetry);

		//--Borders
		/*
		Each side face has 3 border strips: the right, left and top edges.
		Each upward face has one border strip, the step edge.
		*/
		if (make_border)
		{
			//Left strip
			norm = { 0,0,1 };
			bl = { {-current_half_length, current_height, current_radius}, norm, { 0,0 } };
			br = { {-x, current_height, current_radius}, norm, { 0,1 } };
			tr = { {-x, next_height, current_radius}, norm, { 1,1 } };
			tl = { {-current_half_length, next_height, current_radius}, norm, { 1,0 } };
			tmp_vertices = { bl,br,tr,tl };
			shape_utils::add_quads(num_sides, two_theta, tmp_vertices, border_vertices, border_indices, border_index, axis_of_symmetry);

			//Right strip
			norm = { 0,0,1 };
			bl = { {x, current_height, current_radius}, norm, { 0,0 } };
			br = { {current_half_length, current_height, current_radius}, norm, { 0,1 } };
			tr = { {current_half_length, next_height, current_radius}, norm, { 1,1 } };
			tl = { {x, next_height, current_radius}, norm, { 1,0 } };
			tmp_vertices = { bl,br,tr,tl };
			shape_utils::add_quads(num_sides, two_theta, tmp_vertices, border_vertices, border_indices, border_index, axis_of_symmetry);

			//Top strip
			norm = { 0,0,1 };
			bl = { {-x, next_y, current_radius}, norm, { 0,0 } };
			br = { {x, next_y, current_radius}, norm, { 0,1 } };
			tr = { {x, next_height, current_radius}, norm, { 1,1 } };
			tl = { {-x, next_height, current_radius}, norm, { 1,0 } };
			tmp_vertices = { bl,br,tr,tl };
			shape_utils::add_quads(num_sides, two_theta, tmp_vertices, border_vertices, border_indices, border_index, axis_of_symmetry);

			//upward strip (trapezium shape)
			norm = { 0,1,0 };
			bl = { {-current_half_length, next_height, current_radius}, norm, { 0,0 } };
			br = { {current_half_length, next_height, current_radius}, norm, { 0,1 } };
			tr = { {x_prime, next_height, z}, norm, { 1,1 } };
			tl = { {-x_prime, next_height, z}, norm, { 1,0 } };
			tmp_vertices = { bl,br,tr,tl };
			shape_utils::add_quads(num_sides, two_theta, tmp_vertices, border_vertices, border_indices, border_index, axis_of_symmetry);
		}

		current_radius = next_radius;
		current_height = next_height;
	}

	//The peak upward face of the pyramid, which is an N-gon where N is num_sides
	norm = { 0,1,0 };
	bl = { {-x, next_height, z}, norm, { 0,0 } };
	br = { {x, next_height, z}, norm, { 0,1 } };
	tr = { {0, next_height, 0}, norm, { .5f,.5f } };
	tmp_vertices = { bl,br,tr };
	shape_utils::add_tris(num_sides, two_theta, tmp_vertices, face_vertices, face_indices, face_index, axis_of_symmetry);

	//The bottom face
	auto bottom_h_len = bottom_radius * tan(theta);
	norm = { 0,-1,0 };//Inverted normal and opposite winding so that it faces down.
	bl = { {-bottom_h_len, 0, bottom_radius}, norm, { 0,0 } };
	br = { {0, 0, 0}, norm, { 0.5f,.5f } };
	tr = { {bottom_h_len, 0, bottom_radius}, norm, { 0,1 } };
	tmp_vertices = { bl,br,tr };
	shape_utils::add_tris(num_sides, two_theta, tmp_vertices, face_vertices, face_indices, face_index, axis_of_symmetry);

	m_meshes.push_back(engine::mesh::create(face_vertices, face_indices));
	m_meshes.push_back(engine::mesh::create(border_vertices, border_indices));
}

engine::stepped_pyramid::~stepped_pyramid()
{
}

engine::ref<engine::stepped_pyramid> engine::stepped_pyramid::create(float height, float top_radius, float bottom_radius, int num_steps,float border_width,int num_sides)
{
	return  std::make_shared<stepped_pyramid>(height, top_radius, bottom_radius,num_steps,border_width,num_sides);
}
