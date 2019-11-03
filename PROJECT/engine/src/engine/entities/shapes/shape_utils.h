#pragma once

#include "engine.h"

namespace engine {

	//Helper methods for shapes
	class shape_utils {
	public:
		//Add a quad from the given "tmp_vertices", replicated "num_sides" times, rotated by "angle" around "rotation_axis" each time. Put results in "vertices" and "indices", using "index" as the current index. 
		static void add_quads(uint32_t num_sides, float angle, std::vector<engine::mesh::vertex> tmp_vertices, std::vector<engine::mesh::vertex>& vertices, std::vector<uint32_t>& indices, uint32_t& index, glm::vec3 rotation_axis);
		static void add_tris(uint32_t num_sides, float angle, std::vector<engine::mesh::vertex> tmp_vertices, std::vector<engine::mesh::vertex>& vertices, std::vector<uint32_t>& indices, uint32_t& index, glm::vec3 rotation_axis);
	};
	
}
