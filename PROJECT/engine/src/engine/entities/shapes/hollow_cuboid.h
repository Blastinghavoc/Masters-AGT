#pragma once

#include <engine.h>

namespace engine {

	class hollow_cuboid
	{
	public:
		hollow_cuboid(glm::vec3 half_extents, float border_width,float tex_size = 1);

		~hollow_cuboid();

		std::vector < ref<engine::mesh>> meshes() const { return m_meshes; }

		static ref<hollow_cuboid> create(glm::vec3 half_extents, float border_width, float tex_size = 1);
	private:
		std::vector< ref<engine::mesh>> m_meshes{};
	};

}
