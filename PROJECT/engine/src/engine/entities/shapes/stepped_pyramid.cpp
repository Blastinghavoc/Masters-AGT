#include "pch.h"
#include "stepped_pyramid.h"
#include <engine.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/gtx/rotate_vector.hpp>

engine::stepped_pyramid::stepped_pyramid(float height, float top_side_length, float bottom_side_length, int num_steps)
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

	//Change in length for each step
	const float delta_length = (top_side_length - bottom_side_length) / num_steps;

	float current_side_length = bottom_side_length;
	float current_height = 0.f;

	//Vertices
	std::vector<engine::mesh::vertex> vertices{};

	//Indices
	std::vector<uint32_t> indices{};
	int index = 0;

	for (int step = 0; step < num_steps; ++step) {
		const float half_length = current_side_length / 2;
		const float next_length = current_side_length + delta_length;
		const float next_half_length = next_length / 2;
		const float next_height = current_height + step_height;

		//---Sides

		glm::vec3 norm{ 0,0,1 };
		//Front facing vertices
		engine::mesh::vertex bl{ {-half_length, current_height, half_length}, norm, { 0,0 } };
		engine::mesh::vertex br{ {half_length, current_height, half_length}, norm, { 0,1 } };
		engine::mesh::vertex tr{ {half_length, next_height, half_length}, norm, { 1,1 } };
		engine::mesh::vertex tl{ {-half_length, next_height, half_length}, norm, { 1,0 } };
		std::vector<engine::mesh::vertex> tmp_face_vertices{ bl,br,tr,tl };

		float rotation_amnt = M_PI_2;
		for (size_t i = 0; i < 4; i++)
		{
			float angle = i * rotation_amnt;
			for (auto& vert : tmp_face_vertices) {
				vert.position = glm::rotateY(vert.position, angle);
				vert.normal = glm::rotateY(vert.normal, angle);

				vertices.push_back(vert);
			}

			indices.push_back(index);
			indices.push_back(index + 1);
			indices.push_back(index + 2);

			indices.push_back(index);
			indices.push_back(index + 2);
			indices.push_back(index + 3);

			index += 4;
		}

		//---Top facing trapezia

		norm = { 0,1,0 };//Normal now facing up
		bl={ {-half_length, next_height, half_length}, norm, { 0,0 } };
		br={ {half_length, next_height, half_length}, norm, { 0,1 } };
		tr={ {next_half_length, next_height, next_half_length}, norm, { 1,1 } };
		tl={ {-next_half_length, next_height, next_half_length}, norm, { 1,0 } };
		tmp_face_vertices={ bl,br,tr,tl };
		for (size_t i = 0; i < 4; i++)
		{
			float angle = i * rotation_amnt;
			for (auto& vert : tmp_face_vertices) {
				vert.position = glm::rotateY(vert.position, angle);

				vertices.push_back(vert);
			}

			indices.push_back(index);
			indices.push_back(index + 1);
			indices.push_back(index + 2);

			indices.push_back(index);
			indices.push_back(index + 2);
			indices.push_back(index + 3);

			index += 4;
		}


		current_side_length = next_length;
		current_height = next_height;
	}

	m_mesh = engine::mesh::create(vertices, indices);
}

engine::stepped_pyramid::~stepped_pyramid()
{
}

engine::ref<engine::stepped_pyramid> engine::stepped_pyramid::create(float height, float top_side_length, float bottom_side_length, int num_steps)
{
	return  std::make_shared<stepped_pyramid>(height,top_side_length,bottom_side_length,num_steps);
}
