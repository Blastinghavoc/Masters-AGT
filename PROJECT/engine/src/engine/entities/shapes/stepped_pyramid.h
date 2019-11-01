#pragma once
#include <engine.h>

namespace engine
{

	/*
	This class represent a generalised stepped pyramid. That is to say, a pyramid made by stacking
	any polygon. For example, a square-based stepped pyramid is created when num_sides = 4,
	triangular based when num_sides = 3 and N-gon based when num_sides = N.
	*/
	class stepped_pyramid
	{
	public:
		stepped_pyramid(float height,float top_radius,float bottom_radius,int num_steps, float border_fraction = 0.f, int num_sides = 4);

		~stepped_pyramid();

		//This shape has separate meshes for the faces and borders (if any) so that they can be independently textured.
		std::vector < ref<engine::mesh>> meshes() const { return m_meshes; }

		static ref<stepped_pyramid> create(float height, float top_radius, float bottom_radius, int num_steps,float border_fraction = 0.f, int num_sides = 4);
	private:
		std::vector< ref<engine::mesh>> m_meshes{};

		void add_quads(int num_sides, float two_theta, std::vector<engine::mesh::vertex> tmp_vertices, std::vector<engine::mesh::vertex>& vertices, std::vector<uint32_t>& indices, int& index, glm::vec3 rotation_axis);
		void add_tris(int num_sides, float two_theta, std::vector<engine::mesh::vertex> tmp_vertices, std::vector<engine::mesh::vertex>& vertices, std::vector<uint32_t>& indices, int& index, glm::vec3 rotation_axis);
	};
}
