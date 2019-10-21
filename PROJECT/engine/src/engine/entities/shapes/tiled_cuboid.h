#pragma once

namespace engine
{
	class mesh;

	// Class creating a cuboid object with a mesh of a specified size, that has its face textures tiled.
	class tiled_cuboid
	{
	public:

		tiled_cuboid(glm::vec3 half_extents, bool inwards, glm::vec2 tile_scale);

		~tiled_cuboid();

		glm::vec3 half_extents() const { return m_half_extents; }
		bool inwards() const { return m_inwards; }
		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<tiled_cuboid> create(glm::vec3 half_extents, bool inwards, glm::vec2 tile_scale);
	private:

		glm::vec3 m_half_extents;

		bool m_inwards;

		ref<engine::mesh> m_mesh;
	};
}
