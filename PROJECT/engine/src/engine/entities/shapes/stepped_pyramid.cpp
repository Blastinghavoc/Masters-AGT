#include "pch.h"
#include "stepped_pyramid.h"
#include <engine.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/gtx/rotate_vector.hpp>

engine::stepped_pyramid::stepped_pyramid(float height, float top_side_length, float bottom_side_length, int num_steps,float border_fraction)
{
	if (height <= 0.f || top_side_length <= 0.f || bottom_side_length<= 0.f)
	{
		throw std::runtime_error("Cannot create shape with negative dimensions");
	}

	if (num_steps < 1)
	{
		throw std::runtime_error("Cannot create stepped pyramid with less than one step");
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

	//Change in length for each step
	const float delta_length = (top_side_length - bottom_side_length) / num_steps;

	float current_side_length = bottom_side_length;
	float current_height = 0.f;

	//--Faces
	std::vector<engine::mesh::vertex> face_vertices{};
	std::vector<uint32_t> face_indices{};
	int index = 0;

	for (int step = 0; step < num_steps; ++step) {
		const float half_length = current_side_length / 2;
		const float next_length = current_side_length + delta_length;
		const float next_half_length = next_length / 2;
		const float next_height = current_height + step_height;

		//---Sides
		float x = half_length;
		float next_x = next_half_length;
		float next_y = next_height;
		if (make_border)
		{
			x -= border_width;
			next_x -= border_width;
			next_y -= border_width;
		}

		glm::vec3 norm{ 0,0,1 };
		//Front facing vertices
		engine::mesh::vertex bl{ {-x, current_height, half_length}, norm, { 0,0 } };
		engine::mesh::vertex br{ {x, current_height, half_length}, norm, { 0,1 } };
		engine::mesh::vertex tr{ {x, next_y, half_length}, norm, { 1,1 } };
		engine::mesh::vertex tl{ {-x, next_y, half_length}, norm, { 1,0 } };
		std::vector<engine::mesh::vertex> tmp_face_vertices{ bl,br,tr,tl };

		float rotation_amnt = M_PI_2;
		for (size_t i = 0; i < 4; i++)
		{
			float angle = i * rotation_amnt;
			for (auto& vert : tmp_face_vertices) {
				vert.position = glm::rotateY(vert.position, angle);
				vert.normal = glm::rotateY(vert.normal, angle);

				face_vertices.push_back(vert);
			}

			face_indices.push_back(index);
			face_indices.push_back(index + 1);
			face_indices.push_back(index + 2);

			face_indices.push_back(index);
			face_indices.push_back(index + 2);
			face_indices.push_back(index + 3);

			index += 4;
		}

		//---Top faces

		norm = { 0,1,0 };//Normal now facing up
		bl={ {-next_half_length, next_height, x}, norm, { 0,0 } };
		br={ {next_half_length, next_height, x}, norm, { 0,1 } };
		tr={ {next_half_length, next_height, next_half_length}, norm, { 1,1 } };
		tl={ {-next_half_length, next_height, next_half_length}, norm, { 1,0 } };
		tmp_face_vertices={ bl,br,tr,tl };
		for (size_t i = 0; i < 4; i++)
		{
			float angle = i * rotation_amnt;
			for (auto& vert : tmp_face_vertices) {
				vert.position = glm::rotateY(vert.position, angle);

				face_vertices.push_back(vert);
			}

			face_indices.push_back(index);
			face_indices.push_back(index + 1);
			face_indices.push_back(index + 2);

			face_indices.push_back(index);
			face_indices.push_back(index + 2);
			face_indices.push_back(index + 3);

			index += 4;
		}

		current_side_length = next_length;
		current_height = next_height;
	}

	//--Borders
	std::vector<engine::mesh::vertex> border_vertices{};
	std::vector<uint32_t> border_indices{};
	index = 0;

	m_meshes.push_back(engine::mesh::create(face_vertices, face_indices));
}

engine::stepped_pyramid::~stepped_pyramid()
{
}

engine::ref<engine::stepped_pyramid> engine::stepped_pyramid::create(float height, float top_side_length, float bottom_side_length, int num_steps,float border_width)
{
	return  std::make_shared<stepped_pyramid>(height,top_side_length,bottom_side_length,num_steps,border_width);
}
