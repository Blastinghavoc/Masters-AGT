#include "pch.h"
#include "shape_utils.h"
#include <glm/gtx/rotate_vector.hpp>

void engine::shape_utils::add_quads(uint32_t num_sides, float angle, std::vector<engine::mesh::vertex> tmp_vertices, std::vector<engine::mesh::vertex>& vertices, std::vector<uint32_t>& indices, uint32_t& index, glm::vec3 rotation_axis)
{
	for (size_t i = 0; i < num_sides; ++i)
	{
		float tmp_angle = i * angle;
		for (auto tmp_vert : tmp_vertices) {
			tmp_vert.position = glm::rotate(tmp_vert.position, tmp_angle, rotation_axis);
			tmp_vert.normal = glm::rotate(tmp_vert.normal, tmp_angle, rotation_axis);

			vertices.push_back(tmp_vert);
		}

		indices.push_back(index);
		indices.push_back(index + 1);
		indices.push_back(index + 2);

		indices.push_back(index);
		indices.push_back(index + 2);
		indices.push_back(index + 3);

		index += 4;
	}
}

void engine::shape_utils::add_tris(uint32_t num_sides, float angle, std::vector<engine::mesh::vertex> tmp_vertices, std::vector<engine::mesh::vertex>& vertices, std::vector<uint32_t>& indices, uint32_t& index, glm::vec3 rotation_axis)
{
	for (size_t i = 0; i < num_sides; ++i)
	{
		float tmp_angle = i * angle;
		for (auto tmp_vert : tmp_vertices) {
			tmp_vert.position = glm::rotate(tmp_vert.position, tmp_angle, rotation_axis);
			tmp_vert.normal = glm::rotate(tmp_vert.normal, tmp_angle, rotation_axis);

			vertices.push_back(tmp_vert);
		}

		indices.push_back(index);
		indices.push_back(index + 1);
		indices.push_back(index + 2);

		index += 3;
	}
}
